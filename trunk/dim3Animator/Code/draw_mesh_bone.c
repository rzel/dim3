/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Model Mesh and Bones

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;

extern d3rect					model_box;

/* =======================================================

      Draw Mesh
      
======================================================= */

void draw_model_mesh(int mesh_idx)
{
	int					n,ntrig;
    model_trig_type		*trig;
	model_mesh_type		*mesh;
	
		// draw the mesh
		
	mesh=&model.meshes[mesh_idx];
		
	glVertexPointer(3,GL_FLOAT,0,draw_setup.mesh_arrays[mesh_idx].gl_vertex_array);
	glLockArraysEXT(0,mesh->nvertex);
	
	glColor4f(0.75f,0.75f,0.75f,1.0f);
    
	ntrig=mesh->ntrig;
	trig=mesh->trigs;
    
    for (n=0;n!=ntrig;n++) {
	
		if (!vertex_check_hide_mask_trig(mesh_idx,trig)) {
			glBegin(GL_LINE_LOOP);
			glArrayElement(trig->v[0]);
			glArrayElement(trig->v[1]);
			glArrayElement(trig->v[2]);
			glEnd();
		}
		
		trig++;
    }
    
	glUnlockArraysEXT();
}

/* =======================================================

      Draw Bones Drag Handles
      
======================================================= */

float draw_model_bones_drag_handle_offset(void)
{
	float				f;
	
	f=(float)(model.view_box.size.x+model.view_box.size.z+model.view_box.size.y)/3.0f;
	f*=0.15;
	
	if (f<20.0f) return(20.0f);
	if (f>250.0f) return(250.0f);
	
	return(f);
}

void draw_model_bones_drag_handle_calc(d3fpnt *bone_pnt,d3vct *vct,d3ang *ang,d3fpnt *hand_pnt)
{
	matrix_type		mat;
	
		// handle coordinates
	
	if (ang->x!=0) {
		matrix_rotate_x(&mat,ang->x);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
	if (ang->y!=0) {
		matrix_rotate_y(&mat,ang->y);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
	if (ang->z!=0) {
		matrix_rotate_z(&mat,ang->z);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
	hand_pnt->x=bone_pnt->x-vct->x;
	hand_pnt->y=bone_pnt->y-vct->y;
	hand_pnt->z=bone_pnt->z-vct->z;
}

void draw_model_bones_drag_handle(d3fpnt *pnt,d3vct *vct,d3ang *ang,d3col *col)
{
	d3fpnt			hand_pnt;
	
		// handle coordinates
		
	draw_model_bones_drag_handle_calc(pnt,vct,ang,&hand_pnt);
		
		// line
		
	glColor4f(col->r,col->g,col->b,1);
		
	glLineWidth(2);
	
	glBegin(GL_LINES);
	glVertex3f(pnt->x,pnt->y,pnt->z);
	glVertex3f(hand_pnt.x,hand_pnt.y,hand_pnt.z);
	glEnd();
	
	glLineWidth(1);
	
	glPointSize(10);
	
	glBegin(GL_POINTS);
	glVertex3f(hand_pnt.x,hand_pnt.y,hand_pnt.z);
	glEnd();
	
	glPointSize(1);
}

/* =======================================================

      Find Rotation for Handles
      
======================================================= */

void draw_model_bones_get_handle_rot(int bone_idx,d3ang *rot)
{
	int						parent_idx;
	model_draw_bone_type	*draw_bone;
	
	draw_bone=&draw_setup.bones[bone_idx];
	
		// non-comulative rotations
		
	rot->x=draw_bone->rot.x;
	rot->y=draw_bone->rot.y;
	rot->z=draw_bone->rot.z;

	if (model.deform_mode!=deform_mode_comulative_rotate) return;
	
		// cumulative rotations
		
	parent_idx=bone_idx;
	
	while (TRUE) {
		parent_idx=draw_bone->parent_idx;
		if (parent_idx==-1) break;
		
		draw_bone=&draw_setup.bones[parent_idx];

		rot->x+=draw_bone->rot.x;
		rot->y+=draw_bone->rot.y;
		rot->z+=draw_bone->rot.z;
	}
}

/* =======================================================

      Draw Bones
      
======================================================= */

void draw_model_bones(int sel_bone_idx)
{
	int						n,nbone,parent_idx;
	float					x_move,z_move,y_move,
							bone_drag_handle_offset;
	d3fpnt					bone_pnt;
	d3vct					vct;
	d3ang					ang,rot;
	d3col					col;
	model_draw_bone_type	*draw_bone,*parent_bone;
	
	glDisable(GL_DEPTH_TEST);

		// draw the bones

	nbone=model.nbone;
    
    x_move=(float)draw_setup.move.x;
    z_move=(float)draw_setup.move.z;
    y_move=(float)draw_setup.move.y;
	
        // connections
        
	glColor4f(0.2f,0.2f,0.2f,1.0f);
	glLineWidth(2);
	
	glBegin(GL_LINES);
	
	draw_bone=draw_setup.bones;
	
	for (n=0;n!=nbone;n++) {
        parent_idx=draw_bone->parent_idx;
        if (parent_idx!=-1) {
            parent_bone=&draw_setup.bones[parent_idx];
			
			bone_pnt.x=draw_bone->fpnt.x+x_move;
			bone_pnt.y=draw_bone->fpnt.y+y_move;
			bone_pnt.z=draw_bone->fpnt.z+z_move;

            glVertex3f(bone_pnt.x,bone_pnt.y,bone_pnt.z);
			
			bone_pnt.x=parent_bone->fpnt.x+x_move;
			bone_pnt.y=parent_bone->fpnt.y-y_move;
			bone_pnt.z=parent_bone->fpnt.z-z_move;
			
            glVertex3f(bone_pnt.x,bone_pnt.y,bone_pnt.z);
        }
		
		draw_bone++;
	}
	
	glEnd();
	
	glLineWidth(1);
        
        // bones
		
	draw_bone=draw_setup.bones;
	
	bone_drag_handle_offset=draw_model_bones_drag_handle_offset();
	
	for (n=0;n!=nbone;n++) {
	
			// bone position
			
		bone_pnt.x=draw_bone->fpnt.x+x_move;
		bone_pnt.y=draw_bone->fpnt.y+y_move;
		bone_pnt.z=draw_bone->fpnt.z+z_move;
	
			// selected or normal
	
		if (sel_bone_idx==n) {

			glPointSize(10.0f);
			
				// draw selected bone
				
			glColor4f(1.0f,0.0f,1.0f,1.0f);
			
			glBegin(GL_POINTS);
			glVertex3f(bone_pnt.x,bone_pnt.y,bone_pnt.z);
			glEnd();
		
				// draw drag handles
				
			if (state.cur_pose_idx!=-1) {
				draw_model_bones_get_handle_rot(n,&rot);
					
				vct.x=bone_drag_handle_offset;
				vct.y=0.0f;
				vct.z=0.0f;
				ang.x=0.0f;
				ang.y=rot.y;
				ang.z=rot.z;
				col.r=1.0f;
				col.g=0.0f;
				col.b=0.0f;
				draw_model_bones_drag_handle(&bone_pnt,&vct,&ang,&col);
				
				vct.x=0.0f;
				vct.y=bone_drag_handle_offset;
				vct.z=0.0f;
				ang.x=rot.x;
				ang.y=0.0f;
				ang.z=rot.z;
				col.r=0.0f;
				col.g=1.0f;
				col.b=0.0f;
				draw_model_bones_drag_handle(&bone_pnt,&vct,&ang,&col);
				
				vct.x=0.0f;
				vct.y=0.0f;
				vct.z=bone_drag_handle_offset;
				ang.x=rot.x;
				ang.y=rot.y;
				ang.z=0.0f;
				col.r=0.0f;
				col.g=0.0f;
				col.b=1.0f;
				draw_model_bones_drag_handle(&bone_pnt,&vct,&ang,&col);
			}
		}
		else {
		
				// draw regular bone
				
			glPointSize(8.0f);
			glColor4f(1.0f,0.0f,0.0f,1.0f);
			
			glBegin(GL_POINTS);
			glVertex3f(bone_pnt.x,bone_pnt.y,bone_pnt.z);
			glEnd();
		}
		
		draw_bone++;
	}
	
	glPointSize(1);
}

void draw_model_bone_names(int sel_bone_idx)
{
	int						n,nbone;
	float					x_move,z_move,y_move;
	d3pnt					tran_pnt;
	d3fpnt					pnt;
	d3col					col;
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone;
	
	glDisable(GL_DEPTH_TEST);
	
		// get translation matrix
				
	draw_model_2D_transform_setup();
	draw_model_gl_setup_2D();
	
		// draw the bone names
		
	col.r=col.b=1.0f;
	col.g=0.0f;
		
	nbone=model.nbone;
    
    x_move=(float)draw_setup.move.x;
    z_move=(float)draw_setup.move.z;
    y_move=(float)draw_setup.move.y;
		
	bone=model.bones;
	draw_bone=draw_setup.bones;
	
	for (n=0;n!=nbone;n++) {
	
			// bone position
			
		pnt.x=draw_bone->fpnt.x+x_move;
		pnt.y=draw_bone->fpnt.y+y_move;
		pnt.z=draw_bone->fpnt.z+z_move;
		
		draw_model_2D_transform(&pnt,&tran_pnt);
		
			// text
			
		if (n!=sel_bone_idx) {
			text_draw_center(tran_pnt.x,(tran_pnt.y+50),12.0f,NULL,bone->name);
		}
		else {
			text_draw_center(tran_pnt.x,(tran_pnt.y+50),12.0f,&col,bone->name);
		}
		
		bone++;
		draw_bone++;
	}
	
	glEnable(GL_DEPTH_TEST);
}

