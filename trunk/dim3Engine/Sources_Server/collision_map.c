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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"
#include "objects.h"

extern map_type			map;
extern view_type		view;
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

      Intersection Utilities
      
======================================================= */

inline bool line_line_intersect(d3pnt *p0,d3pnt *p1,d3pnt *p2,d3pnt *p3,d3pnt *hit_pnt)
{
	float			fx0,fy0,fx1,fy1,fx2,fy2,fx3,fy3,
					denom,
					ax,bx,dx,ay,by,dy,r,s;

	fx0=(float)p0->x;
	fy0=(float)p0->z;
	fx1=(float)p1->x;
	fy1=(float)p1->z;
	fx2=(float)p2->x;
	fy2=(float)p2->z;
	fx3=(float)p3->x;
	fy3=(float)p3->z;
	
	ax=fx0-fx2;
	bx=fx1-fx0;
	dx=fx3-fx2;
	
	ay=fy0-fy2;
	by=fy1-fy0;
	dy=fy3-fy2;
	
	denom=(bx*dy)-(by*dx);
	if (denom==0.0f) return(FALSE);
	
	r=((ay*dx)-(ax*dy))/denom;
	if ((r<0.0f) || (r>1.0f)) return(FALSE);
	
	s=((ay*bx)-(ax*by))/denom;
	if ((s<0.0f) || (s>1.0f)) return(FALSE);
	
	if ((r==0.0f) && (s==0.0f)) return(FALSE);
	
	hit_pnt->x=(int)(fx0+(r*bx));
	hit_pnt->z=(int)(fy0+(r*by));
	
	return(TRUE);
}

int circle_line_intersect(d3pnt *p1,d3pnt *p2,d3pnt *circle_pnt,int radius,d3pnt *hit_pnt)
{
	int				n,xadd,zadd;
	double			rad,d_radius,dist,cur_dist,dx,dz;
	d3pnt			cp2,temp_hit_pnt;
	
		// ray cast like spokes from the circle
		// normal math says check the perpendicular,
		// but that allows parts of the circle to
		// wade into corners

	cur_dist=-1;

	d_radius=(double)radius;

	for (n=0;n!=24;n++) {
		rad=(double)(((float)(n*15))*ANG_to_RAD);

		xadd=(int)(d_radius*sin(rad));
		zadd=-(int)(d_radius*cos(rad));

		cp2.x=circle_pnt->x+xadd;
		cp2.z=circle_pnt->z+zadd;
		
		if (line_line_intersect(p1,p2,circle_pnt,&cp2,&temp_hit_pnt)) {
			dx=(hit_pnt->x-circle_pnt->x);
			dz=(hit_pnt->z-circle_pnt->z);
			dist=(dx*dx)+(dz*dz);

			if ((dist<cur_dist) || (cur_dist==-1)) {
				cur_dist=dist;
				hit_pnt->x=temp_hit_pnt.x;
				hit_pnt->z=temp_hit_pnt.z;
			}
		}
	}

	if (cur_dist==-1) return(-1);
	
	return((int)sqrt(cur_dist));
}

int circle_box_intersect(d3pnt *min,d3pnt *max,d3pnt *circle_pnt,int radius,d3pnt *hit_pnt)
{
	int				n,dist[4],cur_dist;
	d3pnt			p1,p2,hit_pnts[4];
	
		// hit against the four sides
		
	p1.x=min->x;
	p2.x=max->x;
	p1.z=p2.z=min->z;
	dist[0]=circle_line_intersect(&p1,&p2,circle_pnt,radius,&hit_pnts[0]);

	p1.x=min->x;
	p2.x=max->x;
	p1.z=p2.z=max->z;
	dist[1]=circle_line_intersect(&p1,&p2,circle_pnt,radius,&hit_pnts[1]);

	p1.z=min->z;
	p2.z=max->z;
	p1.x=p2.x=min->x;
	dist[2]=circle_line_intersect(&p1,&p2,circle_pnt,radius,&hit_pnts[2]);
	
	p1.z=min->z;
	p2.z=max->z;
	p1.x=p2.x=max->x;
	dist[3]=circle_line_intersect(&p1,&p2,circle_pnt,radius,&hit_pnts[3]);

		// find the best hit
		
	cur_dist=-1;
	
	for (n=0;n!=4;n++) {
		if (dist[n]!=-1) {
			if ((cur_dist==-1) || (dist[n]<cur_dist)) {
				cur_dist=dist[n];
				memmove(hit_pnt,&hit_pnts[n],sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist);
}

void circle_get_point_on_radius_through_hit_point(d3pnt *circle_pnt,int radius,d3pnt *hit_pnt,d3pnt *radius_pt)
{
	double			d,dx,dz;
	float			fr,fx,fz;

		// get the normalized line vector
		
	dx=(double)(hit_pnt->x-circle_pnt->x);
	dz=(double)(hit_pnt->z-circle_pnt->z);

	d=sqrt((dx*dx)+(dz*dz));
	if (d!=0.0) {
		d=1.0/d;
		fx=(float)(dx*d);
		fz=(float)(dz*d);
	}

		// get point on radius
		// that line hits

	fr=(float)radius;

	radius_pt->x=circle_pnt->x+(int)(fr*fx);
	radius_pt->z=circle_pnt->z+(int)(fr*fz);
}

/* =======================================================

      Circle Intersection Routines
      
======================================================= */

bool collide_circle_check_object_cylinder(d3pnt *circle_pnt,int radius,d3pnt *min,d3pnt *max,bool skip_pickup,int *p_cur_dist,obj_type *obj,d3pnt *cur_hit_pnt)
{
	int			dist,chk_radius;
	double		dx,dz;
	
		// y check
		
	if (max->y<=(obj->pnt.y-obj->size.y)) return(FALSE);
	if (min->y>=obj->pnt.y) return(FALSE);

		// radius check

	chk_radius=obj->size.x;
	if (obj->size.z>chk_radius) chk_radius=obj->size.z;

	chk_radius=chk_radius>>1;

		// if distance between center points
		// is greater than radius+chk_radius,
		// then no contact

	dx=(double)(circle_pnt->x-obj->pnt.x);
	dz=(double)(circle_pnt->z-obj->pnt.z);

	dist=(int)sqrt((dx*dx)+(dz*dz));

	if (dist>(chk_radius+radius)) return(FALSE);

		// next nearest hit?

	if ((dist>(*p_cur_dist)) && ((*p_cur_dist)!=-1)) return(FALSE);

		// return hit point, at edge of
		// object circle

	*p_cur_dist=dist;

	circle_get_point_on_radius_through_hit_point(&obj->pnt,chk_radius,circle_pnt,cur_hit_pnt);
	
	return(TRUE);
}

bool collide_circle_check_object_box(d3pnt *circle_pnt,int radius,d3pnt *min,d3pnt *max,bool skip_pickup,int *p_cur_dist,obj_type *obj,d3pnt *cur_hit_pnt)
{
	int						x_sz,z_sz,dist;
	float					rang;
	d3pnt					obj_min,obj_max,hit_pnt;

		// y check
		
	if (max->y<=(obj->pnt.y-obj->size.y)) return(FALSE);
	if (min->y>=obj->pnt.y) return(FALSE);
	
		// run the XZ check

	rang=angle_add(obj->ang.y,obj->draw.rot.y);
	x_sz=obj->size.x>>1;
	z_sz=obj->size.z>>1;
	
	rotate_2D_point_center(&x_sz,&z_sz,rang);
	
	obj_min.x=obj->pnt.x-x_sz;
	obj_max.x=obj->pnt.x+x_sz;
		
	obj_min.z=obj->pnt.z-z_sz;
	obj_max.z=obj->pnt.z+z_sz;
		
		// check for hit
			
	dist=circle_box_intersect(&obj_min,&obj_max,circle_pnt,radius,&hit_pnt);
	if (dist==-1) return(FALSE);

		// next nearest hit?

	if ((dist>(*p_cur_dist)) && ((*p_cur_dist)!=-1)) return(FALSE);

		// return hit point, at edge of
		// object circle

	*p_cur_dist=dist;
		
	cur_hit_pnt->x=hit_pnt.x;
	cur_hit_pnt->y=hit_pnt.y;
	cur_hit_pnt->z=hit_pnt.z;

	return(TRUE);
}

bool collide_circle_check_object(d3pnt *circle_pnt,int radius,d3pnt *min,d3pnt *max,bool skip_pickup,int *p_cur_dist,obj_type *obj,d3pnt *cur_hit_pnt)
{
		// object a hit candidate?

	if (obj->hidden) return(FALSE);
	if ((skip_pickup) && (obj->pickup.on)) return(FALSE);
	if (!obj->contact.object_on) return(FALSE);

		// correct check type
	
	switch (obj->contact.collision_mode) {
			
		case collision_mode_cylinder:
			return(collide_circle_check_object_cylinder(circle_pnt,radius,min,max,skip_pickup,p_cur_dist,obj,cur_hit_pnt));
					
		case collision_mode_box:
			return(collide_circle_check_object_box(circle_pnt,radius,min,max,skip_pickup,p_cur_dist,obj,cur_hit_pnt));
			
	}

	return(FALSE);
}

bool collide_circle_check_projectile(d3pnt *circle_pnt,int radius,d3pnt *min,d3pnt *max,int *p_cur_dist,proj_type *proj,d3pnt *cur_hit_pnt)
{
	int			dist,chk_radius;
	double		dx,dz;
	
		// y check
		
	if (max->y<=(proj->pnt.y-proj->size.y)) return(FALSE);
	if (min->y>=proj->pnt.y) return(FALSE);

		// radius check

	chk_radius=proj->size.x;
	if (proj->size.z>chk_radius) chk_radius=proj->size.z;

	chk_radius=chk_radius>>1;

		// if distance between center points
		// is greater than radius+chk_radius,
		// then no contact

	dx=(double)(circle_pnt->x-proj->pnt.x);
	dz=(double)(circle_pnt->z-proj->pnt.z);

	dist=(int)sqrt((dx*dx)+(dz*dz));

	if (dist>(chk_radius+radius)) return(FALSE);

		// next nearest hit?

	if ((dist>(*p_cur_dist)) && ((*p_cur_dist)!=-1)) return(FALSE);

		// return hit point, at edge of
		// object circle

	*p_cur_dist=dist;

	circle_get_point_on_radius_through_hit_point(&proj->pnt,chk_radius,circle_pnt,cur_hit_pnt);
	
	return(TRUE);
}

bool collide_circle_check_mesh(d3pnt *circle_pnt,int radius,d3pnt *min,d3pnt *max,bool bump_only,int bump_high,int *p_cur_dist,int mesh_idx,int *idx,d3pnt *cur_hit_pnt)
{
	int					n,y,dist,poly_count,cur_dist;
	short				*poly_idx;
	double				dx,dz,hit_dist,tot_dist;
	bool				hit;
	d3pnt				p1,p2,hit_pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// check mesh bounds

	if ((mesh->box.min.x>=max->x) || (mesh->box.max.x<=min->x)) return(FALSE);
	if ((mesh->box.min.y>=max->y) || (mesh->box.max.y<=min->y)) return(FALSE);
	if ((mesh->box.min.z>=max->z) || (mesh->box.max.z<=min->z)) return(FALSE);
	
		// check pass through
		
	if (mesh->flag.pass_through) return(FALSE);
	
		// simplified box collisions
		// if this is on, we are already inside because
		// of the bounds check above

	if (mesh->flag.simple_collision) {
	
			// simplified doesn't bump
			
		if (bump_only) return(FALSE);
		
			// hit against box
			
		dist=circle_box_intersect(&mesh->box.min,&mesh->box.max,circle_pnt,radius,&hit_pnt);
		if (dist==-1) return(FALSE);
		
		*idx=0;
		*p_cur_dist=dist;
		memmove(cur_hit_pnt,&hit_pnt,sizeof(d3pnt));
		
		return(TRUE);
	}

		// complex check
		// check wall polys
					
	poly_count=mesh->poly_list.wall_count;
	poly_idx=mesh->poly_list.wall_idxs;
	
	hit=FALSE;
	cur_dist=*p_cur_dist;

	for (n=0;n!=poly_count;n++) {

		poly=&mesh->polys[poly_idx[n]];

			// is this a bump candidate?

		if (bump_only) {
			if (!poly->draw.bump_ok) continue;
		}
			
			// check poly bounds

		if ((poly->box.min.x>=max->x) || (poly->box.max.x<=min->x)) continue;
		if ((poly->box.min.y>=max->y) || (poly->box.max.y<=min->y)) continue;
		if ((poly->box.min.z>=max->z) || (poly->box.max.z<=min->z)) continue;

			// collide with line

		p1.x=poly->line.lx;
		p1.z=poly->line.lz;

		p2.x=poly->line.rx;
		p2.z=poly->line.rz;

		dist=circle_line_intersect(&p1,&p2,circle_pnt,radius,&hit_pnt);
		if (dist==-1) continue;
		if (dist>radius) continue;
				
			// deal with the slants
			// against y collisions

		if (poly->line.l_ty!=poly->line.r_ty) {
			dx=(double)(hit_pnt.x-poly->line.lx);
			dz=(double)(hit_pnt.z-poly->line.lz);
			hit_dist=sqrt((dx*dx)+(dz*dz));
			
			dx=(double)(poly->line.rx-poly->line.lx);
			dz=(double)(poly->line.rz-poly->line.lz);
			tot_dist=sqrt((dx*dx)+(dz*dz));
			
			if ((tot_dist!=0.0) && (hit_dist!=0.0)) {
				y=poly->line.l_ty+(int)((double)(poly->line.r_ty-poly->line.l_ty)*(hit_dist/tot_dist));
				if (y>=circle_pnt->y) continue;
			}
		}

			// additional bump checks

		if (bump_only) {
			if ((max->y-poly->box.min.y)>bump_high) continue;
		}

			// is this the next nearest hit?

		if ((dist<cur_dist) || (cur_dist==-1)) {
			*idx=poly_idx[n];

			cur_dist=dist;
			cur_hit_pnt->x=hit_pnt.x;
			cur_hit_pnt->z=hit_pnt.z;
			
			hit=TRUE;
		}
	}
	
		// return new cur dist
		
	*p_cur_dist=cur_dist;
	
	return(hit);
}

/* =======================================================

      Generic Collision Routines
      
======================================================= */

bool collide_box_to_map(d3pnt *pt,d3pnt *box_sz,d3pnt *motion,bool check_objs,int skip_obj_idx,bool check_projs,int skip_proj_idx,bool is_proj,obj_contact *contact)
{
	int						n,radius,cur_dist,idx;
	d3pnt					circle_pnt,cur_hit_pnt,radius_pnt,
							min,max;
	obj_type				*chk_obj;
	proj_type				*chk_proj;

		// get the circle radius

	radius=box_sz->x;
	if (box_sz->z>radius) radius=box_sz->z;

	radius=radius>>1;

		// get the moved circle

	circle_pnt.x=pt->x+motion->x;
	circle_pnt.y=pt->y;
	circle_pnt.z=pt->z+motion->z;

		// find min and max for box

	min.x=circle_pnt.x-radius;
	max.x=circle_pnt.x+radius;
	min.z=circle_pnt.z-radius;
	max.z=circle_pnt.z+radius;

	min.y=pt->y-box_sz->y;
	max.y=pt->y;

		// collide against map

	cur_dist=-1;
	
	contact->obj_idx=-1;
	contact->proj_idx=-1;
	contact->hit_poly.mesh_idx=-1;
	contact->hit_poly.poly_idx=-1;
	
		// check objects

	if (check_objs) {

		for (n=0;n!=max_obj_list;n++) {
			if (n==skip_obj_idx) continue;

			chk_obj=server.obj_list.objs[n];
			if (chk_obj==NULL) continue;
			
			if (!is_proj) {
				if (!chk_obj->contact.object_on) continue;
			}
			else {
				if (!chk_obj->contact.projectile_on) continue;
			}
			
			if (collide_circle_check_object(&circle_pnt,radius,&min,&max,TRUE,&cur_dist,chk_obj,&cur_hit_pnt)) {
				contact->obj_idx=n;
				contact->proj_idx=-1;
				contact->hit_poly.mesh_idx=-1;
				contact->hit_poly.poly_idx=-1;
			}
		}

	}

		// check projectiles

	if (check_projs) {
		for (n=0;n!=max_proj_list;n++) {
			if (n==skip_proj_idx) continue;

			chk_proj=server.proj_list.projs[n];
			if (!chk_proj->on) continue;

			if (collide_circle_check_projectile(&circle_pnt,radius,&min,&max,&cur_dist,chk_proj,&cur_hit_pnt)) {
				contact->obj_idx=-1;
				contact->proj_idx=n;
				contact->hit_poly.mesh_idx=-1;
				contact->hit_poly.poly_idx=-1;
			}
		}
	}

		// check meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (collide_circle_check_mesh(&circle_pnt,radius,&min,&max,FALSE,0,&cur_dist,n,&idx,&cur_hit_pnt)) {
			contact->obj_idx=-1;
			contact->proj_idx=-1;
			contact->hit_poly.mesh_idx=n;
			contact->hit_poly.poly_idx=idx;
		}
	}

		// any hits?

	if (cur_dist!=-1) {
		circle_get_point_on_radius_through_hit_point(&circle_pnt,radius,&cur_hit_pnt,&radius_pnt);
	
		motion->x-=radius_pnt.x-cur_hit_pnt.x;
		motion->z-=radius_pnt.z-cur_hit_pnt.z;

		return(TRUE);	
	}

	return(FALSE);
}

bool collide_box_slide_to_map(d3pnt *pt,d3pnt *box_sz,d3pnt *motion,bool check_objs,int skip_obj_idx,bool check_projs,int skip_proj_idx,obj_contact *contact)
{
	d3pnt			motion2;
	obj_contact		contact2;

		// try to move

	if (!collide_box_to_map(pt,box_sz,motion,check_objs,skip_obj_idx,check_projs,skip_proj_idx,FALSE,contact)) return(FALSE);

		// we had a hit ... see if new motion
		// is free or hits some other object

	motion2.x=motion->x;
	motion2.y=motion->y;
	motion2.z=motion->z;

	if (!collide_box_to_map(pt,box_sz,&motion2,check_objs,skip_obj_idx,check_projs,skip_proj_idx,FALSE,&contact2)) return(FALSE);

		// hit same object?

	if ((contact2.obj_idx!=-1) && (contact2.obj_idx==contact->obj_idx)) return(TRUE);
	if ((contact2.proj_idx!=-1) && (contact2.proj_idx==contact->proj_idx)) return(TRUE);
	if (contact2.hit_poly.mesh_idx!=-1) {
		if ((contact2.hit_poly.mesh_idx==contact->hit_poly.mesh_idx) && (contact2.hit_poly.poly_idx==contact->hit_poly.poly_idx)) return(TRUE);
	}

		// if not, then reduce any
		// movement that goes into
		// the other object

	if (((motion2.x*motion->x)<0) || (motion2.x==0)) motion->x=0;
	if (((motion2.z*motion->z)<0) || (motion2.z==0)) motion->z=0;

	return(TRUE);
}

bool collide_box_to_sphere(d3pnt *sphere_pnt,int radius,d3pnt *pt,d3pnt *box_sz)
{
	int			dist,box_radius;
	double		dx,dy,dz;

		// get the box radius

	box_radius=box_sz->x;
	if (box_sz->z>box_radius) box_radius=box_sz->z;

	box_radius=box_radius>>1;

		// see if distance between points
		// is less than both radiuses

	dx=sphere_pnt->x-pt->x;
	dy=sphere_pnt->y-pt->y;
	dz=sphere_pnt->z-pt->z;

	dist=(int)sqrt((dx*dx)+(dy*dy)+(dz*dz));

	return(dist<=(box_radius+radius));
}

/* =======================================================

      Object Collisions
      
======================================================= */

bool collide_object_to_map(obj_type *obj,d3pnt *motion)
{
	int					n,nhit_box;
	bool				hit_box_hit;
	d3pnt				pt,box_sz;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;

		// are we using hit box hits?

	hit_box_hit=FALSE;

	if (obj->hit_box.on) {
		draw=&obj->draw;
		if ((draw->model_idx!=-1) && (draw->on)) {
			model=server.model_list.models[draw->model_idx];
			hit_box_hit=TRUE;
		}
	}
		
	obj->hit_box.obj_hit_box_idx=-1;

		// regular collisions

	if (!hit_box_hit) {
		box_sz.x=obj->size.x;
		box_sz.z=obj->size.z;

		box_sz.y=obj->size.y;
		if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;

		return(collide_box_slide_to_map(&obj->pnt,&box_sz,motion,obj->contact.object_on,obj->idx,FALSE,-1,&obj->contact));
	}

		// hit box collisions

	else {

		hit_box=model->hit_boxes;
		nhit_box=model->nhit_box;
		
		for (n=0;n!=nhit_box;n++) {
			model_hit_box_get_center(model,n,&obj->pnt,&draw->setup,&pt);

			box_sz.x=hit_box->box.size.x;
			box_sz.z=hit_box->box.size.z;

			box_sz.y=hit_box->box.size.y;
			if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;
			
			if (collide_box_slide_to_map(&pt,&box_sz,motion,obj->contact.object_on,obj->idx,FALSE,-1,&obj->contact)) {
				obj->hit_box.obj_hit_box_idx=n;
				return(TRUE);
			}

			hit_box++;
		}
	}

	return(FALSE);
}

bool collide_object_to_map_bump(obj_type *obj,d3pnt *motion,int *bump_y_move)
{
	int						n,radius,cur_dist,ty,idx;
	d3pnt					circle_pnt,cur_hit_pnt,
							min,max;
	obj_type				*chk_obj;
	map_mesh_poly_type		*poly;

		// get the circle radius

	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;

	radius=radius>>1;

		// get the moved circle

	circle_pnt.x=obj->pnt.x+motion->x;
	circle_pnt.y=obj->pnt.y;
	circle_pnt.z=obj->pnt.z+motion->z;

		// find min and max for object

	min.x=circle_pnt.x-radius;
	max.x=circle_pnt.x+radius;
	min.z=circle_pnt.z-radius;
	max.z=circle_pnt.z+radius;

		// min y is top of bump over

	max.y=obj->pnt.y+motion->y;
	min.y=max.y-obj->bump.high;
	
	cur_dist=-1;

		// check meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (collide_circle_check_mesh(&circle_pnt,radius,&min,&max,TRUE,obj->bump.high,&cur_dist,n,&idx,&cur_hit_pnt)) {
			poly=&map.mesh.meshes[n].polys[idx];
			*bump_y_move=poly->box.min.y-obj->pnt.y;		// don't use Y added version, it bumps too much on slopes
			return(TRUE);
		}
	}

		// check objects

	for (n=0;n!=max_obj_list;n++) {
		chk_obj=server.obj_list.objs[n];
		if (chk_obj==NULL) continue;

		if (chk_obj==obj) continue;

			// bump candidate?

		ty=chk_obj->pnt.y-chk_obj->size.y;
		if ((ty<min.y) || (chk_obj->pnt.y>=max.y)) continue;

			// check collision

		if (collide_circle_check_object(&circle_pnt,radius,&min,&max,TRUE,&cur_dist,chk_obj,&cur_hit_pnt)) {
			*bump_y_move=ty-obj->pnt.y;		// don't use Y added version, it bumps too much on slopes
			return(TRUE);
		}
	}

	return(FALSE);
}

bool collide_object_to_mesh(obj_type *obj,int mesh_idx)
{
	int						radius,cur_dist,idx;
	d3pnt					circle_pnt,cur_hit_pnt,
							min,max;

		// get the circle radius

	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;

	radius=radius>>1;

		// get the circle

	circle_pnt.x=obj->pnt.x;
	circle_pnt.y=obj->pnt.y;
	circle_pnt.z=obj->pnt.z;

		// find min and max for object

	min.x=circle_pnt.x-radius;
	max.x=circle_pnt.x+radius;
	min.z=circle_pnt.z-radius;
	max.z=circle_pnt.z+radius;
	max.y=circle_pnt.y;
	min.y=max.y-obj->size.y;

		// check mesh
		
	cur_dist=-1;

	return(collide_circle_check_mesh(&circle_pnt,radius,&min,&max,FALSE,0,&cur_dist,mesh_idx,&idx,&cur_hit_pnt));
}

bool collide_object_to_object(obj_type *obj,d3pnt *motion,obj_type *chk_obj,bool skip_pickup)
{
	int						radius,cur_dist;
	d3pnt					circle_pnt,cur_hit_pnt,
							min,max;
							
		// get the circle radius

	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;

	radius=radius>>1;

		// get the circle

	circle_pnt.x=obj->pnt.x;
	circle_pnt.y=obj->pnt.y;
	circle_pnt.z=obj->pnt.z;
	
	if (motion!=NULL) {
		circle_pnt.x+=motion->x;
		circle_pnt.y+=motion->y;
		circle_pnt.z+=motion->z;
	}

		// find min and max for object

	min.x=circle_pnt.x-radius;
	max.x=circle_pnt.x+radius;
	min.z=circle_pnt.z-radius;
	max.z=circle_pnt.z+radius;
	max.y=circle_pnt.y;
	min.y=max.y-obj->size.y;

		// check mesh
		
	cur_dist=-1;
	
	return(collide_circle_check_object(&circle_pnt,radius,&min,&max,skip_pickup,&cur_dist,chk_obj,&cur_hit_pnt));
}

int collide_object_for_object_stand(obj_type *obj)
{
	int			n,uid,y,ty,ydist;
	d3pnt		motion;
	obj_type	*stand_obj;

	y=obj->pnt.y;
	ydist=floor_slop;
	
		// stand on are 1 point
		// above the standing on object
		
	motion.x=motion.z=0;
	motion.y=1;
	
		// find stand on object
	
	uid=-1;
	
	for (n=0;n!=max_obj_list;n++) {
		stand_obj=server.obj_list.objs[n];
		if (stand_obj==NULL) continue;
	
		if ((stand_obj->idx==obj->idx) || (stand_obj->hidden) || (!stand_obj->contact.object_on) || (stand_obj->pickup.on)) continue;
		
		ty=abs(y-(stand_obj->pnt.y-stand_obj->size.y));
		if (ty>ydist) continue;

		if (collide_object_to_object(obj,&motion,stand_obj,TRUE)) {
			uid=stand_obj->idx;
			ydist=ty;
		}
	}

	return(uid);	
}

bool collide_object_to_sphere(d3pnt *sphere_pnt,int radius,obj_type *obj)
{
	int					n,nhit_box;
	bool				hit_box_hit;
	d3pnt				pt,box_sz;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;

		// are we using hit box hits?

	hit_box_hit=FALSE;

	if (obj->hit_box.on) {
		draw=&obj->draw;
		if ((draw->model_idx!=-1) && (draw->on)) {
			model=server.model_list.models[draw->model_idx];
			hit_box_hit=TRUE;
		}
	}
		
	obj->hit_box.obj_hit_box_idx=-1;

		// regular collisions

	if (!hit_box_hit) {
		box_sz.x=obj->size.x;
		box_sz.z=obj->size.z;

		box_sz.y=obj->size.y;
		if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;

		return(collide_box_to_sphere(sphere_pnt,radius,&obj->pnt,&box_sz));
	}

		// hit box collisions

	else {

		hit_box=model->hit_boxes;
		nhit_box=model->nhit_box;
		
		for (n=0;n!=nhit_box;n++) {
			model_hit_box_get_center(model,n,&obj->pnt,&draw->setup,&pt);

			box_sz.x=hit_box->box.size.x;
			box_sz.z=hit_box->box.size.z;

			box_sz.y=hit_box->box.size.y;
			if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;
			
			if (collide_box_to_sphere(sphere_pnt,radius,&obj->pnt,&box_sz)) {
				obj->hit_box.obj_hit_box_idx=n;
				return(TRUE);
			}

			hit_box++;
		}
	}

	return(FALSE);
}

/* =======================================================

      Project-Map Collisions
      
======================================================= */

bool collide_projectile_to_map(proj_type *proj,d3pnt *motion)
{
	int					skip_obj_idx,skip_proj_idx;
	bool				check_projs;
	d3pnt				box_sz;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;

		// setup contacts

	skip_obj_idx=-1;
	if (proj->parent_grace>0) skip_obj_idx=proj->obj_idx;
	
	obj=server.obj_list.objs[proj->obj_idx];
	weap=obj->weap_list.weaps[proj->weap_idx];
	proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];

	check_projs=FALSE;
	skip_proj_idx=-1;

	if (proj_setup->collision) {
		check_projs=TRUE;
		skip_proj_idx=proj->idx;
	}

		// run the collisions

	box_sz.x=proj->size.x;
	box_sz.y=proj->size.y;
	box_sz.z=proj->size.z;

	return(collide_box_to_map(&proj->pnt,&box_sz,motion,TRUE,skip_obj_idx,check_projs,skip_proj_idx,TRUE,&proj->contact));
}

bool collide_projectile_to_sphere(d3pnt *sphere_pnt,int radius,proj_type *proj)
{
	d3pnt				box_sz;

	box_sz.x=proj->size.x;
	box_sz.y=proj->size.y;
	box_sz.z=proj->size.z;

	return(collide_box_to_sphere(sphere_pnt,radius,&proj->pnt,&box_sz));
}

/* =======================================================

      Pushing
      
======================================================= */

void collide_objects_push(d3pnt *push_pnt,int radius,int force)
{
	int			n,yhit;
	d3pnt		box_sz;
	d3ang		ang;
	obj_type	*obj;
	
		// check objects
		
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->hidden) || (obj->suspend) || (!obj->contact.object_on)) continue;
		
			// check for collision

		box_sz.x=obj->size.x;
		box_sz.y=obj->size.y;
		box_sz.z=obj->size.z;
			
		if (!collide_box_to_sphere(push_pnt,radius,&obj->pnt,&box_sz)) continue;
		
			// add push
				
		yhit=obj->pnt.y-(obj->size.y/2);
		
		ang.x=angle_find(push_pnt->y,push_pnt->z,yhit,obj->pnt.z);
		ang.y=angle_find(push_pnt->x,push_pnt->z,obj->pnt.x,obj->pnt.z);
		ang.z=0;
		
		object_push(obj,&ang,force,FALSE);
	}
}

/* =======================================================

      Polygon APIs
      
======================================================= */

int collide_polygon_find_faced_by_object(obj_type *obj)
{
	int						sz,xadd,zadd;
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

		// setup ray trace

	spt.x=obj->pnt.x;
	spt.y=obj->pnt.y-(obj->size.y>>1);
	spt.z=obj->pnt.z;

	sz=object_get_radius(obj)*10;
	angle_get_movement(obj->ang.y,sz,&xadd,&zadd);

	ept.x=spt.x+xadd;
	ept.y=spt.y;
	ept.z=spt.z+zadd;

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_object;

		// run trace

	if (!ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) return(-1);

		// return polygon contact

	if (contact.poly.mesh_idx==-1) return(-1);

	return((contact.poly.mesh_idx<<16)|contact.poly.poly_idx);
}

int collide_polygon_distance_to_object(int poly_uid,obj_type *obj)
{
	int					mesh_idx,poly_idx,xadd,zadd,x,z;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh_idx=poly_uid>>16;
	if ((mesh_idx<0) || (mesh_idx>=map.mesh.nmesh)) return(-1);
	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=(poly_uid&0xFFFF);
	if ((poly_idx<0) || (poly_idx>=mesh->npoly)) return(-1);
	poly=&mesh->polys[poly_idx];

	angle_get_movement(obj->ang.y,map_max_size,&xadd,&zadd);
	if (!line_2D_get_intersect(obj->pnt.x,obj->pnt.z,(obj->pnt.x+xadd),(obj->pnt.z+zadd),poly->line.lx,poly->line.lz,poly->line.rx,poly->line.rz,&x,&z)) return(-1);

	return(distance_2D_get(obj->pnt.x,obj->pnt.z,x,z));
}

void collide_polygon_hit_point_to_object(int poly_uid,obj_type *obj,d3pnt *pt)
{
	int					mesh_idx,poly_idx,xadd,zadd,x,z;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	pt->x=obj->pnt.x;
	pt->y=obj->pnt.y;
	pt->z=obj->pnt.z;

	mesh_idx=poly_uid>>16;
	if ((mesh_idx<0) || (mesh_idx>=map.mesh.nmesh)) return;
	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=(poly_uid&0xFFFF);
	if ((poly_idx<0) || (poly_idx>=mesh->npoly)) return;
	poly=&mesh->polys[poly_idx];

	angle_get_movement(obj->ang.y,map_max_size,&xadd,&zadd);
	if (!line_2D_get_intersect(obj->pnt.x,obj->pnt.z,(obj->pnt.x+xadd),(obj->pnt.z+zadd),poly->line.lx,poly->line.lz,poly->line.rx,poly->line.rz,&x,&z)) return;

	pt->x=x;
	pt->z=z;
}

void collide_polygon_get_normal(int poly_uid,d3vct *normal)
{
	int					mesh_idx,poly_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	normal->x=normal->y=normal->z=0.0f;

	mesh_idx=poly_uid>>16;
	if ((mesh_idx<0) || (mesh_idx>=map.mesh.nmesh)) return;
	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=(poly_uid&0xFFFF);
	if ((poly_idx<0) || (poly_idx>=mesh->npoly)) return;
	poly=&mesh->polys[poly_idx];

	normal->x=poly->tangent_space.normal.x;
	normal->y=poly->tangent_space.normal.y;
	normal->z=poly->tangent_space.normal.z;
}

float collide_polygon_dot_product_to_object(int poly_uid,obj_type *obj)
{
	int					mesh_idx,poly_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh_idx=poly_uid>>16;
	if ((mesh_idx<0) || (mesh_idx>=map.mesh.nmesh)) return(0.0f);
	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=(poly_uid&0xFFFF);
	if ((poly_idx<0) || (poly_idx>=mesh->npoly)) return(0.0f);
	poly=&mesh->polys[poly_idx];

	return(((poly->tangent_space.normal.x*(float)(poly->box.mid.x-obj->pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-obj->pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-obj->pnt.z))));
}

