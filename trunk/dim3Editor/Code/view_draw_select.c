/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Selection Drawing Routines

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;
extern editor_setup_type		setup;
extern editor_state_type		state;

/* =======================================================

      Draw Selection for Mesh
      
======================================================= */

void view_draw_select_mesh_get_grow_handles(int mesh_idx,d3pnt *pts)
{
	d3pnt			min,max;
	
	map_mesh_calculate_extent(&map,mesh_idx,&min,&max);
	
		// resize

	pts[0].x=pts[3].x=pts[4].x=pts[7].x=min.x;
	pts[0].z=pts[1].z=pts[4].z=pts[5].z=min.z;
	pts[1].x=pts[2].x=pts[5].x=pts[6].x=max.x;
	pts[2].z=pts[3].z=pts[6].z=pts[7].z=max.z;
	pts[0].y=pts[1].y=pts[2].y=pts[3].y=min.y;
	pts[4].y=pts[5].y=pts[6].y=pts[7].y=max.y;

		// skew x

	pts[8].x=pts[9].x=min.x;
	pts[10].x=pts[11].x=max.x;
	pts[8].y=pts[10].y=min.y;
	pts[9].y=pts[11].y=max.y;
	pts[8].z=pts[9].z=pts[10].z=pts[11].z=(min.z+max.z)>>1;

		// skew y

	pts[12].y=pts[13].y=min.y;
	pts[14].y=pts[15].y=max.y;
	pts[12].z=pts[14].z=min.z;
	pts[13].z=pts[15].z=max.z;
	pts[12].x=pts[13].x=pts[14].x=pts[15].x=(min.x+max.x)>>1;

		// skew z

	pts[16].z=pts[17].z=min.z;
	pts[18].z=pts[19].z=max.z;
	pts[16].x=pts[18].x=min.x;
	pts[17].x=pts[19].x=max.x;
	pts[16].y=pts[17].y=pts[18].y=pts[19].y=(min.y+max.y)>>1;
}

void view_draw_select_mesh(int mesh_idx)
{
	int						n,k,t;
	d3pnt					*pt,pts[20];
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// draw selected mesh
		
	glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
	
	glEnable(GL_DEPTH_TEST);
	
	mesh_poly=mesh->polys;
	
	for (k=0;k!=mesh->npoly;k++) {
	
		glBegin(GL_LINE_LOOP);
		
		for (t=0;t!=mesh_poly->ptsz;t++) {
			pt=&mesh->vertexes[mesh_poly->v[t]];
			glVertex3i(pt->x,pt->y,pt->z);
		}
		
		glEnd();
		
		mesh_poly++;
	}
	
		// is mesh only, draw resize handles
		
	if (state.drag_mode==drag_mode_mesh) {
		
		view_draw_select_mesh_get_grow_handles(mesh_idx,pts);

			// draw the resize handles

		glEnable(GL_DEPTH_TEST);

		glPointSize(view_handle_size);
		glBegin(GL_POINTS);

		for (n=0;n!=8;n++) {
			if (state.drag_handle_idx==n) {
				glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
			}
			else {
				glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
			}
			glVertex3i(pts[n].x,pts[n].y,pts[n].z);
		}

		glEnd();

			// draw the skew handles

		glPointSize(view_skew_handle_size);
		glBegin(GL_POINTS);

		for (n=8;n!=20;n++) {
			if (state.drag_handle_idx==n) {
				glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
			}
			else {
				glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
			}

			glVertex3i(pts[n].x,pts[n].y,pts[n].z);
		}

		glEnd();

		glPointSize(1.0f);
	}
}

void view_draw_select_mesh_vertex(int mesh_idx)
{
	int						n;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	
	glEnable(GL_DEPTH_TEST);

	glPointSize(view_handle_size);
	
	mesh=&map.mesh.meshes[mesh_idx];
	pt=mesh->vertexes;
	
	glBegin(GL_POINTS);

	for (n=0;n!=mesh->nvertex;n++) {
		if (state.drag_handle_idx==n) {
			glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
		}
		else {
			glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
		}
		glVertex3i(pt->x,pt->y,pt->z);
		
		pt++;
	}

	glEnd();
}

void view_draw_select_mesh_poly(int mesh_idx,int poly_idx)
{
	int						n;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
	glDisable(GL_DEPTH_TEST);
	
	glLineWidth(2.0f);

	glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
	
	mesh=&map.mesh.meshes[mesh_idx];
	mesh_poly=&mesh->polys[poly_idx];
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=mesh_poly->ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		glVertex3i(pt->x,pt->y,pt->z);
	}
	
	glEnd();

	glLineWidth(1.0f);
}

/* =======================================================

      Draw Selection for Liquid
      
======================================================= */

void view_draw_select_liquid_get_grow_handles(int liquid_idx,d3pnt *pts)
{
	map_liquid_type			*liq;
	
	liq=&map.liquid.liquids[liquid_idx];
	
	pts[0].x=pts[3].x=pts[4].x=pts[7].x=liq->lft;
	pts[1].x=pts[2].x=pts[5].x=pts[6].x=liq->rgt;
	pts[0].z=pts[1].z=pts[4].z=pts[5].z=liq->top;
	pts[2].z=pts[3].z=pts[6].z=pts[7].z=liq->bot;
	pts[0].y=pts[1].y=pts[2].y=pts[3].y=liq->y;
	pts[4].y=pts[5].y=pts[6].y=pts[7].y=liq->y+liq->depth;
}

void view_draw_select_liquid(int liquid_idx)
{
	int						n,y;
	d3pnt					pts[8];
	map_liquid_type			*liq;
	
	liq=&map.liquid.liquids[liquid_idx];
	
	glEnable(GL_DEPTH_TEST);

	glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
	
		// outline
		
	glBegin(GL_LINE_LOOP);
	glVertex3i(liq->lft,liq->y,liq->top);
	glVertex3i(liq->rgt,liq->y,liq->top);
	glVertex3i(liq->rgt,liq->y,liq->bot);
	glVertex3i(liq->lft,liq->y,liq->bot);
	glEnd();
	
	y=liq->y+liq->depth;
	
	glBegin(GL_LINE_LOOP);
	glVertex3i(liq->lft,y,liq->top);
	glVertex3i(liq->rgt,y,liq->top);
	glVertex3i(liq->rgt,y,liq->bot);
	glVertex3i(liq->lft,y,liq->bot);
	glEnd();
	
		// handles

	glPointSize(view_handle_size);
		
	view_draw_select_liquid_get_grow_handles(liquid_idx,pts);
		
	glBegin(GL_POINTS);

	for (n=0;n!=8;n++) {
		if (state.drag_handle_idx==n) {
			glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
		}
		else {
			glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
		}
		glVertex3i(pts[n].x,pts[n].y,pts[n].z);
	}

	glEnd();
}

/* =======================================================

      Draw Selection for Cube Items
      
======================================================= */

void view_draw_select_cube(d3pnt *v_pnts)
{
    glLineWidth(3.0f);
	glColor4f(1.0f,0.0f,0.0f,1.0f);
    
	glBegin(GL_LINE_LOOP);
	glVertex3i(v_pnts[0].x,v_pnts[0].y,v_pnts[0].z);
	glVertex3i(v_pnts[1].x,v_pnts[1].y,v_pnts[1].z);
	glVertex3i(v_pnts[2].x,v_pnts[2].y,v_pnts[2].z);
	glVertex3i(v_pnts[3].x,v_pnts[3].y,v_pnts[3].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3i(v_pnts[4].x,v_pnts[4].y,v_pnts[4].z);
	glVertex3i(v_pnts[5].x,v_pnts[5].y,v_pnts[5].z);
	glVertex3i(v_pnts[6].x,v_pnts[6].y,v_pnts[6].z);
	glVertex3i(v_pnts[7].x,v_pnts[7].y,v_pnts[7].z);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3i(v_pnts[0].x,v_pnts[0].y,v_pnts[0].z);
	glVertex3i(v_pnts[4].x,v_pnts[4].y,v_pnts[4].z);
	glVertex3i(v_pnts[1].x,v_pnts[1].y,v_pnts[1].z);
	glVertex3i(v_pnts[5].x,v_pnts[5].y,v_pnts[5].z);
	glVertex3i(v_pnts[2].x,v_pnts[2].y,v_pnts[2].z);
	glVertex3i(v_pnts[6].x,v_pnts[6].y,v_pnts[6].z);
	glVertex3i(v_pnts[3].x,v_pnts[3].y,v_pnts[3].z);
	glVertex3i(v_pnts[7].x,v_pnts[7].y,v_pnts[7].z);
	glEnd();
    
    glLineWidth(1.0f);
}

/* =======================================================

      Draw Rotation and Move Handles
      
======================================================= */

void view_draw_select_2D_rot_handle(d3pnt *pnt,d3pnt *hand_pnt,d3col *col)
{
    glLineWidth(view_handle_line_width);
	
	glColor4f(col->r,col->g,col->b,1.0f);
	
	glBegin(GL_LINES);
	glVertex2i(pnt->x,pnt->y);
	glVertex2i(hand_pnt->x,hand_pnt->y);
	glEnd();
	
    glLineWidth(1.0f);
	
	glPointSize(view_handle_size);
	
	glBegin(GL_POINTS);
	glVertex2i(hand_pnt->x,hand_pnt->y);
	glEnd();
	
	glPointSize(1.0f);
}

void view_draw_select_rot_handles(editor_view_type *view,d3pnt *pnt,d3ang *ang)
{
	d3pnt			center_pnt,hand_pnt[3];
	d3col			col;

		// create rot handles
		
	if (!view_handle_create_rot_handle(view,pnt,ang,&center_pnt,hand_pnt)) return;

		// draw points

	view_set_2D_projection(view);

	glDisable(GL_DEPTH_TEST);
	
	col.r=1.0f;
	col.g=0.0f;
	col.b=0.0f;
	
	view_draw_select_2D_rot_handle(&center_pnt,&hand_pnt[0],&col);
	
	col.r=0.0f;
	col.g=1.0f;
	col.b=0.0f;
	
	view_draw_select_2D_rot_handle(&center_pnt,&hand_pnt[1],&col);
	
	col.r=0.0f;
	col.g=0.0f;
	col.b=1.0f;
	
	view_draw_select_2D_rot_handle(&center_pnt,&hand_pnt[2],&col);
}

/* =======================================================

      Draw Selection Box
      
======================================================= */

void view_draw_select_box(editor_view_type *view)
{
	int				lx,rx,ty,by;
	d3rect			box;

		// selection in this view?

	if ((!state.select_box_on) || (view!=view_get_current_view())) return;

		// need the box to position

	view_get_pixel_box(view,&box);

	lx=state.select_box_start_pnt.x+box.lx;
	rx=state.select_box_end_pnt.x+box.lx;
	ty=state.select_box_start_pnt.y+box.ty;
	by=state.select_box_end_pnt.y+box.ty;

		// draw the selection

	view_set_2D_projection(view);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glColor4f(0.5f,0.5f,0.5f,0.7f);
	
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glColor4f(1.0f,1.0f,1.0f,0.7f);

	glBegin(GL_LINE_LOOP);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Draw Selections for Map
      
======================================================= */

void view_draw_select(editor_view_type *view)
{
	int						n,sel_count,
							type,main_idx,sub_idx;
	unsigned char			draw_mesh_once[max_mesh];
	d3pnt					v_pnts[8];

		// push view forward so selections
		// display properly

	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
	
		// if no selection, only the box
		// select can draw

	sel_count=select_count();
	if (sel_count==0) {
		view_draw_select_box(view);
		return;
	}

		// only draw mesh selections once
		// as there could be multiple polygon selections
		// for a single mesh
		
	memset(draw_mesh_once,0x0,max_mesh);
	
		// draw the selection of meshes, liquids,
		// and other pieces, drawing the selected
		// polygons later
		
	for (n=(sel_count-1);n>=0;n--) {
	
		select_get(n,&type,&main_idx,&sub_idx);
		
			// draw selection
			
		switch (type) {
		
			case mesh_piece:
				if (draw_mesh_once[main_idx]==0x0) {
					draw_mesh_once[main_idx]=0x1;
					view_draw_select_mesh(main_idx);
				}
				break;
				
			case liquid_piece:
				view_draw_select_liquid(main_idx);
				break;
				
			case node_piece:
				view_get_sprite_vertexes(&map.nodes[main_idx].pnt,NULL,v_pnts);
				view_draw_select_cube(v_pnts);
				view_draw_select_rot_handles(view,&map.nodes[main_idx].pnt,&map.nodes[main_idx].ang);
				break;
				
			case spot_piece:
				view_model_cube_vertexes(map.spots[main_idx].display_model,&map.spots[main_idx].pnt,&map.spots[main_idx].ang,1.0f,v_pnts);
				view_draw_select_cube(v_pnts);
				view_draw_select_rot_handles(view,&map.spots[main_idx].pnt,&map.spots[main_idx].ang);
				break;
				
			case scenery_piece:
				view_model_cube_vertexes(map.sceneries[main_idx].model_name,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,map.sceneries[main_idx].resize,v_pnts);
				view_draw_select_cube(v_pnts);
				view_draw_select_rot_handles(view,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang);
				break;
				
			case light_piece:
				view_get_sprite_vertexes(&map.lights[main_idx].pnt,NULL,v_pnts);
				view_draw_select_cube(v_pnts);
				break;
				
			case sound_piece:
				view_get_sprite_vertexes(&map.sounds[main_idx].pnt,NULL,v_pnts);
				view_draw_select_cube(v_pnts);
				break;
				
			case particle_piece:
				view_get_sprite_vertexes(&map.particles[main_idx].pnt,NULL,v_pnts);
				view_draw_select_cube(v_pnts);
				break;

		}
	}
	
		// draw selected polygons
		// draw afterwards so mesh highlighting doesn't
		// effect selection
		
	if (state.drag_mode==drag_mode_polygon) {
	
		for (n=(sel_count-1);n>=0;n--) {
		
			select_get(n,&type,&main_idx,&sub_idx);
			if (type!=mesh_piece) continue;
			
			view_draw_select_mesh_poly(main_idx,sub_idx);
		}
		
	}
	
		// draw selected mesh vertexes
		
	if (state.drag_mode==drag_mode_vertex) {

		memset(draw_mesh_once,0x0,max_mesh);
			
		for (n=(sel_count-1);n>=0;n--) {
		
			select_get(n,&type,&main_idx,&sub_idx);
			if (type!=mesh_piece) continue;
			
			if (draw_mesh_once[main_idx]==0x0) {
				draw_mesh_once[main_idx]=0x1;
				view_draw_select_mesh_vertex(main_idx);
			}
		}
	}

		// draw box selection

	view_draw_select_box(view);
}
			
