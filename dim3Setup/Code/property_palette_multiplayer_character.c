/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Multiplayer Character

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

#define kMPCharacterPropertySettingsName		0
#define kMPCharacterPropertySettingsModelName	1
#define kMPCharacterPropertySettingsParam		2

#define kMPCharacterPropertyOptionsResize		3
#define kMPCharacterPropertyOptionsOffset		4

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Multiplayer Character
      
======================================================= */

void property_palette_fill_multiplayer_character(int multiplayer_character_idx)
{
	iface_mp_character_type		*mp_character;

	mp_character=&iface.multiplayer.character_list.characters[multiplayer_character_idx];

	list_palette_set_title(&property_palette,"Multiplayer",NULL,"Character",mp_character->name,NULL,NULL);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_string(&property_palette,kMPCharacterPropertySettingsName,"Name",mp_character->name,name_str_len,FALSE);
	list_palette_add_picker_file(&property_palette,kMPCharacterPropertySettingsModelName,list_button_none,0,"Model","Models","","Mesh.xml;Model.xml",mp_character->model_name,FALSE);
	list_palette_add_string(&property_palette,kMPCharacterPropertySettingsParam,"Parameters",mp_character->param,name_str_len,FALSE);

		// options

	list_palette_add_header(&property_palette,0,"UI Drawing");
	list_palette_add_float(&property_palette,kMPCharacterPropertyOptionsResize,"Resize",&mp_character->interface_resize,FALSE);
	list_palette_add_point(&property_palette,kMPCharacterPropertyOptionsOffset,"Offset",&mp_character->interface_offset,FALSE);
}

/* =======================================================

      Property Palette Click Multiplayer Character
      
======================================================= */

void property_palette_click_multiplayer_character(int multiplayer_character_idx,bool double_click)
{
}
