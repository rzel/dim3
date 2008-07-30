/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Collisions

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

#include "scripts.h"
#include "objects.h"
#include "projectiles.h"
#include "models.h"
#include "physics.h"

extern map_type			map;
extern server_type		server;

/* =======================================================

      Determine if Contact Poly is Wall-Like
      
======================================================= */

bool collide_contact_is_wall_hit(poly_pointer_type *hit_poly)
{
	map_mesh_poly_type		*mesh_poly;

	if (hit_poly->mesh_idx==-1) return(FALSE);

	mesh_poly=&map.mesh.meshes[hit_poly->mesh_idx].polys[hit_poly->poly_idx];
	return(mesh_poly->box.wall_like);
}

/* =======================================================

      Check for Object-Map Collisions
      
======================================================= */

// supergumba -- NEW COLLISION -- to collide utility

int collide_point_distance(d3pnt *pt_1,d3pnt *pt_2)
{
	double		dx,dz,dy;
	
	dx=(double)(pt_1->x-pt_2->x);
	dy=(double)(pt_1->y-pt_2->y);
	dz=(double)(pt_1->z-pt_2->z);
	
	return((int)sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

void collide_object_ray_trace_points(obj_type *obj,int x_add,int z_add,int *px,int *py,int *pz)
{
	int			x,z,x_sz,z_sz,
				move_side,face_side,side;
	float		move_ang,face_ang;

		// determine the polygon face to
		// check hit points for

	move_ang=angle_find(0,0,x_add,z_add);
	face_ang=angle_add(obj->ang.y,obj->draw.rot.y);

	move_side=((int)((move_ang+45.0f)/90.0f))&0x3;
	face_side=((int)((face_ang+45.0f)/90.0f))&0x3;
	side=(move_side-face_side)&0x3;

		// polygon size

	x=obj->pos.x;
	x_sz=obj->size.x;

	z=obj->pos.z;
	z_sz=obj->size.z;

		// get points for that poly face

	switch (side) {

			// front side

		case 0:
			pz[0]=pz[1]=pz[2]=pz[3]=pz[4]=pz[5]=pz[6]=pz[7]=pz[8]=pz[9]=pz[10]=pz[11]=pz[12]=pz[13]=pz[14]=pz[15]=z-(z_sz>>1);
			px[0]=px[3]=px[6]=px[9]=px[12]=x-(x_sz>>1);
			px[1]=px[4]=px[7]=px[10]=px[13]=x;
			px[2]=px[5]=px[8]=px[11]=px[14]=x+(x_sz>>1);
			break;

			// right side

		case 1:
			px[0]=px[1]=px[2]=px[3]=px[4]=px[5]=px[6]=px[7]=px[8]=px[9]=px[10]=px[11]=px[12]=px[13]=px[14]=px[15]=x+(x_sz>>1);
			pz[0]=pz[3]=pz[6]=pz[9]=pz[12]=z-(z_sz>>1);
			pz[1]=pz[4]=pz[7]=pz[10]=pz[13]=z;
			pz[2]=pz[5]=pz[8]=pz[11]=pz[14]=z+(z_sz>>1);
			break;

			// back side

		case 2:
			pz[0]=pz[1]=pz[2]=pz[3]=pz[4]=pz[5]=pz[6]=pz[7]=pz[8]=pz[9]=pz[10]=pz[11]=pz[12]=pz[13]=pz[14]=pz[15]=z+(z_sz>>1);
			px[0]=px[3]=px[6]=px[9]=px[12]=x-(x_sz>>1);
			px[1]=px[4]=px[7]=px[10]=px[13]=x;
			px[2]=px[5]=px[8]=px[11]=px[14]=x+(x_sz>>1);
			break;

			// left side

		case 3:
			px[0]=px[1]=px[2]=px[3]=px[4]=px[5]=px[6]=px[7]=px[8]=px[9]=px[10]=px[11]=px[12]=px[13]=px[14]=px[15]=x-(x_sz>>1);
			pz[0]=pz[3]=pz[6]=pz[9]=pz[12]=z-(z_sz>>1);
			pz[1]=pz[4]=pz[7]=pz[10]=pz[13]=z;
			pz[2]=pz[5]=pz[8]=pz[11]=pz[14]=z+(z_sz>>1);
			break;

	}

		// rotate face

	rotate_2D_polygon(15,px,pz,x,z,face_ang);

		// setup Ys

	py[0]=py[1]=py[2]=(obj->pos.y-obj->size.y)+(map_enlarge>>1);
	py[3]=py[4]=py[5]=(obj->pos.y-obj->size.y)+(obj->size.y>>2);
	py[6]=py[7]=py[8]=obj->pos.y-(obj->size.y>>1);
	py[9]=py[10]=py[11]=obj->pos.y-(obj->size.y>>2);
	py[12]=py[13]=py[14]=obj->pos.y-(map_enlarge>>1);
}

bool collide_object_to_map(obj_type *obj,int *xadd,int *yadd,int *zadd)
{
	int						n,x,z,idx,xadd2,yadd2,zadd2,radius,
							px[15],py[15],pz[15],
							d,dist,mv_dist;
	float					mv_ang;
	bool					bump;
	d3pnt					spt,ept,hpt[15];
	map_mesh_poly_type		*poly;
	ray_trace_contact_type	contact[15];

		// get collision points

	collide_object_ray_trace_points(obj,*xadd,*zadd,px,py,pz);

		// need to increase ray check distance
		// to avoid objects getting close to polygons
		// and then turning their corner into them

	radius=obj->size.radius;

	x=*xadd;
	z=*zadd;

	mv_dist=(int)(sqrt((double)(x*x)+(double)(z*z)));
	mv_ang=angle_find(0,0,x,z);

	if (radius>mv_dist) {
		angle_get_movement(mv_ang,radius,&xadd2,&zadd2);
		
		yadd2=*yadd;
		if (mv_dist!=0) yadd2=((*yadd)*radius)/mv_dist;
	}
	else {
		xadd2=x;
		zadd2=z;
		yadd2=*yadd;
	}

		// run rays on all points
		// find the one that moves the leasts
		// as the most suitable hit point

	idx=-1;
	dist=0;

	for (n=0;n!=15;n++) {
			
			// setup ray trace

		spt.x=px[n];
		spt.y=py[n];
		spt.z=pz[n];

		ept.x=spt.x+xadd2;
		ept.y=spt.y+yadd2;
		ept.z=spt.z+zadd2;

		contact[n].obj_on=TRUE;
		contact[n].obj_ignore_uid=obj->uid;

		contact[n].proj_on=FALSE;
		contact[n].proj_ignore_uid=-1;

		contact[n].hit_mode=poly_ray_trace_hit_mode_wall_only;
		contact[n].origin=poly_ray_trace_origin_object;
	
			// run trace

		if (ray_trace_map_by_point(&spt,&ept,&hpt[n],&contact[n])) {
			d=collide_point_distance(&spt,&hpt[n]);
			if ((d<=dist) || (idx==-1)) {
				dist=d;
				idx=n;
			}
		}
	}

		// no hits?

	if (idx==-1) return(FALSE);
	
		// determine if hit poly is bump-capable
		// we ignore object radius calculations
		// for bump-capable segments as it pushes
		// the object around on stairs
		
	if (contact[idx].poly.mesh_idx!=-1) {
	
		if (obj->bump.on) {
			poly=&map.mesh.meshes[contact[idx].poly.mesh_idx].polys[contact[idx].poly.poly_idx];
			bump=((obj->pos.y-poly->box.min.y)<=obj->bump.high);
		}
		else {
			bump=FALSE;
		}
		
	}
	
		// make sure distance stays within
		// object radius

	if (!bump) {
		x=hpt[idx].x-px[idx];
		z=hpt[idx].z-pz[idx];

		d=(int)(sqrt((double)(x*x)+(double)(z*z)));

		angle_get_movement(mv_ang,(radius-d),&xadd2,&zadd2);
		*xadd=(*xadd)-xadd2;
		*zadd=(*zadd)-zadd2;
	}
	else {
		*xadd=hpt[idx].x-px[idx];
		*zadd=hpt[idx].z-pz[idx];
	}
	
		// setup the hits

	if (contact[idx].poly.mesh_idx!=-1) {
		memmove(&obj->contact.hit_poly,&contact[idx].poly,sizeof(poly_pointer_type));
	}

	if (contact[idx].obj_uid!=-1) obj->contact.obj_uid=contact[idx].obj_uid;
	if (contact[idx].proj_uid!=-1) obj->contact.proj_uid=contact[idx].proj_uid;

	return(TRUE);
}

/* =======================================================

      Check for Project-Map Collisions
      
======================================================= */

bool collide_projectile_to_map(proj_type *proj,int xadd,int yadd,int zadd)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;
	proj_setup_type			*proj_setup;

		// setup ray trace

	spt.x=proj->pnt.x;
	spt.y=proj->pnt.y;
	spt.z=proj->pnt.z;

	ept.x=spt.x+xadd;
	ept.y=spt.y+yadd;
	ept.z=spt.z+zadd;

	contact.obj_on=TRUE;
	if (proj->parent_grace>0) {
		contact.obj_ignore_uid=proj->obj_uid;
	}
	else {
		contact.obj_ignore_uid=-1;
	}

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	contact.proj_on=proj_setup->collision;
	contact.proj_ignore_uid=proj->uid;

	contact.hit_mode=poly_ray_trace_hit_mode_all;
	contact.origin=poly_ray_trace_origin_projectile;

		// run trace

	if (!ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		proj->pnt.x=ept.x;
		proj->pnt.y=ept.y;
		proj->pnt.z=ept.z;
		return(FALSE);
	}

		// move to hit point

	proj->pnt.x=hpt.x;
	proj->pnt.y=hpt.y;
	proj->pnt.z=hpt.z;

		// setup hits

	proj->contact.obj_uid=contact.obj_uid;
	proj->contact.proj_uid=contact.proj_uid;
	memmove(&proj->contact.hit_poly,&contact.poly,sizeof(poly_pointer_type));

	return(TRUE);
}

