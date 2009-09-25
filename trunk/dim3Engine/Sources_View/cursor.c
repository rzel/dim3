/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Cursor Routines

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"
#include "inputs.h"

extern hud_type				hud;
extern setup_type			setup;

bitmap_type					cursor_bitmap;

/* =======================================================

      Cursor Initialize and Shutdown
      
======================================================= */

void cursor_initialize(void)
{
	char		path[1024];
	
	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","cursor","png");
	bitmap_open(&cursor_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
}

void cursor_shutdown(void)
{
	bitmap_close(&cursor_bitmap);
}

/* =======================================================

      Crosshair Draw
      
======================================================= */

void cursor_draw(void)
{
	int				x,y,sz,lft,top,rgt,bot;
	
		// get cursor position
		
	input_gui_get_mouse_position(&x,&y);

	sz=(int)(((float)hud.scale_x)*cursor_size_factor);
		
	lft=x;
	rgt=lft+sz;
	top=y;
	bot=top+sz;

		// draw mouse
		
	view_draw_next_vertex_object_2D_texture_quad(cursor_bitmap.gl_id,NULL,1.0f,lft,rgt,top,bot,0.0f,0.0f);
}


