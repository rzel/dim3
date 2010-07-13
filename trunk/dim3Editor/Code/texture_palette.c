/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Texture Palette

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
	#include "dim3editor.h"
#endif

#include "walk_view.h"
#include "interface.h"
#include "dialog.h"
#include "common_view.h"

#define txt_wind_per_page_count		32

extern map_type				map;
extern editor_state_type	state;

int							txt_page,txt_offset,txt_pixel_sz;
d3rect						txt_palette_box;

/* =======================================================

      Texture Palette Setup
      
======================================================= */

void texture_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	txt_pixel_sz=((wbox.rx-16)-(wbox.lx+64))/txt_wind_per_page_count;
	
	txt_palette_box.lx=wbox.lx;
	txt_palette_box.rx=wbox.rx;
	txt_palette_box.ty=wbox.by-txt_pixel_sz;
	txt_palette_box.by=(txt_palette_box.ty+txt_pixel_sz)+1;
}

/* =======================================================

      Get and Put Selection Texture
      
======================================================= */

int texture_palette_get_selected_texture(void)
{
	int					type,main_idx,poly_idx;
	
	if (select_count()!=1) return(-1);
	
	select_get(0,&type,&main_idx,&poly_idx);
	
	if (type==liquid_piece) {
		if (walk_view_get_uv_layer()==uv_layer_normal) return(map.liquid.liquids[main_idx].txt_idx);
		return(map.liquid.liquids[main_idx].lmap_txt_idx);
	}
	
	if (type==mesh_piece) {
		if (walk_view_get_uv_layer()==uv_layer_normal) return(map.mesh.meshes[main_idx].polys[poly_idx].txt_idx);
		return(map.mesh.meshes[main_idx].polys[poly_idx].lmap_txt_idx);
	}
	
	return(-1);
}

void texture_palette_put_selected_texture(int txt_idx)
{
	int					n,k,sel_count,type,main_idx,poly_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
	
		select_get(n,&type,&main_idx,&poly_idx);
		
			// liquids
			
		if (type==liquid_piece) {
			if (walk_view_get_uv_layer()==uv_layer_normal) {
				map.liquid.liquids[main_idx].txt_idx=txt_idx;
			}
			else {
				map.liquid.liquids[main_idx].lmap_txt_idx=txt_idx;
			}
			continue;
		}
		
			// meshes
			
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[main_idx];
		
			// only set polygon
			
		if (state.drag_mode==drag_mode_polygon) {
			if (walk_view_get_uv_layer()==uv_layer_normal) {
				mesh->polys[poly_idx].txt_idx=txt_idx;
			}
			else {
				mesh->polys[poly_idx].lmap_txt_idx=txt_idx;
			}
		}
		
			// set all mesh
			
		else {
			
			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				if (walk_view_get_uv_layer()==uv_layer_normal) {
					poly->txt_idx=txt_idx;
				}
				else {
					poly->lmap_txt_idx=txt_idx;
				}
				poly++;
			}
		
		}		
	}
}

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw(void)
{
	int					n,k,x,ty,by,sel,idx;
	bool				has_texture;
	texture_type		*texture;
	
		// viewport setup
		
	walk_view_set_viewport_box(&txt_palette_box,TRUE,FALSE);
	
		// texture page switch
	
	x=0;
	ty=txt_palette_box.ty;
	by=ty+(txt_pixel_sz>>1);

	for (n=0;n!=8;n++) {
	
			// time to move down?
			
		if (n==4) {
			x=0;
			ty=by;
			by=txt_palette_box.ty+txt_pixel_sz;
		}
		
			// draw page
			
		glColor4f(0.75f,0.75f,0.75f,1.0f);
		
		glBegin(GL_QUADS);
		glVertex2i(x,ty);
		glVertex2i((x+16),ty);
		glVertex2i((x+16),by);
		glVertex2i(x,by);
		glEnd();
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,ty);
		glVertex2i((x+16),ty);
		glVertex2i((x+16),by);
		glVertex2i(x,by);
		glEnd();
		
		if (txt_page==n) {
			glColor4f(0.3f,0.3f,1.0f,1.0f);
			
			glBegin(GL_QUADS);
			glVertex2i((x+1),(ty+1));
			glVertex2i((x+14),(ty+1));
			glVertex2i((x+14),(by-2));
			glVertex2i((x+1),(by-2));
			glEnd();
		}
		
			// mark if there are textures
			
		has_texture=FALSE;
		idx=n*txt_wind_per_page_count;
		
		for (k=idx;k!=(idx+txt_wind_per_page_count);k++) {
			if (map.textures[k].frames[0].bitmap.gl_id!=-1) {
				has_texture=TRUE;
				break;
			}
		}
		
		if (has_texture) {
			glColor4f(0.0f,0.0f,0.0f,1.0f);
			
			glBegin(GL_QUADS);
			glVertex2i((x+11),(by-5));
			glVertex2i((x+14),(by-5));
			glVertex2i((x+14),(by-2));
			glVertex2i((x+11),(by-2));
			glEnd();
		}
		
		x+=16;
	}

		// textures
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	
	x=64;
	ty=txt_palette_box.ty+1;
	by=(ty+txt_pixel_sz)-1;
		
	for (n=0;n!=txt_wind_per_page_count;n++) {
		texture=&map.textures[n+(txt_page*txt_wind_per_page_count)];
		
			// the textures
			
		if (texture->frames[0].bitmap.gl_id!=-1) {
			glBindTexture(GL_TEXTURE_2D,texture->frames[0].bitmap.gl_id);

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2i(x,ty);
			glTexCoord2f(1,0);
			glVertex2i((x+txt_pixel_sz),ty);
			glTexCoord2f(1,1);
			glVertex2i((x+txt_pixel_sz),by);
			glTexCoord2f(0,1);
			glVertex2i(x,by);
			glEnd();
		}
		
		x+=txt_pixel_sz;
	}
	
	glDisable(GL_TEXTURE_2D);
	
	glDisable(GL_ALPHA_TEST);
	
		// right fill
		
	glColor4f(0.75f,0.75f,0.75f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(x,ty);
	glVertex2i(txt_palette_box.rx,ty);
	glVertex2i(txt_palette_box.rx,by);
	glVertex2i(x,by);
	glEnd();
	
		// lines
		
	x=64;
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	for (n=0;n!=txt_wind_per_page_count;n++) {
		idx=n+(txt_page*txt_wind_per_page_count);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,ty);
		glVertex2i((x+txt_pixel_sz),ty);
		glVertex2i((x+txt_pixel_sz),by);
		glVertex2i(x,by);
		glEnd();
		
		x+=txt_pixel_sz;
	}
	
	glBegin(GL_LINES);
	glVertex2i(txt_palette_box.lx,ty);
	glVertex2i(txt_palette_box.rx,ty);
	glEnd();
	
		// selection
		
	sel=texture_palette_get_selected_texture();
	if (sel==-1) return;
	if ((sel<(txt_page*txt_wind_per_page_count)) || (sel>=((txt_page+1)*txt_wind_per_page_count))) return;
	
	x=((sel-(txt_page*txt_wind_per_page_count))*txt_pixel_sz)+64;
	
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(x,ty);
	glVertex2i((x+txt_pixel_sz),ty);
	glVertex2i((x+txt_pixel_sz),by);
	glVertex2i(x,by);
	glEnd();
}

/* =======================================================

      Reset Texture Palette
      
======================================================= */

void texture_palette_reset(void)
{
	int			sel;
	
		// reset texture set if necessary
		
	sel=texture_palette_get_selected_texture();
	if (sel!=-1) txt_offset=sel/8;
}

/* =======================================================

      Texture Clicking
      
======================================================= */

void texture_palette_click(d3pnt *pnt,bool dblclick)
{
	int				nsel,page;
	
		// move within palette
		
	pnt->y-=txt_palette_box.ty;
	
		// texture page change
		
	if (pnt->x<64) {
		page=((pnt->y/(txt_pixel_sz>>1))*4)+(pnt->x/16);
		if (txt_page!=page) {
			txt_page=page;
			main_wind_draw();
		}
		return;
	}
	
		// find clicked texture
	
	nsel=(pnt->x-64)/txt_pixel_sz;
	nsel+=(txt_page*txt_wind_per_page_count);
		
		// double clicks
		
	if (dblclick) {
		dialog_texture_setting_run(nsel);
		main_wind_draw();
		return;
	}
    
		// single clicks
		
	if (map.textures[nsel].frames[0].bitmap.gl_id==-1) return;
	
	texture_palette_put_selected_texture(nsel);
	texture_palette_reset();
	main_wind_draw();
}
