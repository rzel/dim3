/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette HUD Text

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kTextSettingsName						0
#define kTextSettingsData						1
#define kTextSettingsShow						2

#define kTextPositionX							3
#define kTextPositionY							4

#define kTextOptionSize							5
#define kTextOptionJust							6
#define kTextOptionSpecial						7
#define kTextOptionAlpha						8
#define kTextOptionColor						9

#define kTextFadeOn								10
#define kTextFadeInTick							11
#define kTextFadeLifeTick						12
#define kTextFadeOutTick						13

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

char							hud_text_just_type_str[][32]=text_just_list_def,
								hud_text_special_type_str[][32]=text_special_list_def;

/* =======================================================

      Property Palette Fill HUD Text
      
======================================================= */

void property_palette_fill_hud_text(int hud_text_idx)
{
	iface_text_type				*text;

	text=&iface.text_list.texts[hud_text_idx];

	list_palette_set_title(&property_palette,"HUD",NULL,"Text",text->name,NULL,NULL);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_string(&property_palette,kTextSettingsName,"Name",text->name,name_str_len,FALSE);
	list_palette_add_string(&property_palette,kTextSettingsData,"Data",text->data,max_hud_text_str_sz,FALSE);
	list_palette_add_checkbox(&property_palette,kTextSettingsShow,"Show",&text->show,FALSE);

		// position

	list_palette_add_header(&property_palette,0,"Position");
	list_palette_add_int(&property_palette,kTextPositionX,"X",&text->pnt.x,FALSE);
	list_palette_add_int(&property_palette,kTextPositionY,"Y",&text->pnt.y,FALSE);

		// options

	list_palette_add_header(&property_palette,0,"Options");
	list_palette_add_int(&property_palette,kTextOptionSize,"Text Size",&text->size,FALSE);
	list_palette_add_picker_list_int(&property_palette,kTextOptionJust,"Justification",(char*)hud_text_just_type_str,-1,name_str_len,0,FALSE,&text->just,FALSE);
	list_palette_add_picker_list_int(&property_palette,kTextOptionSpecial,"Special",(char*)hud_text_special_type_str,-1,name_str_len,0,FALSE,&text->special,FALSE);
	list_palette_add_float(&property_palette,kTextOptionAlpha,"Alpha",&text->alpha,FALSE);
	list_palette_add_pick_color(&property_palette,kTextOptionColor,"Color",&text->color,FALSE);

		// fade

	list_palette_add_header(&property_palette,0,"Fade");
	list_palette_add_checkbox(&property_palette,kTextFadeOn,"On",&text->fade.on,FALSE);
	list_palette_add_int(&property_palette,kTextFadeInTick,"Fade In Milliseconds",&text->fade.fade_in_tick,FALSE);
	list_palette_add_int(&property_palette,kTextFadeLifeTick,"Life Milliseconds",&text->fade.life_tick,FALSE);
	list_palette_add_int(&property_palette,kTextFadeOutTick,"Fade Out Milliseconds",&text->fade.fade_out_tick,FALSE);
}

/* =======================================================

      Property Palette Click HUD Text
      
======================================================= */

void property_palette_click_hud_text(int hud_text_idx,bool double_click)
{
}

