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

bool		test_me_ok=FALSE;	// supergumba
d3pnt		test_me_spnt[32],test_me_epnt[32];
int			test_me_px[21],test_me_py[21],test_me_pz[21];

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

int collide_point_distance(d3pnt *pt_1,d3pnt *pt_2)
{
	double		dx,dz,dy;
	
	dx=(double)(pt_1->x-pt_2->x);
	dy=(double)(pt_1->y-pt_2->y);
	dz=(double)(pt_1->z-pt_2->z);
	
	return((int)sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

void collide_object_box_ray_trace_points(d3pnt *pt,d3pnt *box_sz,int side,float face_ang,int *px,int *py,int *pz)
{
	int			ty,by,y_sz;

		// get points for that poly face

	switch (side) {

			// front side

		case 0:
			pz[0]=pz[1]=pz[2]=pz[3]=pz[4]=pz[5]=pz[6]=pz[7]=pz[8]=pz[9]=pz[10]=pz[11]=pz[12]=pz[13]=pz[14]=pz[15]=pt->z-(box_sz->z>>1);
			px[0]=px[3]=px[6]=px[9]=px[12]=pt->x-(box_sz->x>>1);
			px[1]=px[4]=px[7]=px[10]=px[13]=pt->x;
			px[2]=px[5]=px[8]=px[11]=px[14]=pt->x+(box_sz->x>>1);
			break;

			// right side

		case 1:
			px[0]=px[1]=px[2]=px[3]=px[4]=px[5]=px[6]=px[7]=px[8]=px[9]=px[10]=px[11]=px[12]=px[13]=px[14]=px[15]=pt->x+(box_sz->x>>1);
			pz[0]=pz[3]=pz[6]=pz[9]=pz[12]=pt->z-(box_sz->z>>1);
			pz[1]=pz[4]=pz[7]=pz[10]=pz[13]=pt->z;
			pz[2]=pz[5]=pz[8]=pz[11]=pz[14]=pt->z+(box_sz->z>>1);
			break;

			// back side

		case 2:
			pz[0]=pz[1]=pz[2]=pz[3]=pz[4]=pz[5]=pz[6]=pz[7]=pz[8]=pz[9]=pz[10]=pz[11]=pz[12]=pz[13]=pz[14]=pz[15]=pt->z+(box_sz->z>>1);
			px[0]=px[3]=px[6]=px[9]=px[12]=pt->x-(box_sz->x>>1);
			px[1]=px[4]=px[7]=px[10]=px[13]=pt->x;
			px[2]=px[5]=px[8]=px[11]=px[14]=pt->x+(box_sz->x>>1);
			break;

			// left side

		case 3:
			px[0]=px[1]=px[2]=px[3]=px[4]=px[5]=px[6]=px[7]=px[8]=px[9]=px[10]=px[11]=px[12]=px[13]=px[14]=px[15]=pt->x-(box_sz->x>>1);
			pz[0]=pz[3]=pz[6]=pz[9]=pz[12]=pt->z-(box_sz->z>>1);
			pz[1]=pz[4]=pz[7]=pz[10]=pz[13]=pt->z;
			pz[2]=pz[5]=pz[8]=pz[11]=pz[14]=pt->z+(box_sz->z>>1);
			break;

	}

		// rotate face

	rotate_2D_polygon(15,px,pz,pt->x,pt->z,face_ang);

		// setup Ys
		
	ty=pt->y-box_sz->y;
	by=pt->y;
	
	y_sz=(by-ty)>>2;

	py[0]=py[1]=py[2]=ty;
	py[3]=py[4]=py[5]=ty+y_sz;
	py[6]=py[7]=py[8]=(ty+by)>>1;
	py[9]=py[10]=py[11]=by-y_sz;
	py[12]=py[13]=py[14]=by-(map_enlarge>>1);
}

void test_me(void)	// supergumba
{
	int		n;
	if (!test_me_ok) return;

			glColor4f(1.0f,1.0f,0.0f,1.0f);
		glBegin(GL_LINES);
		for (n=0;n!=21;n++) {
			glVertex3i(test_me_spnt[n].x,test_me_spnt[n].y,test_me_spnt[n].z);
			glVertex3i(test_me_epnt[n].x,test_me_epnt[n].y,test_me_epnt[n].z);
		}
		glEnd();
		glColor4f(1.0f,0.0f,1.0f,1.0f);
		glBegin(GL_LINE_LOOP);
			glVertex3i(test_me_px[0],test_me_py[0],test_me_pz[0]);
			glVertex3i(test_me_px[2],test_me_py[2],test_me_pz[2]);
			glVertex3i(test_me_px[14],test_me_py[14],test_me_pz[14]);
			glVertex3i(test_me_px[12],test_me_py[12],test_me_pz[12]);
		glEnd();
}

bool collide_object_box_to_map(obj_type *obj,d3pnt *pt,d3pnt *box_sz,int *xadd,int *yadd,int *zadd)
{
	int						n,x,z,idx,xadd2,yadd2,zadd2,mvx,mvz,radius,
							px[21],py[21],pz[21],
							d,dist,mv_dist,side,move_side,face_side;
	float					move_ang,face_ang;
	bool					bump,hits[21];
	d3pnt					spt[21],ept[21],hpt[21];
	map_mesh_poly_type		*poly;
	ray_trace_contact_type	base_contact,contacts[21];

		// determine the polygon face to
		// check hit points for

	move_ang=angle_find(0,0,(*xadd),(*zadd));
	face_ang=angle_add(obj->ang.y,obj->draw.rot.y);

	move_side=((int)((move_ang+45.0f)/90.0f))&0x3;
	face_side=((int)((face_ang+45.0f)/90.0f))&0x3;
	side=(move_side-face_side)&0x3;

		// run our ray traces from the opposite
		// side through the polygon side we
		// are checking hits for

	xadd2=*xadd;
	yadd2=*yadd;
	zadd2=*zadd;
/* supergumba -- test
	switch (side) {

			// front side

		case 0:
			angle_get_movement(face_ang,obj->size.z,&mvx,&mvz);
			xadd2+=mvx;
			zadd2+=mvz;
			side=2;
			break;

			// right side

		case 1:
			angle_get_movement(angle_add(face_ang,90.0f),obj->size.x,&mvx,&mvz);
			xadd2+=mvx;
			zadd2+=mvz;
			side=3;
			break;

			// back side

		case 2:
			angle_get_movement(angle_add(face_ang,180.0f),obj->size.z,&mvx,&mvz);
			xadd2+=mvx;
			zadd2+=mvz;
			side=0;
			break;

			// left side

		case 3:
			angle_get_movement(angle_add(face_ang,270.0f),obj->size.x,&mvx,&mvz);
			xadd2+=mvx;
			zadd2+=mvz;
			side=1;
			break;
	}
*/
		// get collision points

	collide_object_box_ray_trace_points(pt,box_sz,side,face_ang,px,py,pz);

		// need to increase ray check distance
		// to avoid objects getting close to polygons
		// and then turning their corner into them

	radius=obj->size.radius;

	x=*xadd;
	z=*zadd;

	mv_dist=(int)(sqrt((double)(x*x)+(double)(z*z)));

	if (radius>mv_dist) {
		angle_get_movement(move_ang,radius,&xadd2,&zadd2);
		
		yadd2=*yadd;
		if (mv_dist!=0) yadd2=((*yadd)*radius)/mv_dist;
	}
	else {
		xadd2=x;
		zadd2=z;
		yadd2=*yadd;
	}

		// set the collisions and run the
		// ray tracing

	base_contact.obj.on=TRUE;
	base_contact.obj.ignore_uid=obj->uid;

	base_contact.proj.on=FALSE;
	base_contact.proj.ignore_uid=-1;

	base_contact.hit_mode=poly_ray_trace_hit_mode_wall_only;
	base_contact.origin=poly_ray_trace_origin_object;

		// straight forward rays

	for (n=0;n!=15;n++) {
		spt[n].x=px[n];
		spt[n].y=py[n];
		spt[n].z=pz[n];

		ept[n].x=spt[n].x+xadd2;
		ept[n].y=spt[n].y+yadd2;
		ept[n].z=spt[n].z+zadd2;
	}
		
		// cross-over rays

	spt[15].x=px[15]=px[0];
	spt[15].y=py[15]=py[0];
	spt[15].z=pz[15]=pz[0];
	ept[15].x=px[14]+xadd2;
	ept[15].y=py[14]+yadd2;
	ept[15].z=pz[14]+zadd2;

	spt[16].x=px[16]=px[2];
	spt[16].y=py[16]=py[2];
	spt[16].z=pz[16]=pz[2];
	ept[16].x=px[12]+xadd2;
	ept[16].y=py[12]+yadd2;
	ept[16].z=pz[12]+zadd2;

	spt[17].x=px[17]=px[12];
	spt[17].y=py[17]=py[12];
	spt[17].z=pz[17]=pz[12];
	ept[17].x=px[2]+xadd2;
	ept[17].y=py[2]+yadd2;
	ept[17].z=pz[2]+zadd2;

	spt[18].x=px[18]=px[14];
	spt[18].y=py[18]=py[14];
	spt[18].z=pz[18]=pz[14];
	ept[18].x=px[0]+xadd2;
	ept[18].y=py[0]+yadd2;
	ept[18].z=pz[0]+zadd2;

	spt[19].x=px[19]=px[6];
	spt[19].y=py[19]=py[6];
	spt[19].z=pz[19]=pz[6];
	ept[19].x=px[8]+xadd2;
	ept[19].y=py[8]+yadd2;
	ept[19].z=pz[8]+zadd2;

	spt[20].x=px[20]=px[8];
	spt[20].y=py[20]=py[8];
	spt[20].z=pz[20]=pz[8];
	ept[20].x=px[6]+xadd2;
	ept[20].y=py[6]+yadd2;
	ept[20].z=pz[6]+zadd2;

		// debug mode ray traces

	if (obj->player) {
		test_me_ok=TRUE;
		memmove(test_me_spnt,spt,(sizeof(d3pnt)*21));
		memmove(test_me_epnt,ept,(sizeof(d3pnt)*21));
		memmove(test_me_px,px,(sizeof(int)*21));
		memmove(test_me_py,py,(sizeof(int)*21));
		memmove(test_me_pz,pz,(sizeof(int)*21));
	}

		// run ray trace

	ray_trace_map_by_point_array(21,spt,ept,hpt,hits,&base_contact,contacts);

		// find the one that moves the leasts
		// as the most suitable hit point

	idx=-1;
	dist=0;

	for (n=0;n!=21;n++) {
			
		if (hits[n]) {
			d=collide_point_distance(&spt[n],&hpt[n]);
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
		
	if (contacts[idx].poly.mesh_idx!=-1) {
	
		if (obj->bump.on) {
			poly=&map.mesh.meshes[contacts[idx].poly.mesh_idx].polys[contacts[idx].poly.poly_idx];
			bump=((obj->pnt.y-poly->box.min.y)<=obj->bump.high);
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

		angle_get_movement(move_ang,(radius-d),&xadd2,&zadd2);
		*xadd=(*xadd)-xadd2;
		*zadd=(*zadd)-zadd2;
	}
	else {
		*xadd=hpt[idx].x-px[idx];
		*zadd=hpt[idx].z-pz[idx];
	}
	
		// setup the hits

	if (contacts[idx].poly.mesh_idx!=-1) {
		memmove(&obj->contact.hit_poly,&contacts[idx].poly,sizeof(poly_pointer_type));
	}

	if (contacts[idx].obj.uid!=-1) {
		obj->contact.obj_uid=contacts[idx].obj.uid;
		obj->contact.hit_face=contacts[idx].obj.hit_face;
	}

	if (contacts[idx].proj.uid!=-1) {
		obj->contact.proj_uid=contacts[idx].proj.uid;
		obj->contact.hit_face=contacts[idx].proj.hit_face;
	}

	return(TRUE);
}

bool collide_object_to_map(obj_type *obj,int *xadd,int *yadd,int *zadd)
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
		if ((draw->uid!=-1) && (!draw->on)) {
	
			model=model_find_uid(draw->uid);
			if (model!=NULL) {
				hit_box_hit=TRUE;
			}
		}
	}

		// regular collisions

	if (!hit_box_hit) {
		box_sz.x=obj->size.x;
		box_sz.z=obj->size.z;

		box_sz.y=obj->size.y;
		if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;

		return(collide_object_box_to_map(obj,&obj->pnt,&box_sz,xadd,yadd,zadd));
	}

		// hit box collisions

	else {

		hit_box=model->hit_boxes;
		nhit_box=model->nhit_box;
		
		for (n=0;n!=nhit_box;n++) {
			pt.x=obj->pnt.x+hit_box->box.offset.x;
			pt.y=obj->pnt.y+hit_box->box.offset.y;
			pt.z=obj->pnt.z+hit_box->box.offset.z;

			box_sz.x=hit_box->box.size.x;
			box_sz.z=hit_box->box.size.z;

			box_sz.y=hit_box->box.size.y;
			if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;
			
			if (collide_object_box_to_map(obj,&pt,&box_sz,xadd,yadd,zadd)) {
				obj->hit_box.obj_hit_box_idx=n;
				return(TRUE);
			}

			hit_box++;
		}
	}

	return(FALSE);
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

	contact.obj.on=TRUE;
	if (proj->parent_grace>0) {
		contact.obj.ignore_uid=proj->obj_uid;
	}
	else {
		contact.obj.ignore_uid=-1;
	}

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	contact.proj.on=proj_setup->collision;
	contact.proj.ignore_uid=proj->uid;

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

	proj->contact.obj_uid=contact.obj.uid;
	proj->contact.proj_uid=contact.proj.uid;
	memmove(&proj->contact.hit_poly,&contact.poly,sizeof(poly_pointer_type));

	return(TRUE);
}

