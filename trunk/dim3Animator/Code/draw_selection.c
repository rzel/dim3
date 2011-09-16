/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Model Selection and Normals

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
#include "interface.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_setup_type		setup;
extern animator_state_type		state;

/* =======================================================

      Draw Selected Vertexes
      
======================================================= */

void draw_model_selected_vertexes(int mesh_idx)
{
	int				n,nvertex;
	float			*pv;
	model_mesh_type	*mesh;

	glEnableClientState(GL_VERTEX_ARRAY);
	
	glPointSize(draw_vertex_handle_size);
	
	mesh=&model.meshes[mesh_idx];
	nvertex=mesh->nvertex;
	
	pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array;

	for (n=0;n!=nvertex;n++) {
	
		if ((!vertex_check_sel_mask(mesh_idx,n)) || (vertex_check_hide_mask(mesh_idx,n))) {
			pv+=3;
			continue;
		}
		
		if (mesh->vertexes[n].major_bone_idx!=-1) {
			if ((mesh->vertexes[n].major_bone_idx==state.cur_bone_idx) && (state.cur_bone_idx!=-1)) {
				glColor4f(1.0f,0.2f,1.0f,1.0f);
			}
			else {
				glColor4f(0.0f,0.0f,0.0f,1.0f);
			}
		}
		else {
			glColor4f(0.5f,0.5f,0.5f,1.0f);
		}
	
		glVertexPointer(3,GL_FLOAT,0,pv);
		glDrawArrays(GL_POINTS,0,1);
		
		pv+=3;
	}
	
	glPointSize(1.0f);

	glDisableClientState(GL_VERTEX_ARRAY);
}

/* =======================================================

      Draw Selected Trig
      
======================================================= */

void draw_model_selected_trig(int mesh_idx)
{
	int					n,k,ntrig,vertex_idx;
	float				vertexes[3*3];
	float				*pv,*pa;
	model_mesh_type		*mesh;
	model_trig_type		*trig;

	mesh=&model.meshes[mesh_idx];
	ntrig=mesh->ntrig;

	glEnableClientState(GL_VERTEX_ARRAY);

		// selection
	
	glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
	glLineWidth(draw_trig_select_line_size);

	for (n=0;n!=ntrig;n++) {

		if ((!trig_check_sel_mask(mesh_idx,n)) || (trig_check_hide_mask(mesh_idx,n))) continue;
		
		trig=&model.meshes[mesh_idx].trigs[n];

			// draw the selected trig

		pv=vertexes;

		for (k=0;k!=3;k++) {
			pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(3*trig->v[k]);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
		}
	
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,3);
	}
		
	glLineWidth(1.0f);

		// points
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(draw_vertex_handle_size);
		
	for (n=0;n!=ntrig;n++) {

		if ((!trig_check_sel_mask(mesh_idx,n)) || (trig_check_hide_mask(mesh_idx,n))) continue;
		
		trig=&model.meshes[mesh_idx].trigs[n];
		
			// draw any selected vertexes
			// on the trig
		
		for (k=0;k!=3;k++) {
			vertex_idx=trig->v[k];
			if ((vertex_check_sel_mask(mesh_idx,vertex_idx)) && (!vertex_check_hide_mask(mesh_idx,vertex_idx))) {
				pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(3*vertex_idx);

				glVertexPointer(3,GL_FLOAT,0,pa);
				glDrawArrays(GL_POINTS,0,1);
			}
		}
	}
		
	glPointSize(1.0f);

	glDisableClientState(GL_VERTEX_ARRAY);
}

/* =======================================================

      Draw Model Normals
      
======================================================= */

void draw_model_normals_vertexes(int mesh_idx)
{
	int				n,k,ntrig;
	float			fx,fy,fz,vertexes[6];
	float			*pv,*pn,*pt;
	bool			has_sel;
	d3vct			tangent,normal,binormal;
	model_trig_type	*trig;

	glEnableClientState(GL_VERTEX_ARRAY);
	
		// is there a vertex selection?
		
	has_sel=vertex_check_sel_any(mesh_idx);
	
		// draw normals
	
	glLineWidth(draw_model_normal_size);
	
	glBegin(GL_LINES);
	
	ntrig=model.meshes[mesh_idx].ntrig;

	for (n=0;n!=ntrig;n++) {
	
		trig=&model.meshes[mesh_idx].trigs[n];
		
		pn=draw_setup.mesh_arrays[mesh_idx].gl_normal_array+(n*9);
		pt=draw_setup.mesh_arrays[mesh_idx].gl_tangent_array+(n*9);
		
			// vertex normals
			
		for (k=0;k!=3;k++) {
				
			if (has_sel) {
				if (!vertex_check_sel_mask(mesh_idx,trig->v[k])) {
					pn+=3;
					pt+=3;
					continue;
				}
			}

				// vertex point

			pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[k]*3);
			fx=*pv++;
			fy=*pv++;
			fz=*pv;

			normal.x=*pn++;
			normal.y=*pn++;
			normal.z=*pn++;

			if (setup.show_tangent_binormal) {

					// tangent

				vertexes[0]=fx;
				vertexes[1]=fy;
				vertexes[2]=fz;

				tangent.x=*pt++;
				tangent.y=*pt++;
				tangent.z=*pt++;

				vertexes[3]=fx+(tangent.x*draw_model_normal_len);
				vertexes[4]=fy+(tangent.y*draw_model_normal_len);
				vertexes[5]=fz+(tangent.z*draw_model_normal_len);
			
				glVertexPointer(3,GL_FLOAT,0,vertexes);

				glColor4f(1.0f,0.0f,0.0f,1.0f);
				glDrawArrays(GL_LINES,0,2);

					// binormal

				vector_cross_product(&binormal,&tangent,&normal);

				vertexes[0]=fx;
				vertexes[1]=fy;
				vertexes[2]=fz;

				vertexes[3]=fx+(binormal.x*draw_model_normal_len);
				vertexes[4]=fy+(binormal.y*draw_model_normal_len);
				vertexes[5]=fz+(binormal.z*draw_model_normal_len);

				glVertexPointer(3,GL_FLOAT,0,vertexes);

				glColor4f(0.0f,0.0f,1.0f,1.0f);
				glDrawArrays(GL_LINES,0,2);
			}

				// normal

			vertexes[0]=fx;
			vertexes[1]=fy;
			vertexes[2]=fz;

			vertexes[3]=fx+(normal.x*draw_model_normal_len);
			vertexes[4]=fy+(normal.y*draw_model_normal_len);
			vertexes[5]=fz+(normal.z*draw_model_normal_len);

			glVertexPointer(3,GL_FLOAT,0,vertexes);

			glColor4f(1.0f,0.0f,1.0f,1.0f);
			glDrawArrays(GL_LINES,0,2);
		}
	}
	
	glEnd();
	
	glLineWidth(1.0f);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void draw_model_normals_trig(int mesh_idx)
{
	int				n,k,ntrig;
	float			fx,fy,fz,fx2,fy2,fz2;
	float			*pv,*pn,*pt;
	d3vct			tangent,normal,binormal;
	model_trig_type	*trig;
	
	glLineWidth(draw_model_normal_size);
	glColor4f(1.0f,0.0f,1.0f,1.0f);
	
	glBegin(GL_LINES);

	ntrig=model.meshes[mesh_idx].ntrig;

	for (n=0;n!=ntrig;n++) {
		
		if ((!trig_check_sel_mask(mesh_idx,n)) || (trig_check_hide_mask(mesh_idx,n))) continue;
		
		trig=&model.meshes[mesh_idx].trigs[n];

			// draw trig normals
		
		pn=draw_setup.mesh_arrays[mesh_idx].gl_normal_array+(n*9);
		pt=draw_setup.mesh_arrays[mesh_idx].gl_tangent_array+(n*9);
			
		for (k=0;k!=3;k++) {

			if (!vertex_check_sel_mask(mesh_idx,trig->v[k])) {
				pn+=3;
				pt+=3;
				continue;
			}
		
			pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[k]*3);
			fx=*pv++;
			fy=*pv++;
			fz=*pv;

			normal.x=*pn++;
			normal.y=*pn++;
			normal.z=*pn++;

			if (setup.show_tangent_binormal) {

					// tangent

				tangent.x=*pt++;
				tangent.y=*pt++;
				tangent.z=*pt++;

				glColor4f(1.0f,0.0f,0.0f,1.0f);

				glVertex3f(fx,fy,fz);
				
				fx2=fx+(tangent.x*draw_model_normal_len);
				fy2=fy+(tangent.y*draw_model_normal_len);
				fz2=fz+(tangent.z*draw_model_normal_len);

				glVertex3f(fx2,fy2,fz2);

					// binormal

				vector_cross_product(&binormal,&tangent,&normal);

				glColor4f(0.0f,0.0f,1.0f,1.0f);

				glVertex3f(fx,fy,fz);
				
				fx2=fx+(binormal.x*draw_model_normal_len);
				fy2=fy+(binormal.y*draw_model_normal_len);
				fz2=fz+(binormal.z*draw_model_normal_len);

				glVertex3f(fx2,fy2,fz2);
			}

				// normal

			glColor4f(1.0f,0.0f,1.0f,1.0f);

			glVertex3f(fx,fy,fz);
			
			fx2=fx+(normal.x*draw_model_normal_len);
			fy2=fy+(normal.y*draw_model_normal_len);
			fz2=fz+(normal.z*draw_model_normal_len);

			glVertex3f(fx2,fy2,fz2);
		}
	}

	glEnd();
	glLineWidth(1.0f);
}

