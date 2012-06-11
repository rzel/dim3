/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Simple Save Pick

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
	#include "dim3engine.h"
#endif

#include "interface.h"

#define simple_save_pick_frame_id				0
#define simple_save_pick_button_cancel_id		1
#define simple_save_pick_button_erase_id		2

#define simple_save_pick_erase_frame_id			10
#define simple_save_pick_erase_button_id		20
#define simple_save_pick_erase_cancel_id		30

#define simple_save_pick_button_start_id		100
#define simple_save_pick_description_id			200
#define simple_save_pick_progress_id			300

extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int					intro_simple_save_idx;

bool						simple_save_pick_esc_down,
							simple_save_pick_in_erase;

/* =======================================================

      Simple Save Pick Control Reset
      
======================================================= */

void simple_save_pick_reset_controls(void)
{
	int				n,bitmap_count;

		// load simple saves

	simple_save_xml_read(&iface);
	
		// simple save erase
		
	if (simple_save_pick_in_erase) {
	
		for (n=0;n!=max_simple_save_spot;n++) {
			element_enable((simple_save_pick_button_start_id+n),FALSE);
			element_enable((simple_save_pick_description_id+n),FALSE);
		}
		
		element_enable(simple_save_pick_button_erase_id,FALSE);
		element_enable(simple_save_pick_button_cancel_id,FALSE);
		
		element_hide(simple_save_pick_erase_frame_id,FALSE);
		
		for (n=0;n!=max_simple_save_spot;n++) {
			element_hide((simple_save_pick_erase_button_id+n),FALSE);
		}
		
		element_hide(simple_save_pick_erase_cancel_id,FALSE);
	}
	
		// regular picking
		
	else {
		for (n=0;n!=max_simple_save_spot;n++) {
			element_enable((simple_save_pick_button_start_id+n),TRUE);
			element_enable((simple_save_pick_description_id+n),TRUE);
		}
		
		element_enable(simple_save_pick_button_erase_id,TRUE);
		element_enable(simple_save_pick_button_cancel_id,TRUE);
		
		element_hide(simple_save_pick_erase_frame_id,TRUE);
		
		for (n=0;n!=max_simple_save_spot;n++) {
			element_hide((simple_save_pick_erase_button_id+n),TRUE);
		}
		
		element_hide(simple_save_pick_erase_cancel_id,TRUE);
	}
	
		// reset settings
		
	for (n=0;n!=max_simple_save_spot;n++) {
		if (iface.simple_save_list.saves[n].save_id!=-1) {
			element_text_change((simple_save_pick_description_id+n),iface.simple_save_list.saves[n].desc);
		}
		else {
			element_text_change((simple_save_pick_description_id+n),"New");
		}

		if (iface.intro.simple_save_progress.on) {
			if (simple_save_pick_in_erase) {
				bitmap_count=0;
			}
			else {
				bitmap_count=(iface.intro.simple_save_progress.max_bitmap*iface.simple_save_list.saves[n].points)/iface.intro.simple_save_progress.max_point;
			}
			element_set_value((simple_save_pick_progress_id+n),bitmap_count);
		}
	}
}

/* =======================================================

      Simple Save Pick Open and Close
      
======================================================= */

void simple_save_pick_open(void)
{
	int				n,x,y,bx,by,wid,high,
					txt_y_off,txt_size,
					bitmap_max,bitmap_size,bitmap_add,
					butt_wid,butt_high,save_wid,save_high,erase_wid,erase_high,
					padding,control_y_add;
	char			name[256],path[1024],disable_path[1024];

		// simple save UI
		
	gui_initialize("Bitmaps/Backgrounds","main");

		// get height

	padding=element_get_padding();
	control_y_add=element_get_control_high();
	
	butt_wid=element_get_button_short_wid();
	butt_high=element_get_button_high();
	
		// dialog and frame

	x=25;
	y=25+(control_y_add+padding);
	wid=iface.scale_x-50;
	high=iface.scale_y-(50+(control_y_add+padding));
	
	element_frame_add("Pick Save Spot",simple_save_pick_frame_id,x,y,wid,high);

		// simple save picker sizes

	bx=x+padding;
	by=y+padding;

	save_wid=wid-(padding*2);
	save_high=((high-((padding*2)+butt_high))/max_simple_save_spot)-padding;

		// calculate size of text + progress
		// by dividing it out
	
	txt_size=(save_high-(padding*3))/4;
	
	if (!iface.intro.simple_save_progress.on) {
		txt_y_off=(save_high/2)+(txt_size/2);
	}
	else {
		bitmap_max=iface.intro.simple_save_progress.max_bitmap;
		bitmap_size=((save_wid-(padding*2))/bitmap_max)-(padding/4);
		bitmap_add=bitmap_size+(padding/4);

		txt_y_off=((save_high/2)-(((txt_size+padding)+bitmap_size)/2))+(txt_size-(padding/2));
	}

		// add simple saves

	for (n=0;n!=max_simple_save_spot;n++) {
			
			// big button

		element_button_box_add((simple_save_pick_button_start_id+n),bx,by,save_wid,save_high,element_pos_left,element_pos_top);

			// description and progress

		element_text_add("",(simple_save_pick_description_id+n),(bx+padding),((by+txt_y_off)+padding),txt_size,tx_left,&iface.color.picker.text,FALSE);

		if (iface.intro.simple_save_progress.on) {
			file_paths_data(&setup.file_path_setup,path,"Bitmaps/Interface",iface.intro.simple_save_progress.bitmap_name,"png");
			file_paths_data(&setup.file_path_setup,disable_path,"Bitmaps/Interface",iface.intro.simple_save_progress.bitmap_disable_name,"png");
			element_count_add(path,disable_path,(simple_save_pick_progress_id+n),(bx+padding),((by+txt_y_off)+(padding*2)),bitmap_size,bitmap_size,bitmap_add,TRUE,0,0,bitmap_max);
		}

		by+=(save_high+padding);
	}
	
		// simple save erase
		
	erase_wid=(butt_wid*4)+(padding*5);
	erase_high=butt_high+(padding*2);
		
	bx=(x+(wid>>1))-(erase_wid>>1);
	by=(y+(high>>1))-(erase_high>>1);
	
	element_frame_add("Erase Save Spot",simple_save_pick_erase_frame_id,bx,by,erase_wid,erase_high);
	
	bx+=padding;
	by+=padding;
	
	for (n=0;n!=max_simple_save_spot;n++) {
		sprintf(name,"Erase %d",(n+1));
		element_button_text_add(name,(simple_save_pick_erase_button_id+n),bx,by,butt_wid,butt_high,element_pos_left,element_pos_top);
		bx+=(butt_wid+padding);
	}
	
	element_button_text_add("Cancel",simple_save_pick_erase_cancel_id,bx,by,butt_wid,butt_high,element_pos_left,element_pos_top);

		// erase button

	bx=x+padding;
	by=(y+high)-padding;

	element_button_text_add("Erase",simple_save_pick_button_erase_id,bx,by,butt_wid,butt_high,element_pos_left,element_pos_bottom);

		// cancel button

	bx=(x+wid)-padding;

	element_button_text_add("Cancel",simple_save_pick_button_cancel_id,bx,by,butt_wid,butt_high,element_pos_right,element_pos_bottom);
	
		// in key state
	
	simple_save_pick_esc_down=FALSE;
	
		// setup controls
		
	simple_save_pick_in_erase=FALSE;
	simple_save_pick_reset_controls();
}

void simple_save_pick_close(void)
{
	gui_shutdown();
}

/* =======================================================

      Simple Save Pick Input
      
======================================================= */

void simple_save_pick_click(void)
{
	int						id,simple_save_idx;
	char					err_str[256];
	iface_simple_save_type	*save;
	
		// element being clicked?
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// regular button clicks

	if (id==simple_save_pick_button_cancel_id) {
		server.next_state=gs_intro;
		return;
	}

		// turn erase dialog on/off
		
	if (id==simple_save_pick_button_erase_id) {
		simple_save_pick_in_erase=TRUE;
		simple_save_pick_reset_controls();
		return;
	}
	
	if (id==simple_save_pick_erase_cancel_id) {
		simple_save_pick_in_erase=FALSE;
		simple_save_pick_reset_controls();
		return;
	}
	
		// erase dialog buttong
		
	if ((id>=simple_save_pick_erase_button_id) && (id<(simple_save_pick_erase_button_id+max_simple_save_spot))) {

		simple_save_idx=id-simple_save_pick_erase_button_id;
		
			// reset save
			
		save=&iface.simple_save_list.saves[simple_save_idx];
		save->save_id=-1;
		save->points=0;
		save->desc[0]=0x0;
		
		simple_save_xml_write(&iface,err_str);
	
			// back to picker

		simple_save_pick_in_erase=FALSE;
		simple_save_pick_reset_controls();
		return;
	}

		// play button clicks

	if ((id>=simple_save_pick_button_start_id) && (id<(simple_save_pick_button_start_id+max_simple_save_spot))) {

		simple_save_idx=id-simple_save_pick_button_start_id;

			// if we have any options, then
			// go to options interface

		if (intro_click_has_singleplayer_options()) {
			intro_simple_save_idx=simple_save_idx;
			server.next_state=gs_singleplayer_option;
			return;
		}

			// otherwise, just start the
			// game

		intro_start_game(skill_medium,0,NULL,simple_save_idx);
		return;
	}

/*		
	switch (id) {
	
		case singleplayer_option_button_ok_id:

				// get setup

			skill=skill_medium;
			if (iface.singleplayer.skill) skill=element_get_value(singleplayer_option_skill_id);
			
			option_flags=0x0;
			for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
				if (element_get_value(singleplayer_option_option_start_id+n)!=0) option_flags|=(0x1<<n);
			}
			
				// picked map?
				
			map_name[0]=0x0;
			
			if (iface.singleplayer.map_pick) {
				idx=element_get_value(singleplayer_option_map_table_id);
				if (idx!=-1) singleplayer_option_map_list_get_name(idx,map_name);
			}

				// start game

			if (map_name[0]==0x0) {
				intro_start_game(skill,option_flags,NULL,intro_simple_save_idx);
			}
			else {
				intro_start_game(skill,option_flags,map_name,intro_simple_save_idx);
			}
			
			break;

		case singleplayer_option_button_cancel_id:
			server.next_state=gs_intro;
			break;

	}
	*/
}

void simple_save_pick_key(void)
{
		// check for esc
		
	if (!input_get_keyboard_escape()) {
		simple_save_pick_esc_down=FALSE;
		return;
	}
	
	if (simple_save_pick_esc_down) return;
	
	hud_click();
	
	simple_save_pick_esc_down=TRUE;

		// escape cancels

	server.next_state=gs_intro;
}

/* =======================================================

      Run Simple Save Pick Page
      
======================================================= */

void simple_save_pick_run(void)
{
	gui_draw(1.0f,TRUE);
	simple_save_pick_click();
	simple_save_pick_key();
}


