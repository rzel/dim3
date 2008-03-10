/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Preparing Map For Engine Rendering

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Get Enclosing Square for Polygon
      
======================================================= */

void map_prepare_set_mesh_poly_box(map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int				n,k,ptsz,y;
	bool			flat,common_xz;
	d3pnt			min,max,mid;
	d3pnt			*pt,*chk_pt;

		// find enclosing square
		// and middle and if polygon is flat
		
	pt=&mesh->vertexes[mesh_poly->v[0]];

	min.x=max.x=mid.x=pt->x;
	min.y=max.y=mid.y=y=pt->y;
	min.z=max.z=mid.z=pt->z;

	flat=TRUE;
    
    ptsz=mesh_poly->ptsz;
    
	for (n=1;n<ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];

			// get min and max

		if (pt->x<min.x) min.x=pt->x;
		if (pt->x>max.x) max.x=pt->x;
		if (pt->y<min.y) min.y=pt->y;
		if (pt->y>max.y) max.y=pt->y;
		if (pt->z<min.z) min.z=pt->z;
		if (pt->z>max.z) max.z=pt->z;

			// add for middle

		mid.x+=pt->x;
		mid.y+=pt->y;
		mid.z+=pt->z;

			// check for flat y

		if (pt->y!=y) flat=FALSE;
	}
    
	memmove(&mesh_poly->box.min,&min,sizeof(d3pnt));
	memmove(&mesh_poly->box.max,&max,sizeof(d3pnt));
	
	mesh_poly->box.mid.x=mid.x/ptsz;
	mesh_poly->box.mid.y=mid.y/ptsz;
	mesh_poly->box.mid.z=mid.z/ptsz;
	
	mesh_poly->box.flat=flat;

		// check for common xz points
		// this helps determine which way
		// to tessel lighting

	common_xz=FALSE;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];

		for (k=0;k!=ptsz;k++) {
			if (k==n) continue;
			
			chk_pt=&mesh->vertexes[mesh_poly->v[k]];
			if ((pt->x==chk_pt->x) && (pt->z==chk_pt->z)) {
				common_xz=TRUE;
				break;
			}
		}

		if (common_xz) break;
	}

	mesh_poly->box.common_xz=common_xz;
}

/* =======================================================

      Create Slopes Data
      
======================================================= */

void map_prepare_set_mesh_poly_slope(map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int				n,x[8],y[8],z[8];
	d3pnt			*pt;
	
	for (n=0;n!=mesh_poly->ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		x[n]=pt->x;
		y[n]=pt->y;
		z[n]=pt->z;
	}

	mesh_poly->slope.y=polygon_get_slope_y(mesh_poly->ptsz,x,y,z,&mesh_poly->slope.ang_y);
	angle_get_movement_float(mesh_poly->slope.ang_y,(gravity_slope_factor*mesh_poly->slope.y),&mesh_poly->slope.move_x,&mesh_poly->slope.move_z);
}

/* =======================================================

      Prepare Map
      
======================================================= */

void map_prepare(map_type *map)
{
	int					i,n,k,t;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	map_liquid_type		*liq;
	node_type			*node;
	map_scenery_type	*scenery;
	map_light_type		*light;
	map_sound_type		*sound;
	map_particle_type	*particle;
	spot_type			*spot;
	
		// optimizations
		
	map->optimizations.lod_light_distance*=map_enlarge;
	map->optimizations.lod_bump_distance*=map_enlarge;
	map->optimizations.lod_specular_distance*=map_enlarge;
	map->optimizations.lod_glow_distance*=map_enlarge;
	map->optimizations.lod_model_distance*=map_enlarge;
	map->optimizations.lod_shadow_distance*=map_enlarge;
	map->optimizations.lod_effect_distance*=map_enlarge;
	
		// portals
	
	portal=map->portals;
	
	for (i=0;i!=map->nportal;i++) {

			// portal middle

		portal->mx=(portal->x+portal->ex)>>1;
		portal->mz=(portal->z+portal->ez)>>1;

			// search for y size when
			// preparing meshes
		
		portal->ty=99999;
		portal->by=0;
		
			// prepare meshes

		mesh=portal->mesh.meshes;
		
		for (n=0;n!=portal->mesh.nmesh;n++) {
				
				// translate vertexes from portal to global
					
			pt=mesh->vertexes;
			
			for (k=0;k!=mesh->nvertex;k++) {
				pt->x+=portal->x;
				pt->z+=portal->z;
				pt++;
			}
			
				// run through the mesh polygons

			mesh_poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
			
					// find lowest and highest Y for portal

				for (t=0;t!=mesh_poly->ptsz;t++) {
					pt=&mesh->vertexes[mesh_poly->v[t]];

					if (pt->y<portal->ty) portal->ty=pt->y;
					if (pt->y>portal->by) portal->by=pt->y;
				}
				
					// setup box and slope

				map_prepare_set_mesh_poly_box(mesh,mesh_poly);
				map_prepare_set_mesh_poly_slope(mesh,mesh_poly);

					// setup simple tessel and shifting flag

				mesh_poly->draw.txt_frame_offset=0;
				mesh_poly->draw.simple_tessel=(mesh_poly->ptsz==3);
				mesh_poly->draw.shift_on=((mesh_poly->x_shift!=0.0f) || (mesh_poly->y_shift!=0.0f));
			
				mesh_poly++;
			}
		
			mesh++;
		}
		
			// liquids

		liq=portal->liquid.liquids;
		
		for (n=0;n!=portal->liquid.nliquid;n++) {
			liq->lft+=portal->x;
			liq->rgt+=portal->x;
			liq->top+=portal->z;
			liq->bot+=portal->z;
			liq++;
		}
		
			// fix portal heights

		if ((portal->ty==99999) && (portal->by!=0)) portal->ty=portal->by;
		if ((portal->by==0) && (portal->ty!=99999)) portal->by=portal->ty;
		if ((portal->ty==99999) || (portal->by==0)) portal->ty=portal->by=0;

		portal++;
	}
	
		// translate other objects from portal
		// to global coordinates
	
	scenery=map->sceneries;
	
	for (n=0;n!=map->nscenery;n++) {
		portal=&map->portals[scenery->pos.rn];
		scenery->pos.x+=portal->x;
		scenery->pos.z=scenery->pos.z+portal->z;
		scenery++;
	}
		
	light=map->lights;
	
	for (n=0;n!=map->nlight;n++) {
		portal=&map->portals[light->pos.rn];
		light->pos.x=light->pos.x+portal->x;
		light->pos.z=light->pos.z+portal->z;
		light++;
	}
	
		// map sounds
	
	sound=map->sounds;
	
	for (n=0;n!=map->nsound;n++) {
		portal=&map->portals[sound->pos.rn];
		sound->pos.x=sound->pos.x+portal->x;
		sound->pos.z=sound->pos.z+portal->z;
		sound++;
	}
	
		// map particles
	
	particle=map->particles;
	
	for (n=0;n!=map->nparticle;n++) {
		portal=&map->portals[particle->pos.rn];
		particle->pos.x=particle->pos.x+portal->x;
		particle->pos.z=particle->pos.z+portal->z;
		particle++;
	}
	
		// nodes

	node=map->nodes;
	
	for (n=0;n!=map->nnode;n++) {
		portal=&map->portals[node->pos.rn];
        node->idx=i;
		node->pos.x=node->pos.x+portal->x;
		node->pos.z=node->pos.z+portal->z;
		node++;
	}
    
		// object starts
		
	spot=map->spots;

	for (n=0;n!=map->nspot;n++) {
		portal=&map->portals[spot->pos.rn];
		spot->pos.x=spot->pos.x+portal->x;
		spot->pos.z=spot->pos.z+portal->z;
		spot++;
	}
}

