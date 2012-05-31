/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette HUD Bitmap

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

#define kBitmapSettingsName						0
#define kBitmapSettingsFileName					1
#define kBitmapSettingsShow						2

#define kBitmapPositionX						3
#define kBitmapPositionY						4
#define kBitmapPositionWid						5
#define kBitmapPositionHigh						6

#define kBitmapOptionAlpha						7
#define kBitmapOptionRotate						8
#define kBitmapOptionFlash						9
#define kBitmapOptionFlipHorz					10
#define kBitmapOptionFlipVert					11
#define kBitmapOptionTeamTint					12

#define kBitmapRepeatOn							13
#define kBitmapRepeatXAdd						14
#define kBitmapRepeatYAdd						15
#define kBitmapRepeatColumn						16
#define kBitmapRepeatRow						17
#define kBitmapRepeatCount						18

#define kBitmapFadeOn							19
#define kBitmapFadeInTick						20
#define kBitmapFadeLifeTick						21
#define kBitmapFadeOutTick						22

#define kBitmapImageCount						23
#define kBitmapImageMillisecond					24
#define kBitmapImageLoop						25
#define kBitmapImageLoopBack					26

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill HUD Bitmap
      
======================================================= */

void property_palette_fill_hud_bitmap(int hud_bitmap_idx)
{
	iface_bitmap_type			*bitmap;

	bitmap=&iface.bitmap_list.bitmaps[hud_bitmap_idx];

	list_palette_set_title(&property_palette,"HUD",NULL,"Bitmap",bitmap->name,NULL,NULL);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_string(&property_palette,kBitmapSettingsName,"Name",bitmap->name,name_str_len,FALSE);
	list_palette_add_picker_file(&property_palette,kBitmapSettingsFileName,list_button_none,0,"Bitmap","Bitmaps/Interface","png","",bitmap->filename,FALSE);
	list_palette_add_checkbox(&property_palette,kBitmapSettingsShow,"Show",&bitmap->show,FALSE);

		// position

	list_palette_add_header(&property_palette,0,"Position");
	list_palette_add_int(&property_palette,kBitmapPositionX,"X",&bitmap->pnt.x,FALSE);
	list_palette_add_int(&property_palette,kBitmapPositionY,"Y",&bitmap->pnt.y,FALSE);
	list_palette_add_int(&property_palette,kBitmapPositionWid,"Width",&bitmap->size.x,FALSE);
	list_palette_add_int(&property_palette,kBitmapPositionHigh,"Height",&bitmap->size.y,FALSE);

		// options

	list_palette_add_header(&property_palette,0,"Options");
	list_palette_add_float(&property_palette,kBitmapOptionAlpha,"Alpha",&bitmap->alpha,FALSE);
	list_palette_add_float(&property_palette,kBitmapOptionRotate,"Rotate",&bitmap->rot,FALSE);
	list_palette_add_checkbox(&property_palette,kBitmapOptionFlash,"Flash",&bitmap->flash,FALSE);
	list_palette_add_checkbox(&property_palette,kBitmapOptionFlipHorz,"Flip Horizontal",&bitmap->flip_horz,FALSE);
	list_palette_add_checkbox(&property_palette,kBitmapOptionFlipVert,"Flip Vertical",&bitmap->flip_vert,FALSE);
	list_palette_add_checkbox(&property_palette,kBitmapOptionTeamTint,"Team Tint",&bitmap->team_tint,FALSE);

		// repeat

	list_palette_add_header(&property_palette,0,"Repeat");
	list_palette_add_checkbox(&property_palette,kBitmapRepeatOn,"On",&bitmap->repeat.on,FALSE);
	list_palette_add_int(&property_palette,kBitmapRepeatXAdd,"X Add",&bitmap->repeat.x_add,FALSE);
	list_palette_add_int(&property_palette,kBitmapRepeatYAdd,"Y Add",&bitmap->repeat.y_add,FALSE);
	list_palette_add_int(&property_palette,kBitmapRepeatColumn,"Column Count",&bitmap->repeat.col,FALSE);
	list_palette_add_int(&property_palette,kBitmapRepeatRow,"Row Count",&bitmap->repeat.row,FALSE);
	list_palette_add_int(&property_palette,kBitmapRepeatCount,"Count",&bitmap->repeat.count,FALSE);

		// fade

	list_palette_add_header(&property_palette,0,"Fade");
	list_palette_add_checkbox(&property_palette,kBitmapFadeOn,"On",&bitmap->fade.on,FALSE);
	list_palette_add_int(&property_palette,kBitmapFadeInTick,"Fade In Milliseconds",&bitmap->fade.fade_in_tick,FALSE);
	list_palette_add_int(&property_palette,kBitmapFadeLifeTick,"Life Milliseconds",&bitmap->fade.life_tick,FALSE);
	list_palette_add_int(&property_palette,kBitmapFadeOutTick,"Fade Out Milliseconds",&bitmap->fade.fade_out_tick,FALSE);

		// animation

	list_palette_add_header(&property_palette,0,"Animation");
	list_palette_add_int(&property_palette,kBitmapImageCount,"Count",&bitmap->animate.image_count,FALSE);
	list_palette_add_int(&property_palette,kBitmapImageMillisecond,"Display Milliseconds",&bitmap->animate.msec,FALSE);
	list_palette_add_checkbox(&property_palette,kBitmapImageLoop,"Loop",&bitmap->animate.loop,FALSE);
	list_palette_add_checkbox(&property_palette,kBitmapImageLoopBack,"Loop Back",&bitmap->animate.loop_back,FALSE);
}

/* =======================================================

      Property Palette Click HUD Bitmap
      
======================================================= */

void property_palette_click_hud_bitmap(int hud_bitmap_idx,bool double_click)
{
}
