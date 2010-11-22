/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Render Setup

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"

extern setup_type			setup;
extern render_info_type		render_info;

SDL_Window					*sdl_wind;
SDL_GLContext				*sdl_gl_ctx;

int							orig_scrn_x,orig_scrn_y,orig_scrn_refresh;

/* =======================================================

      Detect if in Windowed Mode
      
======================================================= */

inline bool gl_in_window_mode(void)
{
	return((setup.window) || ((setup.editor_override.on) && (setup.window_editor)));
}

/* =======================================================

      GL Context Settings
      
======================================================= */

void gl_setup_context(void)
{
		// perspective correction
		
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
		
		// smoothing
		
	glDisable(GL_DITHER);
	
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	
		// texture compression
		
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_NICEST);
	glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
}

/* =======================================================

      Open and Close OpenGL Context
      
======================================================= */

bool gl_initialize(int screen_wid,int screen_high,bool lock_fps_refresh,int fsaa_mode,bool reset,char *err_str)
{
	int					sdl_flags;
    GLint				ntxtunit,ntxtsize;
#ifdef D3_OS_MAC
    long				rect[4];
	CGLContextObj		current_ctx;
#else
	GLenum				glew_error;
#endif
	SDL_DisplayMode		dsp_mode;
		
		// setup rendering sizes
        
	setup.screen.x_sz=screen_wid;
	setup.screen.y_sz=screen_high;
	
		// normal attributes
		
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	
		// full screen anti-aliasing attributes
		
	switch (fsaa_mode) {
		case fsaa_mode_2x:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,2);
			break;
		case fsaa_mode_4x:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
			break;
		case fsaa_mode_8x:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,8);
			break;
	}

		// start window or full screen
		
	sdl_flags=SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN;
	if (!gl_in_window_mode()) sdl_flags|=SDL_WINDOW_FULLSCREEN;
	
	sdl_wind=SDL_CreateWindow("dim3",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,setup.screen.x_sz,setup.screen.y_sz,sdl_flags);
	if (sdl_wind==NULL) {
		sprintf(err_str,"SDL: Could not create window (Error: %s)",SDL_GetError());
		return(FALSE);
	}
	
	sdl_gl_ctx=SDL_GL_CreateContext(sdl_wind);
	
		// use glew on linux and windows
		
#ifndef D3_OS_MAC
	glew_error=glewInit();
	if (glew_error!=GL_NO_ERROR) {
		strcpy(err_str,glewGetErrorString(glew_error));
		return(FALSE);
	}
#endif

		// grab openGL attributes
		
	strncpy(render_info.name,(char*)glGetString(GL_RENDERER),64);
	render_info.name[63]=0x0;
	
	strncpy(render_info.ext_string,(char*)glGetString(GL_EXTENSIONS),8192);
	render_info.ext_string[8191]=0x0;
			
	glGetIntegerv(GL_MAX_TEXTURE_UNITS,&ntxtunit);
	render_info.texture_unit_count=(int)ntxtunit;

	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&ntxtsize);
	render_info.texture_max_size=(int)ntxtsize;
	
	gl_check_initialize();

		// in case screen is bigger than window
		
	SDL_GetWindowSize(sdl_wind,&render_info.monitor_x_sz,&render_info.monitor_y_sz);

	render_info.view_x=(render_info.monitor_x_sz-setup.screen.x_sz)>>1;
	render_info.view_y=(render_info.monitor_y_sz-setup.screen.y_sz)>>1;
	
		// determine the referesh rate

	SDL_GetWindowDisplayMode(sdl_wind,&dsp_mode);

	render_info.monitor_refresh_rate=dsp_mode.refresh_rate;
	if (render_info.monitor_refresh_rate==0) render_info.monitor_refresh_rate=60;

        // clear the entire window so it doesn't flash
        
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
   
	SDL_GL_SwapWindow(sdl_wind);

        // setup renderer

#ifdef D3_OS_MAC
	current_ctx=CGLGetCurrentContext();
	
	rect[0]=render_info.view_x;
	rect[1]=render_info.view_y;
	rect[2]=setup.screen.x_sz;
	rect[3]=setup.screen.y_sz;
 
 	CGLSetParameter(current_ctx,kCGLCPSwapRectangle,rect);
	CGLEnable(current_ctx,kCGLCESwapRectangle);
#endif

	if (lock_fps_refresh) SDL_GL_SetSwapInterval(1);

	glViewport(render_info.view_x,render_info.view_y,setup.screen.x_sz,setup.screen.y_sz);
	
	gl_setup_context();
		
	if (fsaa_mode!=fsaa_mode_none) glEnable(GL_MULTISAMPLE);
	
		// all alphas by 8 bit component
		
	glDisable(GL_ALPHA_TEST);

		// texture utility initialize
		
	gl_texture_initialize();
	
		// do an initial draw
		
	if (!reset) {
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		SDL_GL_SwapWindow(sdl_wind);
	}
	
	return(TRUE);
}
	
void gl_shutdown(void)
{
	gl_texture_shutdown();
	
	SDL_GL_DeleteContext(sdl_gl_ctx);
	SDL_DestroyWindow(sdl_wind);
}

/* =======================================================

      Check Widescreen
      
======================================================= */

bool gl_is_size_widescreen(int wid,int high)
{
	return(((float)high/(float)wid)<=0.625f);
}

bool gl_is_screen_widescreen(void)
{
	return(gl_is_size_widescreen(setup.screen.x_sz,setup.screen.y_sz));
}

/* =======================================================

      ScreenShots
      
======================================================= */

bool gl_screen_shot(int lft_x,int top_y,int wid,int high,bool thumbnail,char *path)
{
	int					x,y,x_skip,y_skip,
						ss_wid,ss_high,dsz;
	unsigned char		*pixel_buffer,*data,*sptr,*dptr,*s2ptr,*d2ptr;
	bool				ok;
	
	pixel_buffer=(unsigned char*)malloc((wid*3)*high);
	if (pixel_buffer==NULL) return(FALSE);
	
	glReadPixels(lft_x,top_y,wid,high,GL_RGB,GL_UNSIGNED_BYTE,pixel_buffer);
	
		// is there a limiting?
		
	x_skip=y_skip=1;
	ss_wid=wid;
	ss_high=high;
	
	if (thumbnail) {
		x_skip=wid/128;
		y_skip=high/128;
		ss_wid=ss_high=128;
	}
	
		// flip the data
		
	dsz=((ss_wid*3)*ss_high);
	
	data=(unsigned char*)malloc(dsz);
	if (data==NULL) {
		free(pixel_buffer);
		return(FALSE);
	}
	
	bzero(data,dsz);
	
	sptr=pixel_buffer;
	dptr=data+((ss_high-1)*(ss_wid*3));

	for (y=0;y!=ss_high;y++) {
	
		s2ptr=sptr;
		d2ptr=dptr;
		
		for (x=0;x!=ss_wid;x++) {
			*d2ptr++=*s2ptr++;
			*d2ptr++=*s2ptr++;
			*d2ptr++=*s2ptr++;
			
			s2ptr+=((x_skip-1)*3);
		}
			
		sptr+=((wid*3)*y_skip);
		dptr-=(ss_wid*3);
	}

	free(pixel_buffer);

		// save screenshot

	ok=bitmap_write_png_data(data,ss_wid,ss_high,FALSE,path);
		
	free(data);
	
	return(ok);
}
	
