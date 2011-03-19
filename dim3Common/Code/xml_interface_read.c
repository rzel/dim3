/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Interface XML Read

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
	#ifdef D3_ENGINE
		#include "dim3engine.h"
	#endif
	#ifdef D3_EDITOR
		#include "dim3Editor.h"
	#endif
	#ifdef D3_ANIMATOR
		#include "dim3Animator.h"
	#endif
	#ifdef D3_SETUP
		#include "dim3Setup.h"
	#endif
#endif

#include "interface.h"

extern iface_type				iface;
extern setup_type				setup;
extern network_setup_type		net_setup;

char							just_mode_str[][32]={"left","center","right"},
								text_special_str[][32]=text_special_list_def,
								control_names_str[][32]=control_names;

/* =======================================================

      Read Bitmap XML
      
======================================================= */

void read_settings_interface_bitmap(int bitmap_tag)
{
	int					tag;
	iface_bitmap_type	*bitmap;
    
		// read next bitmap

	if (iface.bitmap_list.nbitmap>max_iface_bitmap) return;
		
	bitmap=&iface.bitmap_list.bitmaps[iface.bitmap_list.nbitmap];
	iface.bitmap_list.nbitmap++;
	
		// read attributes
		
	xml_get_attribute_text(bitmap_tag,"name",bitmap->name,name_str_len);
	
	tag=xml_findfirstchild("Image",bitmap_tag);
	if (tag==-1) return;
	
	xml_get_attribute_text(tag,"file",bitmap->filename,file_str_len);
	bitmap->animate.image_count=xml_get_attribute_int_default(tag,"count",1);
	bitmap->animate.image_per_row=(int)sqrt((float)bitmap->animate.image_count);
	bitmap->animate.msec=xml_get_attribute_int(tag,"time");
    bitmap->animate.loop=xml_get_attribute_boolean(tag,"loop");
    bitmap->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
	bitmap->rot=xml_get_attribute_float_default(tag,"rot",0.0f);
	
	bitmap->show_tick=0;

	bitmap->x=bitmap->y=0;
	
	tag=xml_findfirstchild("Position",bitmap_tag);
	if (tag!=-1) {
		bitmap->x=xml_get_attribute_int(tag,"x");
		bitmap->y=xml_get_attribute_int(tag,"y");
	}
	
	bitmap->x_size=bitmap->y_size=-1;
	
	tag=xml_findfirstchild("Size",bitmap_tag);
	if (tag!=-1) {
		bitmap->x_size=xml_get_attribute_int(tag,"width");
		bitmap->y_size=xml_get_attribute_int(tag,"height");
	}
	
	bitmap->alpha=1.0f;
	bitmap->flip_horz=bitmap->flip_vert=FALSE;
	bitmap->team_tint=FALSE;
	bitmap->show=TRUE;
	
	tag=xml_findfirstchild("Settings",bitmap_tag);
	if (tag!=-1) {
		bitmap->alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		bitmap->flip_horz=xml_get_attribute_boolean(tag,"flip_horizontal");
		bitmap->flip_vert=xml_get_attribute_boolean(tag,"flip_vertical");
		bitmap->team_tint=xml_get_attribute_boolean(tag,"team_tint");
		bitmap->show=!xml_get_attribute_boolean(tag,"hide");
		bitmap->flash=xml_get_attribute_boolean(tag,"flash");
	}
	
	bitmap->repeat.on=FALSE;
	bitmap->repeat.count=0;
	bitmap->repeat.x_add=bitmap->repeat.y_add=0;
	bitmap->repeat.col=bitmap->repeat.row=0;
	
	tag=xml_findfirstchild("Repeat",bitmap_tag);
	if (tag!=-1) {
		bitmap->repeat.on=xml_get_attribute_boolean(tag,"on");
		bitmap->repeat.count=xml_get_attribute_int(tag,"count");
		bitmap->repeat.x_add=xml_get_attribute_int(tag,"x");
		bitmap->repeat.y_add=xml_get_attribute_int(tag,"y");
		bitmap->repeat.col=xml_get_attribute_int(tag,"col");
		bitmap->repeat.row=xml_get_attribute_int(tag,"row");
	}
	
	bitmap->fade.on=FALSE;
	bitmap->fade.fade_in_tick=bitmap->fade.life_tick=bitmap->fade.fade_out_tick=100;
	
	tag=xml_findfirstchild("Fade",bitmap_tag);
	if (tag!=-1) {
		bitmap->fade.on=xml_get_attribute_boolean(tag,"on");
		bitmap->fade.fade_in_tick=xml_get_attribute_int(tag,"fade_in_msec");
		bitmap->fade.life_tick=xml_get_attribute_int(tag,"life_msec");
		bitmap->fade.fade_out_tick=xml_get_attribute_int(tag,"fade_out_msec");
	}
}

/* =======================================================

      Read Text XML
      
======================================================= */

void read_settings_interface_text(int text_tag)
{
	int					tag;
	char				data[max_hud_text_str_sz];
	iface_text_type		*text;
	
		// read next text
	
	if (iface.text_list.ntext>=max_iface_text) return;
	
	text=&iface.text_list.texts[iface.text_list.ntext];
	iface.text_list.ntext++;
	
		// read attributes

	xml_get_attribute_text(text_tag,"name",text->name,name_str_len);
	
	text->x=text->y=0;
	
	tag=xml_findfirstchild("Position",text_tag);
	if (tag!=-1) {
		text->x=xml_get_attribute_int(tag,"x");
		text->y=xml_get_attribute_int(tag,"y");
	}
	
	text->size=iface.font.text_size_small;
	text->color.r=text->color.g=text->color.b=1;
	text->show=TRUE;
	text->special=text_special_none;
	text->just=tx_left;
	text->alpha=1.0f;

	data[0]=0x0;
	
	tag=xml_findfirstchild("Settings",text_tag);
	if (tag!=-1) {
		text->alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_text(tag,"data",data,max_hud_text_str_sz);
		text->size=xml_get_attribute_int_default(tag,"size",iface.font.text_size_small);
		xml_get_attribute_color(tag,"color",&text->color);
		text->just=xml_get_attribute_list(tag,"just",(char*)just_mode_str);
		text->show=!xml_get_attribute_boolean(tag,"hide");
		text->special=xml_get_attribute_list(tag,"special",(char*)text_special_str);
	}

	text->fade.on=FALSE;
	text->fade.fade_in_tick=text->fade.life_tick=text->fade.fade_out_tick=100;
	
	tag=xml_findfirstchild("Fade",text_tag);
	if (tag!=-1) {
		text->fade.on=xml_get_attribute_boolean(tag,"on");
		text->fade.fade_in_tick=xml_get_attribute_int(tag,"fade_in_msec");
		text->fade.life_tick=xml_get_attribute_int(tag,"life_msec");
		text->fade.fade_out_tick=xml_get_attribute_int(tag,"fade_out_msec");
	}

	iface_text_set(text,data);
}

/* =======================================================

      Read Bar XML
      
======================================================= */

void read_settings_interface_bar(int bar_tag)
{
	int					tag;
	iface_bar_type		*bar;
    
		// read next bar

	if (iface.bar_list.nbar>=max_iface_bar) return;
		
	bar=&iface.bar_list.bars[iface.bar_list.nbar];
	iface.bar_list.nbar++;
	
		// read attributes
		
	xml_get_attribute_text(bar_tag,"name",bar->name,name_str_len);
		
	bar->x=bar->y=0;
	
	tag=xml_findfirstchild("Position",bar_tag);
	if (tag!=-1) {
		bar->x=xml_get_attribute_int(tag,"x");
		bar->y=xml_get_attribute_int(tag,"y");
	}
	
	bar->x_size=bar->y_size=-1;
	
	tag=xml_findfirstchild("Size",bar_tag);
	if (tag!=-1) {
		bar->x_size=xml_get_attribute_int(tag,"width");
		bar->y_size=xml_get_attribute_int(tag,"height");
	}
	
	bar->outline=FALSE;
	bar->outline_alpha=1;
	bar->outline_color.r=bar->outline_color.g=bar->outline_color.b=0;
	
	tag=xml_findfirstchild("Outline",bar_tag);
	if (tag!=-1) {
		bar->outline=xml_get_attribute_boolean(tag,"on");
		bar->outline_alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_color(tag,"color",&bar->outline_color);
	}
	
	bar->fill_alpha=1;
	bar->fill_start_color.r=bar->fill_start_color.g=bar->fill_start_color.b=0;
	bar->fill_end_color.r=bar->fill_end_color.g=bar->fill_end_color.b=0;
	
	tag=xml_findfirstchild("Fill",bar_tag);
	if (tag!=-1) {
		bar->fill_alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_color(tag,"start_color",&bar->fill_start_color);
		xml_get_attribute_color(tag,"end_color",&bar->fill_end_color);
	}
	
	bar->vert=FALSE;
	bar->show=TRUE;
	
	tag=xml_findfirstchild("Settings",bar_tag);
	if (tag!=-1) {
		bar->vert=xml_get_attribute_boolean(tag,"vert");
		bar->show=!xml_get_attribute_boolean(tag,"hide");
	}
	
		// value starts at 1
		
	bar->value=1;
}

/* =======================================================

      Read Radar XML
      
======================================================= */

void read_settings_interface_radar(int radar_tag)
{
	int					tag,icons_head_tag,icon_tag;

		// defaults

	iface.radar.on=TRUE;
	iface.radar.rot=TRUE;
	iface.radar.team_tint=FALSE;
	iface.radar.no_motion_fade=0;
	
	iface.radar.x=iface.radar.y=0;
	iface.radar.display_radius=32;
	iface.radar.view_radius=map_enlarge*100;
	iface.radar.nicon=0;
	iface.radar.background_bitmap_name[0]=0x0;

		// read settings

	tag=xml_findfirstchild("Setting",radar_tag);
	if (tag!=-1) {
		iface.radar.on=xml_get_attribute_boolean(tag,"on");		// only use on switch if it exists
		iface.radar.rot=!xml_get_attribute_boolean(tag,"no_rot");
		iface.radar.team_tint=xml_get_attribute_boolean(tag,"team_tint");
		iface.radar.no_motion_fade=xml_get_attribute_int(tag,"no_motion_fade");
	}

	tag=xml_findfirstchild("Position",radar_tag);
	if (tag!=-1) {
		iface.radar.x=xml_get_attribute_int(tag,"x");
		iface.radar.y=xml_get_attribute_int(tag,"y");
		iface.radar.display_radius=xml_get_attribute_int(tag,"radius");
	}

	tag=xml_findfirstchild("View",radar_tag);
	if (tag!=-1) {
		iface.radar.view_radius=xml_get_attribute_int(tag,"radius");
	}

	tag=xml_findfirstchild("Background",radar_tag);
	if (tag!=-1) {
		xml_get_attribute_text(tag,"file",iface.radar.background_bitmap_name,name_str_len);
	}

		// get the icons

	icons_head_tag=xml_findfirstchild("Icons",radar_tag);
	if (icons_head_tag==-1) return;
	
	icon_tag=xml_findfirstchild("Icon",icons_head_tag);
	
	while (icon_tag!=-1) {
		xml_get_attribute_text(icon_tag,"name",iface.radar.icons[iface.radar.nicon].name,name_str_len);
		xml_get_attribute_text(icon_tag,"file",iface.radar.icons[iface.radar.nicon].bitmap_name,name_str_len);
		iface.radar.icons[iface.radar.nicon].size=xml_get_attribute_int(icon_tag,"size");
		iface.radar.icons[iface.radar.nicon].rot=xml_get_attribute_boolean(icon_tag,"rot");
		iface.radar.nicon++;

		icon_tag=xml_findnextchild(icon_tag);
	}
}

/* =======================================================

      Read Menu XML
      
======================================================= */

void read_settings_interface_menu(int menu_tag)
{
	int						items_head_tag,item_tag;
	iface_menu_type			*menu;
	iface_menu_item_type	*item;

		// read next menu

	menu=&iface.menu_list.menus[iface.menu_list.nmenu];
	iface.menu_list.nmenu++;

	xml_get_attribute_text(menu_tag,"name",menu->name,name_str_len);
	menu->nitem=0;

		// read menu

	items_head_tag=xml_findfirstchild("Items",menu_tag);
	if (items_head_tag==-1) return;
	
	item_tag=xml_findfirstchild("Item",items_head_tag);
	
	while (item_tag!=-1) {

		if (menu->nitem>=max_menu_item) return;
	
		item=&menu->items[menu->nitem];
		menu->nitem++;

		item->item_id=xml_get_attribute_int(item_tag,"id");
		xml_get_attribute_text(item_tag,"data",item->data,max_menu_item_data_sz);
		xml_get_attribute_text(item_tag,"sub_menu",item->sub_menu,name_str_len);
		item->multiplayer_disable=xml_get_attribute_boolean(item_tag,"multiplayer_disable");
		item->quit=xml_get_attribute_boolean(item_tag,"quit");
		
		item_tag=xml_findnextchild(item_tag);
	}
}

/* =======================================================

      Read Chooser XML
      
======================================================= */

void read_settings_interface_chooser_copy_template(iface_chooser_type *chooser,iface_chooser_type *template_chooser)
{
	char			name[name_str_len];
	
	strcpy(name,chooser->name);
	memmove(chooser,template_chooser,sizeof(iface_chooser_type));
	strcpy(chooser->name,name);
}

void read_settings_interface_chooser_template_override(iface_chooser_type *chooser,int template_idx)
{
	int							idx,template_piece_idx;

	if (template_idx==-1) return;

		// is the last added piece
		// actually an override?

	idx=chooser->npiece-1;
	
	template_piece_idx=iface_chooser_find_piece_idx(chooser,chooser->pieces[idx].id);
	if (template_piece_idx==-1) return;

		// if so, copy it over to the template
		// piece and then remove last piece

	switch (chooser->pieces[idx].type) {

		case chooser_piece_type_text:
			strcpy(chooser->pieces[template_piece_idx].data.text.str,chooser->pieces[idx].data.text.str);
			break;

		case chooser_piece_type_item:
			strcpy(chooser->pieces[template_piece_idx].data.item.file,chooser->pieces[idx].data.item.file);
			break;

		case chooser_piece_type_model:
			strcpy(chooser->pieces[template_piece_idx].data.model.model_name,chooser->pieces[idx].data.model.model_name);
			strcpy(chooser->pieces[template_piece_idx].data.model.animate_name,chooser->pieces[idx].data.model.animate_name);
			break;

		case chooser_piece_type_button:
			strcpy(chooser->pieces[template_piece_idx].data.button.name,chooser->pieces[idx].data.button.name);
			break;
	}

	chooser->npiece--;
}

// supergumba -- move all this and improve it
void read_settings_interface_chooser_add_text(iface_chooser_type *chooser,int template_idx,int id,char *str,int x,int y,int size,int just,bool clickable,char *goto_name)
{
	iface_chooser_piece_type	*piece;

	// add a new piece
	
	if (chooser->npiece>=max_chooser_piece) return;
	
	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;
	
	piece->type=chooser_piece_type_text;
	piece->id=id;
	strcpy(piece->data.text.str,str);
	piece->x=x;
	piece->y=y;
	piece->data.text.size=size;
	piece->data.text.just=just;

	piece->clickable=clickable;
	strcpy(piece->goto_name,goto_name);
}

void read_settings_interface_chooser_add_item(iface_chooser_type *chooser,int template_idx,int id,char *file,int x,int y,int wid,int high,bool clickable,char *goto_name)
{
	iface_chooser_piece_type	*piece;

		// add a new piece

	if (chooser->npiece>=max_chooser_piece) return;
	
	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;
	
	piece->type=chooser_piece_type_item;
	piece->id=id;
	strcpy(piece->data.item.file,file);
	piece->x=x;
	piece->y=y;
	piece->wid=wid;
	piece->high=high;

	piece->clickable=clickable;
	strcpy(piece->goto_name,goto_name);
}

void read_settings_interface_chooser_add_model(iface_chooser_type *chooser,int template_idx,int id,char *model_name,char *animate_name,int x,int y,d3ang *rot,float resize,bool clickable,char *goto_name)
{
	iface_chooser_piece_type	*piece;

		// add a new piece

	if (chooser->npiece>=max_chooser_piece) return;
	
	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;
	
	piece->type=chooser_piece_type_model;
	piece->id=id;
	strcpy(piece->data.model.model_name,model_name);
	strcpy(piece->data.model.animate_name,animate_name);

	memmove(&piece->data.model.rot,rot,sizeof(d3ang));
	piece->data.model.resize=resize;

	piece->x=x;
	piece->y=y;

	piece->clickable=clickable;
	strcpy(piece->goto_name,goto_name);
}

void read_settings_interface_chooser_add_button(iface_chooser_type *chooser,int template_idx,int id,char *name,int x,int y,int wid,int high,char *goto_name)
{
	iface_chooser_piece_type	*piece;

		// add a new piece

	if (chooser->npiece>=max_chooser_piece) return;
	
	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;
	
	piece->type=chooser_piece_type_button;
	piece->id=id;
	strcpy(piece->data.button.name,name);
	piece->x=x;
	piece->y=y;
	piece->wid=wid;
	piece->high=high;

	piece->clickable=TRUE;
	strcpy(piece->goto_name,goto_name);
}

void read_settings_interface_chooser(int chooser_tag)
{
	int					template_idx,just,tag,texts_head_tag,text_tag,
						items_head_tag,item_tag,models_head_tag,model_item_tag,
						buttons_head_tag,button_tag,
						x,y,wid,high,id,text_size;
	float				resize;
	char				file[file_str_len],
						model_name[name_str_len],animate_name[name_str_len],
						template_name[name_str_len],goto_name[name_str_len],
						btn_name[max_chooser_button_text_sz],
						data[max_chooser_text_data_sz];
	bool				clickable;
	d3ang				rot;
	iface_chooser_type	*chooser;

		// find next chooser

	if (iface.chooser_list.nchooser>=max_iface_chooser) return;
	
	chooser=&iface.chooser_list.choosers[iface.chooser_list.nchooser];
	iface.chooser_list.nchooser++;
	
	xml_get_attribute_text(chooser_tag,"name",chooser->name,name_str_len);

	chooser->npiece=0;
	chooser->frame.on=FALSE;
	chooser->key.ok_id=-1;
	chooser->key.cancel_id=-1;

		// run any templates
		// templates MUST be in order

	xml_get_attribute_text(chooser_tag,"template",template_name,name_str_len);

	template_idx=-1;

	if (template_name[0]!=0x0) {
		template_idx=iface_chooser_find_idx(template_name);
		if (template_idx!=-1) read_settings_interface_chooser_copy_template(chooser,&iface.chooser_list.choosers[template_idx]);
	}
	
		// frames and keys
		
	tag=xml_findfirstchild("Frame",chooser_tag);
	if (tag!=-1) {
		chooser->frame.on=xml_get_attribute_boolean(tag,"on");
		xml_get_attribute_text(tag,"title",chooser->frame.title,max_chooser_frame_text_sz);
		chooser->frame.x=xml_get_attribute_int(tag,"x");
		chooser->frame.y=xml_get_attribute_int(tag,"y");
		chooser->frame.wid=xml_get_attribute_int(tag,"width");
		chooser->frame.high=xml_get_attribute_int(tag,"height");
		xml_get_attribute_color(tag,"background_color",&chooser->frame.background_col);
	}
	
	tag=xml_findfirstchild("Key",chooser_tag);
	if (tag!=-1) {
		chooser->key.ok_id=xml_get_attribute_int(tag,"ok_id");
		chooser->key.cancel_id=xml_get_attribute_int(tag,"cancel_id");
	}
	
		// items
	
	items_head_tag=xml_findfirstchild("Items",chooser_tag);
	if (items_head_tag!=-1) {
	
		item_tag=xml_findfirstchild("Item",items_head_tag);
		
		while (item_tag!=-1) {
			id=xml_get_attribute_int(item_tag,"id");
			xml_get_attribute_text(item_tag,"file",file,file_str_len);
			x=xml_get_attribute_int(item_tag,"x");
			y=xml_get_attribute_int(item_tag,"y");
			wid=xml_get_attribute_int_default(item_tag,"width",-1);
			high=xml_get_attribute_int_default(item_tag,"height",-1);
			clickable=xml_get_attribute_boolean(item_tag,"clickable");
			xml_get_attribute_text(item_tag,"goto",goto_name,name_str_len);
			
			read_settings_interface_chooser_add_item(chooser,template_idx,id,file,x,y,wid,high,clickable,goto_name);
			read_settings_interface_chooser_template_override(chooser,template_idx);
			
			item_tag=xml_findnextchild(item_tag);
		}
	}
	
		// models
	
	models_head_tag=xml_findfirstchild("Models",chooser_tag);
	if (models_head_tag!=-1) {
	
		model_item_tag=xml_findfirstchild("Model",models_head_tag);
		
		while (model_item_tag!=-1) {
			id=xml_get_attribute_int(model_item_tag,"id");
			xml_get_attribute_text(model_item_tag,"model",model_name,name_str_len);
			xml_get_attribute_text(model_item_tag,"animate",animate_name,name_str_len);
			x=xml_get_attribute_int(model_item_tag,"x");
			y=xml_get_attribute_int(model_item_tag,"y");
			xml_get_attribute_3_coord_float(model_item_tag,"rot",&rot.x,&rot.y,&rot.z);
			resize=xml_get_attribute_float_default(model_item_tag,"resize",1.0f);

			clickable=xml_get_attribute_boolean(model_item_tag,"clickable");
			xml_get_attribute_text(model_item_tag,"goto",goto_name,name_str_len);
			
			read_settings_interface_chooser_add_model(chooser,template_idx,id,model_name,animate_name,x,y,&rot,resize,clickable,goto_name);
			read_settings_interface_chooser_template_override(chooser,template_idx);
			
			model_item_tag=xml_findnextchild(model_item_tag);
		}
	}

		// text

	texts_head_tag=xml_findfirstchild("Texts",chooser_tag);
	if (texts_head_tag!=-1) {
	
		text_tag=xml_findfirstchild("Text",texts_head_tag);
		
		while (text_tag!=-1) {

			data[0]=0x0;

			id=xml_get_attribute_int(text_tag,"id");
			xml_get_attribute_text(text_tag,"data",data,max_chooser_text_data_sz);
			text_size=xml_get_attribute_int_default(text_tag,"size",iface.font.text_size_small);
			just=xml_get_attribute_list(text_tag,"just",(char*)just_mode_str);
			x=xml_get_attribute_int(text_tag,"x");
			y=xml_get_attribute_int(text_tag,"y");
			clickable=xml_get_attribute_boolean(text_tag,"clickable");
			xml_get_attribute_text(text_tag,"goto",goto_name,name_str_len);

			read_settings_interface_chooser_add_text(chooser,template_idx,id,data,x,y,text_size,just,clickable,goto_name);
			read_settings_interface_chooser_template_override(chooser,template_idx);

			text_tag=xml_findnextchild(text_tag);
		}
	}
	
		// buttons
	
	buttons_head_tag=xml_findfirstchild("Buttons",chooser_tag);
	if (buttons_head_tag!=-1) {
	
		button_tag=xml_findfirstchild("Button",buttons_head_tag);
		
		while (button_tag!=-1) {
			id=xml_get_attribute_int(button_tag,"id");
			xml_get_attribute_text(button_tag,"name",btn_name,max_chooser_button_text_sz);
			x=xml_get_attribute_int(button_tag,"x");
			y=xml_get_attribute_int(button_tag,"y");
			wid=xml_get_attribute_int_default(button_tag,"width",-1);
			high=xml_get_attribute_int_default(button_tag,"height",-1);
			xml_get_attribute_text(button_tag,"goto",goto_name,name_str_len);
			
			read_settings_interface_chooser_add_button(chooser,template_idx,id,btn_name,x,y,wid,high,goto_name);
			read_settings_interface_chooser_template_override(chooser,template_idx);
			
			button_tag=xml_findnextchild(button_tag);
		}
	}
}

/* =======================================================

      Read Intro XML
      
======================================================= */

void read_settings_interface_intro_button(int tag,iface_intro_button_type *btn,iface_intro_simple_save_desc_type *desc)
{
	if (tag==-1) return;
	
	btn->x=xml_get_attribute_int(tag,"x");
	btn->y=xml_get_attribute_int(tag,"y");
	btn->wid=xml_get_attribute_int_default(tag,"width",-1);
	btn->high=xml_get_attribute_int_default(tag,"height",-1);
	btn->on=xml_get_attribute_boolean(tag,"on");

	if (desc!=NULL) {
		desc->x=xml_get_attribute_int(tag,"desc_x");
		desc->y=xml_get_attribute_int(tag,"desc_y");
		desc->text_size=xml_get_attribute_int_default(tag,"desc_text_size",20);
	}
}

void read_settings_interface_intro_model(int tag)
{
	iface_intro_model_type		*intro_model;

	if (iface.intro.model.nmodel==max_hud_intro_model) return;

	intro_model=&iface.intro.model.models[iface.intro.model.nmodel];
	iface.intro.model.nmodel++;

	xml_get_attribute_text(tag,"model",intro_model->model_name,name_str_len);
	xml_get_attribute_text(tag,"animate",intro_model->animate_name,name_str_len);
	intro_model->x=xml_get_attribute_int(tag,"x");
	intro_model->y=xml_get_attribute_int(tag,"y");
	xml_get_attribute_3_coord_float(tag,"rot",&intro_model->rot.x,&intro_model->rot.y,&intro_model->rot.z);
	intro_model->resize=xml_get_attribute_float_default(tag,"resize",1.0f);
}

/* =======================================================

      Read Interface XML
      
======================================================= */

void read_settings_interface(void)
{
	int						n,cnt,interface_head_tag,scale_tag,
							bitmap_head_tag,bitmap_tag,text_head_tag,text_tag,bar_head_tag,bar_tag,
							radar_head_tag,menu_head_tag,menu_tag,chooser_head_tag,chooser_tag,
							intro_head_tag,intro_model_head_tag,intro_model_tag,
							color_tag,font_tag,progress_tag,chat_tag,fade_tag,button_tag,sound_tag,music_tag,
							proj_tag,debug_tag,games_head_tag,game_tag,options_head_tag,option_tag,
							character_head_tag,character_item_tag,bot_head_tag,bot_tag,news_tag;
	char					path[1024],name[256];
	hud_character_item_type	*hud_character;

	default_settings_interface();
	
		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Interface","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    interface_head_tag=xml_findrootchild("Interface");
    if (interface_head_tag==-1) {
		xml_close_file();
		return;
	}

		// scale

	scale_tag=xml_findfirstchild("Scale",interface_head_tag);
	if (scale_tag!=-1) {
		iface.scale_x=xml_get_attribute_int_default(scale_tag,"x",640);
		iface.scale_y=xml_get_attribute_int_default(scale_tag,"y",480);
	}

		// some initial text sizes

	iface.font.text_size_small=(int)(((float)iface.scale_x)*text_small_wid_factor);
	iface.font.text_size_medium=(int)(((float)iface.scale_x)*text_medium_wid_factor);
	iface.font.text_size_large=(int)(((float)iface.scale_x)*text_large_wid_factor);
	
		// bitmaps
	
    bitmap_head_tag=xml_findfirstchild("Bitmaps",interface_head_tag);
    if (bitmap_head_tag!=-1) {
	
		bitmap_tag=xml_findfirstchild("Bitmap",bitmap_head_tag);
		
		while (bitmap_tag!=-1) {
			read_settings_interface_bitmap(bitmap_tag);
			bitmap_tag=xml_findnextchild(bitmap_tag);
		}
	}
	
		// text
	
	text_head_tag=xml_findfirstchild("Texts",interface_head_tag);
    if (text_head_tag!=-1) {
	
		text_tag=xml_findfirstchild("Text",text_head_tag);
		
		while (text_tag!=-1) {
			read_settings_interface_text(text_tag);
			text_tag=xml_findnextchild(text_tag);
		}
	}
	
		// bars
	
    bar_head_tag=xml_findfirstchild("Bars",interface_head_tag);
    if (bar_head_tag!=-1) {
	
		bar_tag=xml_findfirstchild("Bar",bar_head_tag);
		
		while (bar_tag!=-1) {
			read_settings_interface_bar(bar_tag);
			bar_tag=xml_findnextchild(bar_tag);
		}
	}

		// radar

	radar_head_tag=xml_findfirstchild("Radar",interface_head_tag);
	if (radar_head_tag!=-1) read_settings_interface_radar(radar_head_tag);
	
		// menus
		
	menu_head_tag=xml_findfirstchild("Menus",interface_head_tag);
    if (menu_head_tag!=-1) {
	
		menu_tag=xml_findfirstchild("Menu",menu_head_tag);
		
		while (menu_tag!=-1) {
			read_settings_interface_menu(menu_tag);
			menu_tag=xml_findnextchild(menu_tag);
		}
	}
	
		// choosers
		
	chooser_head_tag=xml_findfirstchild("Choosers",interface_head_tag);
    if (chooser_head_tag!=-1) {
	
		chooser_tag=xml_findfirstchild("Chooser",chooser_head_tag);
		
		while (chooser_tag!=-1) {
			read_settings_interface_chooser(chooser_tag);
			chooser_tag=xml_findnextchild(chooser_tag);
		}
	}
	
		// colors
		
	color_tag=xml_findfirstchild("Color",interface_head_tag);
	if (color_tag!=-1) {
		xml_get_attribute_color(color_tag,"dialog_background",&iface.color.dialog_background);
		xml_get_attribute_color(color_tag,"dialog_base",&iface.color.dialog_base);
		xml_get_attribute_color(color_tag,"dialog_dimmed",&iface.color.dialog_dimmed);
		xml_get_attribute_color(color_tag,"dialog_outline",&iface.color.dialog_outline);
		xml_get_attribute_color(color_tag,"control_label",&iface.color.control_label);
		xml_get_attribute_color(color_tag,"control_text",&iface.color.control_text);
		xml_get_attribute_color(color_tag,"control_fill",&iface.color.control_fill);
		xml_get_attribute_color(color_tag,"control_header",&iface.color.control_header);
		xml_get_attribute_color(color_tag,"control_outline",&iface.color.control_outline);
		xml_get_attribute_color(color_tag,"control_mouse_over",&iface.color.control_mouse_over);
		xml_get_attribute_color(color_tag,"control_hilite",&iface.color.control_hilite);
		xml_get_attribute_color(color_tag,"control_disabled",&iface.color.control_disabled);
		xml_get_attribute_color(color_tag,"button_fill",&iface.color.button_fill);
		xml_get_attribute_color(color_tag,"button_text",&iface.color.button_text);
		xml_get_attribute_color(color_tag,"button_outline",&iface.color.button_outline);
		xml_get_attribute_color(color_tag,"default_tint",&iface.color.default_tint);
	}

		// first tint is always the default tint

	memmove(&iface.color.tints[0],&iface.color.default_tint,sizeof(d3col));

		// fonts

	font_tag=xml_findfirstchild("Font",interface_head_tag);
	if (font_tag!=-1) {
		xml_get_attribute_text(font_tag,"name",iface.font.interface_name,name_str_len);
		xml_get_attribute_text(font_tag,"hud_name",iface.font.hud_name,name_str_len);
	}
	
		// progress

	progress_tag=xml_findfirstchild("Progress",interface_head_tag);
	if (progress_tag!=-1) {
		iface.progress.lx=xml_get_attribute_int(progress_tag,"left_x");
		iface.progress.rx=xml_get_attribute_int(progress_tag,"right_x");
		iface.progress.ty=xml_get_attribute_int(progress_tag,"top_y");
		iface.progress.by=xml_get_attribute_int(progress_tag,"bottom_y");
		iface.progress.text_size=xml_get_attribute_int(progress_tag,"text_size");
		iface.progress.outline=xml_get_attribute_boolean(progress_tag,"outline");
		xml_get_attribute_color(progress_tag,"base_color_start",&iface.progress.base_color_start);
		xml_get_attribute_color(progress_tag,"base_color_end",&iface.progress.base_color_end);
		xml_get_attribute_color(progress_tag,"hilite_color_start",&iface.progress.hilite_color_start);
		xml_get_attribute_color(progress_tag,"hilite_color_end",&iface.progress.hilite_color_end);
		xml_get_attribute_color(progress_tag,"text_color",&iface.progress.text_color);
		xml_get_attribute_color(progress_tag,"outline_color",&iface.progress.outline_color);
	}
	
		// chat

	chat_tag=xml_findfirstchild("Chat",interface_head_tag);
	if (chat_tag!=-1) {
		iface.chat.x=xml_get_attribute_int(chat_tag,"x");
		iface.chat.y=xml_get_attribute_int(chat_tag,"y");
		iface.chat.last_add_life_sec=xml_get_attribute_int(chat_tag,"last_add_life_sec");
		iface.chat.next_life_sec=xml_get_attribute_int(chat_tag,"next_life_sec");
	}
	
		// fade

	fade_tag=xml_findfirstchild("Fade",interface_head_tag);
	if (fade_tag!=-1) {
		iface.fade.title_msec=xml_get_attribute_int(fade_tag,"title_msec");
		iface.fade.map_msec=xml_get_attribute_int(fade_tag,"map_msec");
	}

		// intro

	intro_head_tag=xml_findfirstchild("Intro",interface_head_tag);
	if (intro_head_tag!=-1) {

			// models

		intro_model_head_tag=xml_findfirstchild("Models",intro_head_tag);
		if (intro_model_head_tag!=-1) {
			intro_model_tag=xml_findfirstchild("Model",intro_model_head_tag);
		
			while (intro_model_tag!=-1) {
				read_settings_interface_intro_model(intro_model_tag);
				intro_model_tag=xml_findnextchild(intro_model_tag);
			}
		}

			// buttons
			
		button_tag=xml_findfirstchild("Buttons",intro_head_tag);
		if (button_tag!=-1) {
			read_settings_interface_intro_button(xml_findfirstchild("Game_New",button_tag),&iface.intro.button_game_new,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Game_Load",button_tag),&iface.intro.button_game_load,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Game_Setup",button_tag),&iface.intro.button_game_setup,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Game_New_Easy",button_tag),&iface.intro.button_game_new_easy,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Game_New_Medium",button_tag),&iface.intro.button_game_new_medium,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Game_New_Hard",button_tag),&iface.intro.button_game_new_hard,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Game_New_Cancel",button_tag),&iface.intro.button_game_new_cancel,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Multiplayer_Host",button_tag),&iface.intro.button_multiplayer_host,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Multiplayer_Join",button_tag),&iface.intro.button_multiplayer_join,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Multiplayer_Setup",button_tag),&iface.intro.button_multiplayer_setup,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Credit",button_tag),&iface.intro.button_credit,NULL);
			read_settings_interface_intro_button(xml_findfirstchild("Quit",button_tag),&iface.intro.button_quit,NULL);
			
			for (n=0;n!=max_simple_save_spot;n++) {
				sprintf(name,"Simple_Start_%d",n);
				read_settings_interface_intro_button(xml_findfirstchild(name,button_tag),&iface.intro.simple_save[n].button_start,&iface.intro.simple_save[n].desc);
				sprintf(name,"Simple_Erase_%d",n);
				read_settings_interface_intro_button(xml_findfirstchild(name,button_tag),&iface.intro.simple_save[n].button_erase,NULL);
			}
		}
	}
	
		// sound
		
	sound_tag=xml_findfirstchild("Sound",interface_head_tag);
	if (sound_tag!=-1) {
		xml_get_attribute_text(sound_tag,"click",iface.click_sound,name_str_len);
	}
	
		// music
		
	music_tag=xml_findfirstchild("Music",interface_head_tag);
	if (music_tag!=-1) {
		xml_get_attribute_text(music_tag,"intro",iface.intro.music,name_str_len);
	}

		// project setup
		
	proj_tag=xml_findfirstchild("Project",interface_head_tag);
	if (proj_tag!=-1) {
		xml_get_attribute_text(proj_tag,"name",iface.project_name,name_str_len);
		iface.skill=xml_get_attribute_boolean(proj_tag,"skill");
	}
	
		// debug setup

	debug_tag=xml_findfirstchild("Debug",interface_head_tag);
	if (debug_tag!=-1) {
		iface.debug=xml_get_attribute_boolean(debug_tag,"on");
	}

		// network games

	games_head_tag=xml_findfirstchild("Games",interface_head_tag);
	if (games_head_tag!=-1) {
		
		iface.net_game.ngame=0;
		
		game_tag=xml_findfirstchild("Game",games_head_tag);
		while (game_tag!=-1) {
		
			xml_get_attribute_text(game_tag,"type",iface.net_game.games[iface.net_game.ngame].name,name_str_len);
			iface.net_game.games[iface.net_game.ngame].use_teams=xml_get_attribute_boolean(game_tag,"use_teams");
			iface.net_game.games[iface.net_game.ngame].monsters=xml_get_attribute_boolean(game_tag,"monsters");
			
			iface.net_game.ngame++;
			if (iface.net_game.ngame==max_net_game) break;

			game_tag=xml_findnextchild(game_tag);
		}
	}

		// network options

	options_head_tag=xml_findfirstchild("Options",interface_head_tag);
	if (options_head_tag!=-1) {
		
		iface.net_option.noption=0;
		
		option_tag=xml_findfirstchild("Option",options_head_tag);
		while (option_tag!=-1) {
		
			xml_get_attribute_text(option_tag,"name",iface.net_option.options[iface.net_option.noption].name,name_str_len);
			xml_get_attribute_text(option_tag,"description",iface.net_option.options[iface.net_option.noption].descript,64);
			
			iface.net_option.noption++;
			if (iface.net_option.noption==max_net_option) break;

			option_tag=xml_findnextchild(option_tag);
		}
	}

		// characters

    character_head_tag=xml_findfirstchild("Characters",interface_head_tag);
    if (character_head_tag!=-1) {

		iface.character.ncharacter=0;
		character_item_tag=xml_findfirstchild("Character",character_head_tag);
		
		while (character_item_tag!=-1) {
			hud_character=&iface.character.characters[iface.character.ncharacter];
			
			xml_get_attribute_text(character_item_tag,"name",hud_character->name,name_str_len);
			xml_get_attribute_text(character_item_tag,"model",hud_character->model_name,name_str_len);
			xml_get_attribute_text(character_item_tag,"parameter",hud_character->param,name_str_len);
			hud_character->interface_resize=xml_get_attribute_float_default(character_item_tag,"interface_resize",1.0f);
			xml_get_attribute_3_coord_int(character_item_tag,"interface_offset",&hud_character->interface_offset.x,&hud_character->interface_offset.y,&hud_character->interface_offset.z);
			
			iface.character.ncharacter++;
			character_item_tag=xml_findnextchild(character_item_tag);
		}
	}
	
		// bot names
	
    bot_head_tag=xml_findfirstchild("Bots",interface_head_tag);
    if (bot_head_tag!=-1) {

		iface.net_bot.on=!xml_get_attribute_boolean(bot_head_tag,"hide");
	
		cnt=0;
		bot_tag=xml_findfirstchild("Bot",bot_head_tag);
		
		while (bot_tag!=-1) {
			xml_get_attribute_text(bot_tag,"name",iface.net_bot.bots[cnt].name,name_str_len);
			cnt++;
			bot_tag=xml_findnextchild(bot_tag);
		}
	}
	
		// news
		
    news_tag=xml_findfirstchild("News",interface_head_tag);
    if (news_tag!=-1) {
		xml_get_attribute_text(news_tag,"host",iface.net_news.host,64);
		iface.net_news.port=xml_get_attribute_int_default(news_tag,"port",80);
		xml_get_attribute_text(news_tag,"url",iface.net_news.url,256);
	}

	xml_close_file();
}

/* =======================================================

      Read Interface XML
	  For Project Name, so we can put it in the
	  File Path so project name can override the application's
	  name (therefore we can have multiple data files
	  per same application)
      
======================================================= */

void read_settings_interface_project_name(void)
{
	int						interface_head_tag,proj_tag;
	char					path[1024];
	
		// default project name is blank
		// (will use application name)
		
	iface.project_name[0]=0x0;

		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Interface","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    interface_head_tag=xml_findrootchild("Interface");
    if (interface_head_tag==-1) {
		xml_close_file();
		return;
	}

		// project setup
		
	proj_tag=xml_findfirstchild("Project",interface_head_tag);
	if (proj_tag!=-1) {
		xml_get_attribute_text(proj_tag,"name",setup.file_path_setup.proj_name,256);
	}
	

	xml_close_file();
}

/* =======================================================

      Read Particle XML
      
======================================================= */

void read_settings_particle(void)
{
	int					n,nparticle,
						particle_data_head_tag,particle_head_tag,particle_tag,
						particle_group_head_tag,particle_group_tag,tag,
						ring_radius,ring_size;
	float				ring_min_move,ring_max_move;
	bool				ring_flip;
	char				path[1024];
	iface_particle_type	*particle;

		// read in particles from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Particles","xml");
	if (!xml_open_file(path)) return;
	
		// can accept file with a particle_data tag or not
		
	particle_data_head_tag=xml_findrootchild("Particle_Data");
	
		// particle and group head tags
       
	if (particle_data_head_tag==-1) {
		particle_head_tag=xml_findrootchild("Particles");
		particle_group_head_tag=xml_findrootchild("Particle_Groups");
	}
	else {
		particle_head_tag=xml_findfirstchild("Particles",particle_data_head_tag);
		particle_group_head_tag=xml_findfirstchild("Particle_Groups",particle_data_head_tag);
	}
	
    if (particle_head_tag==-1) {
		xml_close_file();
		return;
	}

		// get counts

	nparticle=xml_countchildren(particle_head_tag);
	if (particle_group_head_tag!=-1) nparticle+=xml_countchildren(particle_group_head_tag);

	if (nparticle==0) {
		xml_close_file();
		return;
	}

		// load regular particles

	particle_tag=xml_findfirstchild("Particle",particle_head_tag);

	while (particle_tag!=-1) {
	
			// create a new particle
			
		if (iface.particle_list.nparticle>=max_iface_particle) {
			xml_close_file();
			return;
		}
			
		particle=&iface.particle_list.particles[iface.particle_list.nparticle];
		iface.particle_list.nparticle++;
		
			// read in particle

		xml_get_attribute_text(particle_tag,"name",particle->name,name_str_len);
		
		particle->group.on=FALSE;

		particle->count=1;
		particle->life_msec=1000;
		particle->reverse=FALSE;
		particle->ambient_factor=1.0f;
		particle->blend_add=FALSE;
		particle->globe=FALSE;

        tag=xml_findfirstchild("Settings",particle_tag);
        if (tag!=-1) {
            particle->count=xml_get_attribute_int(tag,"count");
            particle->life_msec=xml_get_attribute_int(tag,"time");
            particle->reverse=xml_get_attribute_boolean(tag,"reverse");
            particle->ambient_factor=xml_get_attribute_float_default(tag,"ambient",1.0f);
            particle->blend_add=xml_get_attribute_boolean(tag,"additive");
            particle->globe=xml_get_attribute_boolean(tag,"globe");
		}

		if (particle->count>max_particle_count) particle->count=max_particle_count;
		
		particle->animate.image_count=1;
		particle->animate.image_per_row=1;
		particle->animate.msec=200;
		particle->animate.loop=TRUE;
		particle->animate.loop_back=FALSE;
		
        tag=xml_findfirstchild("Image",particle_tag);
        if (tag!=-1) {
            xml_get_attribute_text(tag,"file",particle->bitmap_name,file_str_len);
            particle->animate.image_count=xml_get_attribute_int(tag,"count");
			particle->animate.image_per_row=(int)sqrt((float)particle->animate.image_count);
			particle->animate.msec=xml_get_attribute_int(tag,"time");
            particle->animate.loop=xml_get_attribute_boolean(tag,"loop");
            particle->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
		
        tag=xml_findfirstchild("Trail",particle_tag);
        if (tag!=-1) {
            particle->trail_count=xml_get_attribute_int(tag,"count");
            particle->trail_step=xml_get_attribute_float(tag,"step");
            particle->reduce_pixel_fact=xml_get_attribute_float(tag,"reduce");
        }
		
		if (particle->trail_step>max_particle_trail) particle->trail_step=max_particle_trail;
		
        tag=xml_findfirstchild("Gravity",particle_tag);
        if (tag!=-1) {
            particle->start_gravity=xml_get_attribute_float(tag,"start");
            particle->gravity_add=xml_get_attribute_float(tag,"add");
        }
		
        tag=xml_findfirstchild("Size",particle_tag);
        if (tag!=-1) {
            particle->start_pixel_size=xml_get_attribute_int(tag,"start");
            particle->end_pixel_size=xml_get_attribute_int(tag,"end");
        }

			// old way of defining particles

        tag=xml_findfirstchild("Ring",particle_tag);
        if (tag!=-1) {
            ring_radius=xml_get_attribute_int(tag,"radius");
            ring_size=xml_get_attribute_int(tag,"size");
            ring_min_move=xml_get_attribute_float(tag,"min_move");
            ring_max_move=xml_get_attribute_float(tag,"max_move");
			ring_flip=xml_get_attribute_boolean(tag,"flip");

			particle->rot.x=particle->rot.y=particle->rot.z=0.0f;
			particle->rot_accel.x=particle->rot_accel.y=particle->rot_accel.z=0.0f;
		
			if (ring_flip) {
				particle->pt.x=ring_radius+ring_size;
				particle->pt.z=ring_radius+ring_size;
				particle->pt.y=0;
		
				particle->vct.x=ring_max_move;
				particle->vct.z=ring_max_move;
				particle->vct.y=0;
			}
			else {
				particle->pt.x=ring_radius+ring_size;
				particle->pt.y=ring_radius+ring_size;
				particle->pt.z=ring_radius+ring_size;
		
				particle->vct.x=ring_max_move;
				particle->vct.y=ring_max_move;
				particle->vct.z=ring_max_move;
			}
		}

			// new way of defining particles

		else {

			tag=xml_findfirstchild("X",particle_tag);
			if (tag!=-1) {
				particle->pt.x=xml_get_attribute_int(tag,"offset");
				particle->vct.x=xml_get_attribute_float(tag,"move");
				particle->rot.x=xml_get_attribute_float_default(tag,"rot",0.0f);
				particle->rot_accel.x=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
			}

			tag=xml_findfirstchild("Y",particle_tag);
			if (tag!=-1) {
				particle->pt.y=xml_get_attribute_int(tag,"offset");
				particle->vct.y=xml_get_attribute_float(tag,"move");
				particle->rot.y=xml_get_attribute_float_default(tag,"rot",0.0f);
				particle->rot_accel.y=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
			}

			tag=xml_findfirstchild("Z",particle_tag);
			if (tag!=-1) {
				particle->pt.z=xml_get_attribute_int(tag,"offset");
				particle->vct.z=xml_get_attribute_float(tag,"move");
				particle->rot.z=xml_get_attribute_float_default(tag,"rot",0.0f);
				particle->rot_accel.z=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
			}

		}

		particle->start_color.r=particle->start_color.g=particle->start_color.b=1.0f;
		particle->end_color.r=particle->end_color.g=particle->end_color.b=1.0f;
		particle->team_tint=FALSE;

        tag=xml_findfirstchild("Color",particle_tag);
        if (tag!=-1) {
			xml_get_attribute_color(tag,"start",&particle->start_color);
			xml_get_attribute_color(tag,"end",&particle->end_color);
            particle->team_tint=xml_get_attribute_boolean(tag,"team");
        }
		
        tag=xml_findfirstchild("Alpha",particle_tag);
        if (tag!=-1) {
            particle->start_alpha=xml_get_attribute_float(tag,"start");
            particle->end_alpha=xml_get_attribute_float(tag,"end");
        }
	
			// move on to next particle
		
		particle_tag=xml_findnextchild(particle_tag);
	}

		// load group particles

    if (particle_group_head_tag==-1) {
		xml_close_file();
		return;
	}

  	particle_group_tag=xml_findfirstchild("Particle_Group",particle_group_head_tag);
   
	while (particle_group_tag!=-1) {
	
			// create a new particle group
			
		if (iface.particle_list.nparticle>=max_iface_particle) {
			xml_close_file();
			return;
		}
			
		particle=&iface.particle_list.particles[iface.particle_list.nparticle];
		iface.particle_list.nparticle++;
		
			// read in particle group
		
		xml_get_attribute_text(particle_group_tag,"name",particle->name,name_str_len);
		
		particle->group.on=TRUE;
		particle->group.count=0;

		particle->count=0;

        particle_head_tag=xml_findfirstchild("Particles",particle_group_tag);
        if (particle_head_tag!=-1) {

			particle->group.count=xml_countchildren(particle_head_tag);
			if (particle->group.count>max_particle_group) particle->group.count=max_particle_group;

			particle_tag=xml_findfirstchild("Particle",particle_head_tag);
    
			for (n=0;n!=particle->group.count;n++) {
				xml_get_attribute_text(particle_tag,"name",particle->group.particles[n].name,name_str_len);
				particle->group.particles[n].shift=xml_get_attribute_int_default(particle_tag,"offset",0);
				particle_tag=xml_findnextchild(particle_tag);
			}
		}

			// move on to next particle group
		
		particle_group_tag=xml_findnextchild(particle_group_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Read Ring XML
      
======================================================= */

void read_settings_ring(void)
{
	int					nring,ring_head_tag,ring_tag,tag;
	char				path[1024];
	iface_ring_type		*ring;

		// read in rings from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Rings","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
       
    ring_head_tag=xml_findrootchild("Rings");
    if (ring_head_tag==-1) {
		xml_close_file();
		return;
	}

	nring=xml_countchildren(ring_head_tag);

	if (nring==0) {
		xml_close_file();
		return;
	}

		// read in the rings

	ring_tag=xml_findfirstchild("Ring",ring_head_tag);
    
	while (ring_tag!=-1) {
	
			// create a new ring

		if (iface.ring_list.nring>=max_iface_ring) {
			xml_close_file();
			return;
		}
			
		ring=&iface.ring_list.rings[iface.ring_list.nring];
		iface.ring_list.nring++;
		
			// read in ring
		
		xml_get_attribute_text(ring_tag,"name",ring->name,name_str_len);
		
		ring->life_msec=1000;
		ring->blend_add=FALSE;

        tag=xml_findfirstchild("Settings",ring_tag);
        if (tag!=-1) {
            ring->life_msec=xml_get_attribute_int(tag,"time");
            ring->blend_add=xml_get_attribute_boolean(tag,"additive");
        }
		
		ring->animate.image_count=1;
		ring->animate.image_per_row=1;
		ring->animate.msec=200;
		ring->animate.loop=TRUE;
		ring->animate.loop_back=FALSE;
		
        tag=xml_findfirstchild("Image",ring_tag);
        if (tag!=-1) {
            xml_get_attribute_text(tag,"file",ring->bitmap_name,file_str_len);
            ring->animate.image_count=xml_get_attribute_int(tag,"count");
			ring->animate.image_per_row=(int)sqrt((float)ring->animate.image_count);
			ring->animate.msec=xml_get_attribute_int(tag,"time");
            ring->animate.loop=xml_get_attribute_boolean(tag,"loop");
            ring->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
		
        tag=xml_findfirstchild("Outer",ring_tag);
        if (tag!=-1) {
            ring->start_outer_size=xml_get_attribute_int(tag,"start");
            ring->end_outer_size=xml_get_attribute_int(tag,"end");
        }

        tag=xml_findfirstchild("Inner",ring_tag);
        if (tag!=-1) {
            ring->start_inner_size=xml_get_attribute_int(tag,"start");
            ring->end_inner_size=xml_get_attribute_int(tag,"end");
        }

        tag=xml_findfirstchild("Rotate",ring_tag);
        if (tag!=-1) {
            ring->ang.x=xml_get_attribute_float(tag,"x");
            ring->ang.y=xml_get_attribute_float(tag,"y");
            ring->ang.z=xml_get_attribute_float(tag,"z");
            ring->rot.x=xml_get_attribute_float_default(tag,"x_add",0.0f);
            ring->rot.y=xml_get_attribute_float_default(tag,"y_add",0.0f);
            ring->rot.z=xml_get_attribute_float_default(tag,"z_add",0.0f);
            ring->rot_accel.x=xml_get_attribute_float_default(tag,"x_accel",0.0f);
            ring->rot_accel.y=xml_get_attribute_float_default(tag,"y_accel",0.0f);
            ring->rot_accel.z=xml_get_attribute_float_default(tag,"z_accel",0.0f);
		}

        tag=xml_findfirstchild("Move",ring_tag);
        if (tag!=-1) {
            ring->vct.x=xml_get_attribute_float(tag,"x");
            ring->vct.y=xml_get_attribute_float(tag,"y");
            ring->vct.z=xml_get_attribute_float(tag,"z");
        }
		
		ring->start_color.r=ring->start_color.g=ring->start_color.b=1.0f;
		ring->end_color.r=ring->end_color.g=ring->end_color.b=1.0f;
		ring->team_tint=FALSE;

        tag=xml_findfirstchild("Color",ring_tag);
        if (tag!=-1) {
			xml_get_attribute_color(tag,"start",&ring->start_color);
			xml_get_attribute_color(tag,"end",&ring->end_color);
			ring->team_tint=xml_get_attribute_boolean(tag,"team");
       }
		
        tag=xml_findfirstchild("Alpha",ring_tag);
        if (tag!=-1) {
            ring->start_alpha=xml_get_attribute_float(tag,"start");
            ring->end_alpha=xml_get_attribute_float(tag,"end");
        }
	
			// move on to next ring
			
		ring_tag=xml_findnextchild(ring_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Read Action XML
      
======================================================= */

void read_settings_action(void)
{
	int					n,actions_head_tag,action_tag,idx;
	char				name[name_str_len],path[1024];
						
		// default actions
		
	for (n=0;n!=ncontrol;n++) {
		strcpy(iface.action_display_list.action_displays[n].display_name,control_names_str[n]);
		iface.action_display_list.action_displays[n].show=FALSE;
	}
	
		// read in actions from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Actions","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    actions_head_tag=xml_findrootchild("Actions");
    if (actions_head_tag!=-1) {
	
		action_tag=xml_findfirstchild("Action",actions_head_tag);
		
		while (action_tag!=-1) {
			xml_get_attribute_text(action_tag,"name",name,name_str_len);
			
			idx=setup_find_control_for_name(name);
			if (idx!=-1) {
				xml_get_attribute_text(action_tag,"display",iface.action_display_list.action_displays[idx].display_name,name_str_len);
				iface.action_display_list.action_displays[idx].show=xml_get_attribute_boolean(action_tag,"show");
			}
			
			action_tag=xml_findnextchild(action_tag);
		}
	}
	
	xml_close_file();
}

/* =======================================================

      Read Sound XML
      
======================================================= */

void read_settings_sound(void)
{
	int					sounds_head_tag,sound_tag,tag;
	char				path[1024];
	iface_sound_type	*snd;

	iface.sound_list.nsound=0;
	
		// read in sound from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Sounds","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    sounds_head_tag=xml_findrootchild("Sounds");
    if (sounds_head_tag!=-1) {
	
		sound_tag=xml_findfirstchild("Sound",sounds_head_tag);
		
		while (sound_tag!=-1) {
			snd=&iface.sound_list.sounds[iface.sound_list.nsound];

			xml_get_attribute_text(sound_tag,"name",snd->name,name_str_len);
			
			tag=xml_findfirstchild("Wave",sound_tag);
			if (tag!=-1) xml_get_attribute_text(tag,"file",snd->file_name,file_str_len);
			
			tag=xml_findfirstchild("Distance",sound_tag);
			if (tag!=-1) {
				snd->min_dist=xml_get_attribute_int(tag,"min");
				snd->max_dist=xml_get_attribute_int(tag,"max");
			}

			iface.sound_list.nsound++;
			if (iface.sound_list.nsound==max_iface_sound) break;

			sound_tag=xml_findnextchild(sound_tag);
		}
	}
	
	xml_close_file();
}

/* =======================================================

      Read Shader XML
      
======================================================= */

void read_settings_shader(void)
{
	int						nshader,shaders_head_tag,shader_tag,tag;
	char					path[1024];
	iface_shader_type		*iface_shader;

		// no marks yet

	iface.shader_list.nshader=0;
	
		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Shaders","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    shaders_head_tag=xml_findrootchild("Shaders");
    if (shaders_head_tag==-1) {
		xml_close_file();
		return;
	}

	nshader=xml_countchildren(shaders_head_tag);

	if (nshader==0) {
		xml_close_file();
		return;
	}

		// read the shaders
	
	shader_tag=xml_findfirstchild("Shader",shaders_head_tag);
	
	while (shader_tag!=-1) {
	
		iface_shader=&iface.shader_list.shaders[iface.shader_list.nshader];
		
		xml_get_attribute_text(shader_tag,"name",iface_shader->name,name_str_len);
		
			// shader files
			
		tag=xml_findfirstchild("Code",shader_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"vert",iface_shader->vert_name,file_str_len);
			xml_get_attribute_text(tag,"frag",iface_shader->frag_name,file_str_len);
		}
		
			// move on to next shader
			
		iface.shader_list.nshader++;
		
		shader_tag=xml_findnextchild(shader_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Read Marks XML
      
======================================================= */

void read_settings_mark(void)
{
	int					nmark,marks_head_tag,mark_tag,tag;
	char				path[1024];
	iface_mark_type		*mark;

		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Marks","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    marks_head_tag=xml_findrootchild("Marks");
    if (marks_head_tag==-1) {
		xml_close_file();
		return;
	}

	nmark=xml_countchildren(marks_head_tag);

	if (nmark==0) {
		xml_close_file();
		return;
	}

		// read the marks
	
	mark_tag=xml_findfirstchild("Mark",marks_head_tag);
	
	while (mark_tag!=-1) {
	
			// create a new mark
			
		if (iface.mark_list.nmark>=max_iface_mark) {
			xml_close_file();
			return;
		}
		
		mark=&iface.mark_list.marks[iface.mark_list.nmark];
		iface.mark_list.nmark++;
			
			// read settings
		
		xml_get_attribute_text(mark_tag,"name",mark->name,name_str_len);
		
		tag=xml_findfirstchild("Setting",mark_tag);
		if (tag!=-1) {
			mark->life_msec=xml_get_attribute_int(tag,"time");
			mark->fade_in_msec=xml_get_attribute_int(tag,"fade_in");
			mark->fade_out_msec=xml_get_attribute_int(tag,"fade_out");
			mark->no_rotate=xml_get_attribute_boolean(tag,"no_rotate");
			mark->no_transparent=xml_get_attribute_boolean(tag,"no_transparent");
			mark->no_opaque=xml_get_attribute_boolean(tag,"no_opaque");
            mark->hilite=xml_get_attribute_boolean(tag,"highlight");
            mark->blend_add=xml_get_attribute_boolean(tag,"additive");
		}
		else {
			mark->life_msec=1000;
			mark->fade_in_msec=mark->fade_out_msec=500;
			mark->no_rotate=FALSE;
			mark->no_transparent=FALSE;
			mark->no_opaque=FALSE;
			mark->hilite=FALSE;
			mark->blend_add=FALSE;
		}
		
		mark->total_msec=mark->life_msec+mark->fade_in_msec+mark->fade_out_msec;
		
		mark->animate.image_count=1;
		mark->animate.image_per_row=1;
		mark->animate.msec=200;
		mark->animate.loop=TRUE;
		mark->animate.loop_back=FALSE;
	
		tag=xml_findfirstchild("Image",mark_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"file",mark->bitmap_name,file_str_len);
			mark->animate.image_count=xml_get_attribute_int(tag,"count");
			mark->animate.image_per_row=(int)sqrt((float)mark->animate.image_count);
			mark->animate.msec=xml_get_attribute_int(tag,"time");
			mark->animate.loop=xml_get_attribute_boolean(tag,"loop");
			mark->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
	
		mark->color.r=mark->color.g=mark->color.b=1.0f;
		mark->team_tint=FALSE;

        tag=xml_findfirstchild("Color",mark_tag);
        if (tag!=-1) {
			xml_get_attribute_color(tag,"color",&mark->color);
            mark->team_tint=xml_get_attribute_boolean(tag,"team");
        }

			// move on to next mark
			
		mark_tag=xml_findnextchild(mark_tag);
	}
	
	xml_close_file();
}
