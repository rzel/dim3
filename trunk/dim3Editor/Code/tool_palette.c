/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Tool Palette

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;
extern file_path_setup_type		file_path_setup;

/* =======================================================

      Tool Palette Settings
      
======================================================= */

int tool_palette_pixel_size(void)
{
	int				pixel_sz;
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	pixel_sz=(wbox.rx-wbox.lx)/tool_count;
	if (pixel_sz>tool_button_size) pixel_sz=tool_button_size;
	
	return(pixel_sz);
}

void tool_palette_box(d3rect *box)
{
	int				pixel_sz;
	d3rect			wbox;
	
	pixel_sz=tool_palette_pixel_size();
	
	os_get_window_box(&wbox);
	
	box->lx=wbox.lx;
	box->rx=wbox.rx;
	box->ty=wbox.ty;
	box->by=box->ty+(pixel_sz+1);
}

/* =======================================================

      Tool Palette State
      
======================================================= */

bool tool_get_highlight_state(int tool_idx)
{
	editor_view_type		*view;

	switch (tool_idx) {

			// vertex mode

		case 0:
			return(state.map.vertex_mode==vertex_mode_none);
		case 1:
			return(state.map.vertex_mode==vertex_mode_lock);
		case 2:
			return(state.map.vertex_mode==vertex_mode_snap);

			// free look and select

		case 3:
			return(state.map.free_look);
		case 4:
			return(state.map.select_add);

			// drag mode

		case 5:
			return(state.map.drag_mode==drag_mode_mesh);
		case 6:
			return(state.map.drag_mode==drag_mode_polygon);
		case 7:
			return(state.map.drag_mode==drag_mode_vertex);
			
			// grids
			
		case 11:
			return(state.map.grid_mode==grid_mode_none);
		case 12:
			return(state.map.grid_mode==grid_mode_small);
		case 13:
			return(state.map.grid_mode==grid_mode_large);

			// auto texture

		case 14:
			return(state.map.auto_texture);

			// handle mode

		case 15:
			return(state.map.handle_mode==handle_mode_rotate);
		case 16:
			return(state.map.handle_mode==handle_mode_move);

			// node mode

		case 17:
			return(state.map.node_mode==node_mode_select);
		case 18:
			return(state.map.node_mode==node_mode_duplicate);
		case 19:
			return(state.map.node_mode==node_mode_link);
		case 20:
			return(state.map.node_mode==node_mode_remove_link);
			
			// normals and culling

		case 21:
			view=view_get_current_view();
			return(state.map.show_normals);
		case 22:
			view=view_get_current_view();
			return(view->cull);
	}

	return(FALSE);
}

bool tool_get_disabled_state(int tool_idx)
{
	return(!state.map.map_open);
}

/* =======================================================

      Tool Palette Button Clicks
      
======================================================= */

void tool_click(int tool_idx)
{
	editor_view_type		*view;

		// nothing to do if map closed

	if (!state.map.map_open) return;

		// if in texture view, any tool click
		// leaves it

	if (state.map.texture_edit_idx!=-1) texture_edit_switch(-1);

		// run the click

	switch (tool_idx) {
	
			// vertex mode
			
		case 0:
			state.map.vertex_mode=vertex_mode_none;
			break;
			
		case 1:
			state.map.vertex_mode=vertex_mode_lock;
			break;
			
		case 2:
			state.map.vertex_mode=vertex_mode_snap;
			break;
			
			// free look and selection toggle
			
		case 3:
			state.map.free_look=!state.map.free_look;
			break;

		case 4:
			state.map.select_add=!state.map.select_add;
			break;
			
			// drag mode buttons
			
		case 5:
			state.map.drag_mode=drag_mode_mesh;
			break;
			
		case 6:
			state.map.drag_mode=drag_mode_polygon;
			break;
			
		case 7:
			state.map.drag_mode=drag_mode_vertex;
			break;
			
			// mesh polygons
			
		case 8:
			piece_combine_mesh();
			break;
			
		case 9:
			piece_split_mesh();
			break;

		case 10:
			piece_tesselate(TRUE);
			break;
			
			
			// grids
			
		case 11:
			state.map.grid_mode=grid_mode_none;
			break;

		case 12:
			state.map.grid_mode=grid_mode_small;
			break;

		case 13:
			state.map.grid_mode=grid_mode_large;
			break;
			
			// auto-texture
			
		case 14:
			state.map.auto_texture=!state.map.auto_texture;
			break;
			
			// handle modes
			
		case 15:
			state.map.handle_mode=handle_mode_rotate;
			break;
			
		case 16:
			state.map.handle_mode=handle_mode_move;
			break;
			
			// node editing
			
		case 17:
			state.map.show_node=TRUE;
			menu_update_view();
			state.map.node_mode=node_mode_select;
			break;
			
		case 18:
			state.map.show_node=TRUE;
			menu_update_view();
			state.map.node_mode=node_mode_duplicate;
			break;
			
		case 19:
			state.map.show_node=TRUE;
			menu_update_view();
			state.map.node_mode=node_mode_link;
			break;
			
		case 20:
			state.map.show_node=TRUE;
			menu_update_view();
			state.map.node_mode=node_mode_remove_link;
			break;
			
			// normals
			
		case 21:
			state.map.show_normals=!state.map.show_normals;
			break;
			
		case 22:
			view=view_get_current_view();
			view_cull(!view->cull);
			break;
			
			// script and run buttons
			
		case 23:
			launch_map_script_editor();
			break;
			
		case 24:
			launch_engine();
			break;
			
			// pieces create
			
		case 26:
			piece_create_spot();
			break;
			
		case 27:
			piece_create_light();
			break;
			
		case 28:
			piece_create_sound();
			break;
			
		case 29:
			piece_create_particle();
			break;
			
		case 30:
			piece_create_scenery();
			break;
			
		case 31:
			piece_create_node();
			break;
			
		case 32:
			piece_add_obj_mesh();
			break;
			
		case 33:
			piece_add_obj_mesh_uv();
			break;
			
		case 34:
			piece_add_height_map_mesh();
			break;
			
		case 35:
			piece_add_grid_mesh();
			break;
			
		case 36:
			piece_add_polygon_mesh();
			break;
			
		case 37:
			piece_create_liquid();
			break;
			
	}
	
	menu_fix_enable();
	texture_palette_reset();
	
	main_wind_draw();
}

/* =======================================================

      Values and States
      
======================================================= */

void tool_default(void)
{
	state.map.free_look=pref.map.free_look;
	state.map.select_add=FALSE;
	
    state.map.vertex_mode=vertex_mode_none;
	state.map.drag_mode=drag_mode_mesh;
	state.map.grid_mode=grid_mode_small;
	state.map.node_mode=node_mode_select;
	state.map.handle_mode=handle_mode_rotate;
	
	state.map.auto_texture=pref.map.auto_texture;
	
	state.map.show_liquid=TRUE;
	state.map.show_object=TRUE;
	state.map.show_lightsoundparticle=TRUE;
	state.map.show_node=FALSE;
	
	state.map.show_normals=FALSE;
}

void tool_switch_vertex_mode(void)
{
	state.map.vertex_mode++;
	if (state.map.vertex_mode>vertex_mode_snap) state.map.vertex_mode=vertex_mode_none;
	
	main_wind_draw();
}

void tool_switch_drag_mode(void)
{
	state.map.drag_mode++;
	if (state.map.drag_mode>drag_mode_vertex) state.map.drag_mode=drag_mode_mesh;

	main_wind_draw();
}

void tool_switch_grid_mode(void)
{
	state.map.grid_mode++;
	if (state.map.grid_mode>grid_mode_large) state.map.grid_mode=grid_mode_none;
	
	main_wind_draw();
}

void tool_switch_node_mode(void)
{
	state.map.node_mode++;
	if (state.map.node_mode>node_mode_remove_link) state.map.node_mode=node_mode_select;
	
	main_wind_draw();
}



