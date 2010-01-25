/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Creation

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

#include "interface.h"
#include "dialog.h"
#include "common_view.h"

extern d3pnt					view_pnt;
extern int						drag_mode;
extern bool						dp_object,dp_node,dp_lightsoundparticle,dp_liquid,dp_area;

extern file_path_setup_type		file_path_setup;
extern map_type					map;

/* =======================================================

      Check if Piece Creation OK
      
======================================================= */

bool piece_create_texture_ok(void)
{
	if (map.textures[0].frames[0].bitmap.gl_id!=-1) return(TRUE);

	dialog_alert("You need at least one texture","You need a default texture in the first spot to add pieces with.  Please add a texture (by double clicking the first open texture) before adding pieces to the map.");
	return(FALSE);
}

/* =======================================================

      Find Create Spot
      
======================================================= */

void piece_create_get_spot(d3pnt *pnt)
{
	d3pnt			min,max;
	
		// center in view if no selection
		
	if (select_count()==0) {
		pnt->x=view_pnt.x;
		pnt->z=view_pnt.z;
		pnt->y=view_pnt.y;
	}
	
		// find place in selection
		
	else {

		select_get_extent(&min,&max);
		pnt->x=(min.x+max.x)>>1;
		pnt->z=(min.z+max.z)>>1;
		pnt->y=((min.y+max.y)>>1)-1;
	}
}

/* =======================================================

      Create Spot
      
======================================================= */

void piece_create_spot(void)
{
	int				index;
	spot_type		*spot;
	
	if (map.nspot==max_spot) {
		dialog_alert("Can Not Create Spot","You've reached the maximum number of spots for this map.");
	}
			
		// create spot
		
	index=map.nspot;
	
	spot=&map.spots[index];
 	map.nspot++;
	
	piece_create_get_spot(&spot->pnt);
    
	spot->ang.y=0;
	spot->name[0]=0x0;
	strcpy(spot->type,"Object");
	spot->script[0]=0x0;
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
    
		// select spot
		
    dp_object=TRUE;
	
	select_clear();
	select_add(spot_piece,index,-1);
	
	main_wind_draw();
	
	menu_set_show_hide_check();
	main_wind_tool_reset();
	
	main_wind_tool_fill_spot_combo();
}

/* =======================================================

      Create Scenery
      
======================================================= */

void piece_create_scenery(void)
{
	int					n,index;
	map_scenery_type	*scenery;
	
	if (map.nscenery==max_map_scenery) {
		dialog_alert("Can Not Create Scenery","You've reached the maximum number of sceneries for this map.");
	}
			
		// create spot
		
	index=map.nscenery;
	
	scenery=&map.sceneries[index];
 	map.nscenery++;
	
	piece_create_get_spot(&scenery->pnt);
    
	scenery->ang.x=0.0f;
	scenery->ang.y=0.0f;
	scenery->ang.z=0.0f;
	
	scenery->resize=1.0f;
	
	scenery->model_name[0]=0x0;
	scenery->animation_name[0]=0x0;
	scenery->contact_object_on=TRUE;
	scenery->contact_projectile_on=TRUE;
	scenery->contact_hit_box=TRUE;
	scenery->face_forward=FALSE;
	scenery->shadow=FALSE;
	
	for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
		scenery->texture_frame[n]=0;
	}
    
		// select spot
		
    dp_object=TRUE;
	
	select_clear();
	select_add(scenery_piece,index,-1);
	
	main_wind_draw();
	
	menu_set_show_hide_check();
	main_wind_tool_reset();
	
	main_wind_tool_fill_scenery_combo();
}

/* =======================================================

      Create Light
      
======================================================= */

void piece_create_light(void)
{
	int					index;
	map_light_type		*lit;
	
	if (map.nlight==max_map_light) {
		dialog_alert("Can Not Create Light","You've reached the maximum number of lights for this map.");
        return;
    }
	
		// create light
    
	index=map.nlight;
	
	lit=&map.lights[index];
	map.nlight++;
	
	piece_create_get_spot(&lit->pnt);

	lit->type=lt_normal;
	lit->col.r=lit->col.g=lit->col.b=1;
	lit->direction=ld_all;
	lit->intensity=10000;
	lit->exponent=1.0f;
	lit->on=TRUE;
	lit->light_map=TRUE;
	
		// select light
		
	dp_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(light_piece,index,-1);
	
	main_wind_draw();
	menu_set_show_hide_check();
	
	main_wind_tool_fill_light_combo();
}

/* =======================================================

      Create Sound
      
======================================================= */

void piece_create_sound(void)
{
	int					index;
	map_sound_type		*snd;
	
	if (map.nsound==max_map_sound) {
		dialog_alert("Can Not Create Sound","You've reached the maximum number of sounds for this map.");
        return;
    }
	
		// create sound
	
	index=map.nsound;
	
	snd=&map.sounds[index];
	map.nsound++;
	
	piece_create_get_spot(&snd->pnt);
    
	snd->pitch=1;
	snd->name[0]=0x0;
	snd->on=TRUE;
	
		// select sound
		
    dp_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(sound_piece,index,-1);
	
	main_wind_draw();
	menu_set_show_hide_check();
	
	main_wind_tool_fill_sound_combo();
}

/* =======================================================

      Create Particle
      
======================================================= */

void piece_create_particle(void)
{
	int					index;
	map_particle_type	*prt;
	
	if (map.nparticle==max_map_particle) {
		dialog_alert("Can Not Create Particle","You've reached the maximum number of particles for this map.");
        return;
    }
	
		// create particle
	
	index=map.nparticle;
	
	prt=&map.particles[index];
	map.nparticle++;
	
	piece_create_get_spot(&prt->pnt);
    
	prt->spawn_tick=5000;
	prt->slop_tick=0;
	prt->name[0]=0x0;
	prt->on=TRUE;
	
		// select particle
		
    dp_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(particle_piece,index,-1);
	
	main_wind_draw();
	menu_set_show_hide_check();
	
	main_wind_tool_fill_particle_combo();
}

/* =======================================================

      Create Node
      
======================================================= */

void piece_create_node(void)
{
	int				i,index;
	node_type		*node;
			
	if (map.nnode==max_node) {
		dialog_alert("Can Not Create Node","You've reached the maximum number of nodes for a map.");
	}
	
		// create node
	
	index=map.nnode;
	
	node=&map.nodes[index];
	map.nnode++;
	
	piece_create_get_spot(&node->pnt);
	
	node->ang.x=0.0f;
	node->ang.y=0.0f;
	node->ang.z=0.0f;
	node->name[0]=0x0;
	node->event_id=0;

	for (i=0;i!=max_node_link;i++) {
		node->link[i]=-1;
	}

		// select sound
		
	dp_node=TRUE;
	
	select_clear();
	select_add(node_piece,index,-1);
	
	main_wind_draw();
	
	menu_set_show_hide_check();
	main_wind_tool_reset();
	
	main_wind_tool_fill_node_combo();
}

/* =======================================================

      Create Liquid
	        
======================================================= */

void piece_create_liquid(void)
{
	int				index,sz;
	d3pnt			pnt;
	map_liquid_type	*liq;

	if (!piece_create_texture_ok()) return;

	piece_create_get_spot(&pnt);
	
		// create the liquid
		
	index=map_liquid_add(&map);
	if (index==-1) {
		dialog_alert("Can Not Create Liquid","Not enough memory.");
		return;
	}
	
	liq=&map.liquid.liquids[index];
	
	sz=map_enlarge*4;
	liq->lft=pnt.x-sz;
	liq->rgt=pnt.x+sz;
	liq->top=pnt.z-sz;
	liq->bot=pnt.z+sz;
	
	liq->y=pnt.y;
	liq->depth=5000;
	
	liq->txt_idx=0;
	liq->lmap_txt_idx=-1;
	liq->group_idx=-1;
	liq->never_obscure=FALSE;
	
	liq->tint_alpha=0.5f;
	liq->speed_alter=1.0f;
	
	liq->col.r=liq->col.g=liq->col.b=1.0f;
	
	liq->uv[0].x_offset=liq->uv[0].y_offset=0.0f;
	liq->uv[0].x_size=liq->uv[0].y_size=1.0f;
    liq->x_shift=liq->y_shift=0.0f;
	
	liq->tide.rate=1000;
	liq->tide.high=100;
	liq->tide.division=liquid_min_division;
	liq->tide.direction=liquid_direction_horizontal;
	liq->tide.flat=FALSE;
	
	liq->harm.in_harm=0;
	liq->harm.drown_tick=10000;
	liq->harm.drown_harm=10;

		// select the liquid
		
	dp_liquid=TRUE;
	select_clear();
	select_add(liquid_piece,index,-1);

	main_wind_draw();
	
	menu_set_show_hide_check();
	main_wind_tool_reset();
}

/* =======================================================

      Create Area
	        
======================================================= */

void piece_create_area(void)
{
	int				index,sz;
	map_area_type	*area;
	Point			pt;
	RGBColor		color;
	
		// create the area
		
	if (map.narea>max_area) {
		dialog_alert("Can Not Create Area","Reach maximum number of areas.");
		return;
	}
	
	index=map.narea;
	
	color.red=color.green=color.blue=0xFFFF;
	
	pt.h=pt.v=-1;
	if (!GetColor(pt,"\pChoose the Area Color:",&color,&color)) return;

	area=&map.areas[index];
	map.narea++;
	
	sz=map_enlarge*10;
	area->lft=view_pnt.x-sz;
	area->rgt=view_pnt.x+sz;
	area->top=view_pnt.z-sz;
	area->bot=view_pnt.z+sz;

	area->col.r=((float)color.red/(float)0xFFFF);
	area->col.g=((float)color.green/(float)0xFFFF);
	area->col.b=((float)color.blue/(float)0xFFFF);

		// select the liquid
		
	dp_area=TRUE;
	select_clear();
	select_add(area_piece,index,-1);

	main_wind_draw();
	
	menu_set_show_hide_check();
	main_wind_tool_reset();
}

