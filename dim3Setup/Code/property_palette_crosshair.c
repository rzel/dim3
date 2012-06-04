/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Crosshair

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

#define kCrosshairSettingsName					0
#define kCrosshairSettingsFileName				1

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Crosshair
      
======================================================= */

void property_palette_fill_crosshair(int crosshair_idx)
{
	iface_crosshair_type		*crosshair;

	crosshair=&iface.crosshair_list.crosshairs[crosshair_idx];

	list_palette_set_title(&property_palette,"Crosshairs",NULL,"Crosshair",crosshair->name,NULL,NULL);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_string(&property_palette,kCrosshairSettingsName,"Name",crosshair->name,name_str_len,FALSE);
	list_palette_add_picker_file(&property_palette,kCrosshairSettingsFileName,list_button_none,0,"Bitmap","Bitmaps/Crosshairs","png","",crosshair->bitmap_name,FALSE);
}

/* =======================================================

      Property Palette Click Crosshair
      
======================================================= */

void property_palette_click_crosshair(int crosshair_idx,bool double_click)
{
}
