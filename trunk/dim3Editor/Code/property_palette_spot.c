/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Spot

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kSpotPropertyName						0
#define kSpotPropertyType						1
#define kSpotPropertyScript						2
#define kSpotPropertyScriptEdit					3
#define kSpotPropertySkill						4
#define kSpotPropertySpawn						5
#define kSpotPropertyDisplayModel				6

#define kSpotPropertyParamsStart				100
#define kSpotPropertyParamsEnd					109

extern map_type					map;

extern list_palette_type		property_palette;

int								pal_spot_index;
char							spot_property_type_list[][name_str_len]={"Object","Bot","Player","Spawn",""},
								spot_property_skill_list[][name_str_len]={"Easy","Medium","Hard",""},
								spot_property_spawn_list[][name_str_len]={"Always","Single Player Only","Multiplayer Only",""};

/* =======================================================

      Property Palette Fill Spot
      
======================================================= */

void property_palette_fill_spot(int spot_idx)
{
	int				n;
	char			name[256];
	spot_type		*spot;

	spot=&map.spots[spot_idx];

	list_palette_set_title(&property_palette,"Spot",spot->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&property_palette,0,"Spot Settings");
	list_palette_add_string(&property_palette,kSpotPropertyName,"Name",spot->name,name_str_len,FALSE);
	list_palette_add_picker_list_int(&property_palette,kSpotPropertyType,"Type",(char*)spot_property_type_list,-1,name_str_len,0,FALSE,&spot->type,FALSE);

	if (spot->script[0]!=0x0) {
		list_palette_add_picker_file(&property_palette,kSpotPropertyScript,list_button_edit,kSpotPropertyScriptEdit,"Script","Scripts/Objects","js","",spot->script,FALSE);
	}
	else {
		list_palette_add_picker_file(&property_palette,kSpotPropertyScript,list_button_none,0,"Script","Scripts/Objects","js","",spot->script,FALSE);
	}

	list_palette_add_picker_list_int(&property_palette,kSpotPropertySkill,"Skill",(char*)spot_property_skill_list,-1,name_str_len,0,FALSE,&spot->skill,FALSE);
	list_palette_add_picker_list_int(&property_palette,kSpotPropertySpawn,"Spawn",(char*)spot_property_spawn_list,-1,name_str_len,0,FALSE,&spot->spawn,FALSE);
	list_palette_add_picker_file(&property_palette,kSpotPropertyDisplayModel,list_button_none,0,"Model","Models","","Mesh.xml;Model.xml",spot->display_model,FALSE);

	list_palette_add_header(&property_palette,0,"Spot Parameters");
	for (n=0;n!=10;n++) {
		sprintf(name,"Param %d",n);
		list_palette_add_parameter(&property_palette,(kSpotPropertyParamsStart+n),name,spot->params,n,FALSE);
	}
	
	pal_spot_index=spot_idx;
	
	list_palette_add_header(&property_palette,0,"Spot Info");
	list_palette_add_int(&property_palette,-1,"Index",&pal_spot_index,TRUE);
	list_palette_add_point(&property_palette,-1,"Position",&spot->pnt,TRUE);
	list_palette_add_angle(&property_palette,-1,"Angle",&spot->ang,TRUE);
}

/* =======================================================

      Property Palette Click Spot
      
======================================================= */

void property_palette_click_spot(int spot_idx,bool double_click)
{
	int				id;
	spot_type		*spot;

	spot=&map.spots[spot_idx];

	id=property_palette.item_pane.click.id;

		// script editing

	if (id==kSpotPropertyScriptEdit) {
		launch_spot_script_editor(spot);
		return;
	}
}

