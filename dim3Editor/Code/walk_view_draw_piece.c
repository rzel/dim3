/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Draw Piece Routines

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

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int				cy,main_wind_uv_layer,txt_palette_high,area_col_type;
extern float			walk_view_fov,walk_view_y_angle,walk_view_x_angle;
extern bool				dp_normals,dp_liquid,dp_object,dp_lightsoundparticle,dp_node,dp_textured;
extern d3pnt			view_pnt;

extern AGLContext		ctx;

extern map_type			map;
extern setup_type		setup;
extern bitmap_type		spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
						light_bitmap,sound_bitmap,particle_bitmap;
						
int						area_colors[area_color_count]=area_color_colors;

extern bool obscure_mesh_view_bit_get(unsigned char *visibility_flag,int idx);

/* =======================================================

      Walk View Sprites Drawing
      
======================================================= */

void walk_view_draw_sprite(d3pnt *pnt,float ang_y,unsigned long gl_id)
{
    int			px[4],pz[4],wid,high;
	
    wid=map_enlarge*3;
    high=map_enlarge*4;
	
	px[0]=px[3]=pnt->x-wid;
	px[1]=px[2]=pnt->x+wid;
	pz[0]=pz[1]=pnt->z-wid;
	pz[2]=pz[3]=pnt->z+wid;
	
	rotate_2D_polygon(4,px,pz,pnt->x,pnt->z,ang_y);
    
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,gl_id);
    
	glColor4f(1,1,1,1);
	
	glBegin(GL_QUADS);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[1],pnt->y,pz[1]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],pnt->y,pz[0]);

    glTexCoord2f(0,0);
	glVertex3i(px[3],(pnt->y-high),pz[3]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],pnt->y,pz[3]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[3],(pnt->y-high),pz[3]);
    glTexCoord2f(1,1);
	glVertex3i(px[3],pnt->y,pz[3]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],pnt->y,pz[0]);

    glTexCoord2f(0,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[1],pnt->y,pz[1]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],(pnt->y-high),pz[3]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],pnt->y,pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],pnt->y,pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],pnt->y,pz[3]);
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_circle(d3pnt *pnt,d3col *col,int dist)
{
    int				n,kx,kz;
	
	glLineWidth(4.0f);
	glColor4f(col->r,col->g,col->b,0.75f);
	
		// xz line
		
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=360;n+=10) {
		kx=dist;
		kz=0;
		rotate_2D_point_center(&kx,&kz,(float)n);
		glVertex3i((pnt->x+kx),pnt->y,(pnt->z+kz));
	}

	glEnd();
	
	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

/* =======================================================

      Walk View Mesh Drawing
      
======================================================= */

void walk_view_draw_meshes_texture(editor_3D_view_setup *view_setup,bool opaque)
{
	int						n,k,t;
	unsigned long			old_gl_id;
	bool					clip_ok;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	map_mesh_poly_uv_type	*uv;
	texture_type			*texture;
							
		// no depth buffer for transparent segments
		
	glEnable(GL_TEXTURE_2D);

	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);
	if (opaque) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	old_gl_id=-1;

		// draw meshes
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// skip any meshes that don't have
			// light maps if on light maps
			
		if ((main_wind_uv_layer==uv_layer_light_map) && (mesh->flag.no_light_map)) {
			mesh++;
			continue;
		}
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			
				// no light map?
				
			if ((main_wind_uv_layer==uv_layer_light_map) && (mesh_poly->lmap_txt_idx==-1)) continue;
			
				// get texture.  If in second UV, we use light map
				// texture for display if it exists
				
			if (main_wind_uv_layer==uv_layer_normal) {
				texture=&map.textures[mesh_poly->txt_idx];
				uv=&mesh_poly->main_uv;
			}
			else {
				texture=&map.textures[mesh_poly->lmap_txt_idx];
				uv=&mesh_poly->lmap_uv;
			}
		
				// opaque or transparent flag
				
			if (opaque) {
				if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) continue;
			}
			else {
				if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) continue;
			}
			
				// y clipping
				
			if (view_setup->clip_on) {
			
				clip_ok=TRUE;
				
				for (t=0;t!=mesh_poly->ptsz;t++) {
					if (mesh->vertexes[mesh_poly->v[t]].y>=view_setup->clip_y) {
						clip_ok=FALSE;
						break;
					}
				}
				
				if (clip_ok) continue;
			}
		
				// setup texture
				
			if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
				old_gl_id=texture->frames[0].bitmap.gl_id;
				glBindTexture(GL_TEXTURE_2D,old_gl_id);
			}
		
				// draw polygon
				
			glBegin(GL_POLYGON);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				glTexCoord2f(uv->x[t],uv->y[t]);
				glVertex3i(pt->x,pt->y,pt->z);
			}
			
			glEnd();
		}
	
		mesh++;
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_meshes_line(editor_3D_view_setup *view_setup,bool opaque)
{
	int					n,k,t;
	bool				clip_ok;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
							
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
	glColor4f(setup.col.mesh_line.r,setup.col.mesh_line.g,setup.col.mesh_line.b,1.0f);
	
		// draw portal mesh lines

	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			texture=&map.textures[mesh_poly->txt_idx];
			
				// opaque or transparent flag
		
			if (opaque) {
				if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) continue;
			}
			else {
				if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) continue;
			}
			
				// y clipping
				
			if (view_setup->clip_on) {
			
				clip_ok=TRUE;
				
				for (t=0;t!=mesh_poly->ptsz;t++) {
					if (mesh->vertexes[mesh_poly->v[t]].y>=view_setup->clip_y) {
						clip_ok=FALSE;
						break;
					}
				}
				
				if (clip_ok) continue;
			}
		
		
			glBegin(GL_LINE_LOOP);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				glVertex3i(pt->x,pt->y,pt->z);
			}
			
			glEnd();
		}
	
		mesh++;
	}
}

/* =======================================================

      Walk View Liquid Drawing
      
======================================================= */

void walk_view_draw_liquids(bool opaque)
{
	int					n,nliquid,x,y,z,y2,lx,rx,tz,bz;
	unsigned long		old_gl_id;
	texture_type		*texture;
	map_liquid_type		*liquid;
	map_liquid_uv_type	*uv;
	
	if (!dp_liquid) return;
	
		// no depth buffer for transparent segments
		
	glEnable(GL_TEXTURE_2D);

	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);
	if (opaque) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}
	
	old_gl_id=-1;

		// run through the liquids
		
	nliquid=map.liquid.nliquid;
	
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	for (n=0;n!=nliquid;n++) {
		liquid=&map.liquid.liquids[n];
		
			// no light map?
				
		if ((main_wind_uv_layer==uv_layer_light_map) && (liquid->lmap_txt_idx==-1)) continue;
			
			// get texture.  If in second UV, we use light map
			// texture for display if it exists
			
		if (main_wind_uv_layer==uv_layer_normal) {
			texture=&map.textures[liquid->txt_idx];
			uv=&liquid->main_uv;
		}
		else {
			texture=&map.textures[liquid->lmap_txt_idx];
			uv=&liquid->lmap_uv;
		}
	
		if (opaque) {
			if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) continue;
		}
		else {
			if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) continue;
		}
		
		if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
			old_gl_id=texture->frames[0].bitmap.gl_id;
			glBindTexture(GL_TEXTURE_2D,old_gl_id);
		}
		
			// dimensions
			
		y=liquid->y;
		y2=liquid->y+liquid->depth;
		
		lx=liquid->lft;
		rx=liquid->rgt;
		tz=liquid->top;
		bz=liquid->bot;
		
		glBegin(GL_QUADS);
		
			// bottom
			
		glTexCoord2f(uv->x_offset,uv->y_offset);
		glVertex3i(lx,y2,tz);
		glTexCoord2f((uv->x_offset+uv->x_size),uv->y_offset);
		glVertex3i(rx,y2,tz);
		glTexCoord2f((uv->x_offset+uv->x_size),(uv->y_offset+uv->y_size));
		glVertex3i(rx,y2,bz);
		glTexCoord2f(uv->x_offset,(uv->y_offset+uv->y_size));
		glVertex3i(lx,y2,bz);

			// top
			
		glTexCoord2f(uv->x_offset,uv->y_offset);
		glVertex3i(lx,y,tz);
		glTexCoord2f((uv->x_offset+uv->x_size),uv->y_offset);
		glVertex3i(rx,y,tz);
		glTexCoord2f((uv->x_offset+uv->x_size),(uv->y_offset+uv->y_size));
		glVertex3i(rx,y,bz);
		glTexCoord2f(uv->x_offset,(uv->y_offset+uv->y_size));
		glVertex3i(lx,y,bz);

		glEnd();
		
			// depth lines
	
		glColor4f(setup.col.mesh_line.r,setup.col.mesh_line.g,setup.col.mesh_line.b,1.0f);
		
		glBegin(GL_LINES);
		
		x=liquid->lft;
		z=liquid->top;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->rgt;
		z=liquid->top;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->lft;
		z=liquid->bot;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->rgt;
		z=liquid->bot;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		glEnd();
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Walk View Normal Drawing
      
======================================================= */

void walk_view_draw_meshes_normals(editor_3D_view_setup *view_setup)
{
	int					n,k,t;
	bool				clip_ok;
	d3pnt				*pt,cnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
							
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
	glLineWidth(2.0f);
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];
			
				// y clipping
				
			if (view_setup->clip_on) {
			
				clip_ok=TRUE;
				
				for (t=0;t!=poly->ptsz;t++) {
					if (mesh->vertexes[poly->v[t]].y>=view_setup->clip_y) {
						clip_ok=FALSE;
						break;
					}
				}
				
				if (clip_ok) continue;
			}
			
				// get center
				
			
			cnt.x=cnt.y=cnt.z=0;
			
			for (t=0;t!=poly->ptsz;t++) {
				pt=&mesh->vertexes[poly->v[t]];
				cnt.x+=pt->x;
				cnt.y+=pt->y;
				cnt.z+=pt->z;
			}
			
			cnt.x/=poly->ptsz;
			cnt.y/=poly->ptsz;
			cnt.z/=poly->ptsz;
			
				// draw the tangent
			/*	
			glColor4f(1.0f,0.0f,0.0f,1.0f);
			
			glBegin(GL_LINES);
			glVertex3i(cnt.x,cnt.y,cnt.z);
			glVertex3i((cnt.x+(int)(poly->tangent_space.tangent.x*normal_vector_scale)),(cnt.y+(int)(poly->tangent_space.tangent.y*normal_vector_scale)),(cnt.z+(int)(poly->tangent_space.tangent.z*normal_vector_scale)));
			glEnd();
			
				// draw the binormal
				
			glColor4f(0.0f,0.0f,1.0f,1.0f);
			
			glBegin(GL_LINES);
			glVertex3i(cnt.x,cnt.y,cnt.z);
			glVertex3i((cnt.x+(int)(poly->tangent_space.binormal.x*normal_vector_scale)),(cnt.y+(int)(poly->tangent_space.binormal.y*normal_vector_scale)),(cnt.z+(int)(poly->tangent_space.binormal.z*normal_vector_scale)));
			glEnd();
			*/
				// draw normal
				
			glColor4f(1.0f,0.0f,1.0f,1.0f);
			
			glBegin(GL_LINES);
			glVertex3i(cnt.x,cnt.y,cnt.z);
			glVertex3i((cnt.x+(int)(poly->tangent_space.normal.x*normal_vector_scale)),(cnt.y+(int)(poly->tangent_space.normal.y*normal_vector_scale)),(cnt.z+(int)(poly->tangent_space.normal.z*normal_vector_scale)));
			glEnd();
		}
	
		mesh++;
	}
	
	glLineWidth(1.0f);
}

/* =======================================================

      Walk View Area Drawing
      
======================================================= */

void walk_view_draw_areas(void)
{
	int					n,y,icol;
	d3col				col;
	map_area_type		*area;
	
	if (area_col_type==-1) return;
	
		// get color
		
	icol=area_colors[area_col_type];
	
	col.r=((float)((icol>>16)&0xFF))/255.0f;
	col.g=((float)((icol>>8)&0xFF))/255.0f;
	col.b=((float)(icol&0xFF))/255.0f;
	
		// transparent area draw
		
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
		// areas are 2D
		
	y=view_pnt.y;

		// area colors
				
	glColor4f(col.r,col.g,col.b,0.5f);
		
	area=map.areas;
	
	for (n=0;n!=map.narea;n++) {
	
		if (area->col_type==area_col_type) {
			glBegin(GL_QUADS);
			glVertex3i(area->min.x,y,area->min.z);
			glVertex3i(area->max.x,y,area->min.z);
			glVertex3i(area->max.x,y,area->max.z);
			glVertex3i(area->min.x,y,area->max.z);
			glEnd();
		}
		
		area++;
	}
	
		// area boxes

	glColor4f((col.r*0.7f),(col.g*0.7f),(col.b*0.7f),1.0f);

	area=map.areas;
	
	for (n=0;n!=map.narea;n++) {
		
		if (area->col_type==area_col_type) {
			glBegin(GL_LINE_LOOP);
			glVertex3i(area->min.x,y,area->min.z);
			glVertex3i(area->max.x,y,area->min.z);
			glVertex3i(area->max.x,y,area->max.z);
			glVertex3i(area->min.x,y,area->max.z);
			glEnd();
		}
		
		area++;
	}
	
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

/* =======================================================

      Walk View Nodes, Scenery, etc Drawing
      
======================================================= */

void walk_view_draw_nodes(d3pnt *cpt)
{
	int			n,k;
	float		fx,fy,fz;
	node_type	*node,*lnode;
	matrix_type	mat;

	if (!dp_node) return;
	
		// angles
		
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
		
	glLineWidth(3.0f);
	glColor4f(1.0f,0.7f,0.0f,1.0f);
	
	glBegin(GL_LINES);
		
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
	
		fx=0.0f;
		fy=0.0f;
		fz=-(float)(map_enlarge*8);
	
		matrix_rotate_xyz(&mat,node->ang.x,node->ang.y,node->ang.z);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			
		glVertex3i(node->pnt.x,(node->pnt.y-(map_enlarge*2)),node->pnt.z);
		glVertex3i((node->pnt.x+(int)fx),((node->pnt.y-(map_enlarge*2))+(int)fy),(node->pnt.z+(int)fz));
		
		node++;
	}
	
	glEnd();
	
		// connections
		
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
		
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
			
		for (k=0;k!=max_node_link;k++) {
		
			if (node->link[k]!=-1) {
				glVertex3i(node->pnt.x,(node->pnt.y-(map_enlarge*2)),node->pnt.z);
				lnode=&map.nodes[node->link[k]];
				glVertex3i(lnode->pnt.x,(lnode->pnt.y-(map_enlarge*2)),lnode->pnt.z);
			}
		}
		
		node++;
	}
	
	glEnd();

	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		// nodes
	
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		if (node->name[0]==0x0) {
			walk_view_draw_sprite(&node->pnt,0.0f,node_bitmap.gl_id);
		}
		else {
			walk_view_draw_sprite(&node->pnt,0.0f,node_defined_bitmap.gl_id);
		}
		node++;
	}
}

void walk_view_draw_spots_scenery(d3pnt *cpt)
{
	int					n;
	spot_type			*spot;
	map_scenery_type	*scenery;
	
    if (!dp_object) return;
    
	spot=map.spots;
	
	for (n=0;n!=map.nspot;n++) {
	
		if (!walk_view_model_draw(&spot->pnt,&spot->ang,spot->display_model,NULL,0)) {
			walk_view_draw_sprite(&spot->pnt,spot->ang.y,spot_bitmap.gl_id);
		}
		
		spot++;
	}		
    
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
	
		if (!walk_view_model_draw(&scenery->pnt,&scenery->ang,scenery->model_name,scenery->texture_frame,max_map_scenery_model_texture_frame)) {
			walk_view_draw_sprite(&scenery->pnt,scenery->ang.y,scenery_bitmap.gl_id);
		}
		
		scenery++;
	}		
}

void walk_view_draw_lights_sounds_particles(d3pnt *cpt,bool draw_light_circle)
{
	int				n;
	
	if (!dp_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		if (draw_light_circle) walk_view_draw_circle(&map.lights[n].pnt,&map.lights[n].col,map.lights[n].intensity);
		walk_view_draw_sprite(&map.lights[n].pnt,0.0f,light_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nsound;n++) {
		walk_view_draw_sprite(&map.sounds[n].pnt,0.0f,sound_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nparticle;n++) {
		walk_view_draw_sprite(&map.particles[n].pnt,0.0f,particle_bitmap.gl_id);
	}
}

/* =======================================================

      Walk View Drawing Setup
      
======================================================= */

void walk_view_gl_setup(editor_3D_view_setup *view_setup)
{
	main_wind_set_viewport(&view_setup->box,TRUE,TRUE);
	main_wind_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

/* =======================================================

      Draw Current Position
      
======================================================= */

void walk_view_draw_position(editor_3D_view_setup *view_setup)
{
    int			x,z,sz;
   
 	main_wind_set_2D_projection(view_setup);

	glDisable(GL_DEPTH_TEST);

	x=view_setup->box.lx+((view_setup->box.rx-view_setup->box.lx)>>1);
	z=view_setup->box.ty+((view_setup->box.by-view_setup->box.ty)>>1);
	
	sz=10;
	
	glColor4f(0.0f,0.0f,0.0f,0.75f);

	glLineWidth(3.0f);
	
	glBegin(GL_LINES);
	glVertex2i(x,(z-sz));
	glVertex2i(x,(z+sz));
	glVertex2i((x-sz),z);
	glVertex2i((x+sz),z);
	glEnd();
	
	glLineWidth(1.0f);
}

/* =======================================================

      Draw Walk View
      
======================================================= */

void walk_view_draw(editor_3D_view_setup *view_setup,bool draw_position)
{
       // 3D view
        
	main_wind_set_viewport(&view_setup->box,TRUE,TRUE);
	main_wind_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

        // draw opaque parts of portals in sight path
        
	if (!view_setup->mesh_only) walk_view_draw_meshes_texture(view_setup,TRUE);
	walk_view_draw_nodes(&view_setup->cpt);
	walk_view_draw_spots_scenery(&view_setup->cpt);
	walk_view_draw_lights_sounds_particles(&view_setup->cpt,view_setup->draw_light_circle);
	walk_view_draw_liquids(TRUE);
	
		// draw opaque mesh lines
		// push view forward to better z-buffer lines
		
	main_wind_set_3D_projection(view_setup,(map.settings.editor.view_near_dist+10),(map.settings.editor.view_far_dist-10),walk_view_near_offset);
	walk_view_draw_meshes_line(view_setup,TRUE);

        // draw transparent parts of portals in sight path
        
	main_wind_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);

	if (!view_setup->mesh_only) walk_view_draw_meshes_texture(view_setup,FALSE);
	walk_view_draw_liquids(FALSE);
 	
        // draw transparent mesh lines
		// push view forward to better z-buffer lines
        
	main_wind_set_3D_projection(view_setup,(map.settings.editor.view_near_dist+10),(map.settings.editor.view_far_dist-10),walk_view_near_offset);
	walk_view_draw_meshes_line(view_setup,TRUE);
	
        // draw normals mesh lines
		// push view forward to better z-buffer lines
      
	if (dp_normals) {
		main_wind_set_3D_projection(view_setup,(map.settings.editor.view_near_dist+20),(map.settings.editor.view_far_dist-20),walk_view_near_offset);
		walk_view_draw_meshes_normals(view_setup);
	}
	
		// draw areas
		
	if (view_setup->draw_area) walk_view_draw_areas();
		
		// draw selection
		
	walk_view_draw_select(&view_setup->cpt,view_setup->draw_area);

		// position
		
	main_wind_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);

	if (draw_position) walk_view_draw_position(view_setup);
}

