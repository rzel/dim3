/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Auto-Generate Setting Dialogs

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
#include "import.h"

#define kMapGenerateTabCount						4

#define kMapGenerateTab								FOUR_CHAR_CODE('tabb')

#define kMapGenerateSeed							FOUR_CHAR_CODE('seed')
#define kMapGenerateBlock							FOUR_CHAR_CODE('blck')
#define kMapGenerateMapSize							FOUR_CHAR_CODE('mpsz')
#define kMapGeneratePortalSize						FOUR_CHAR_CODE('ptsz')
#define kMapGeneratePortalHigh						FOUR_CHAR_CODE('pthg')

#define kMapGenerateFloorType						FOUR_CHAR_CODE('flrt')
#define kMapGenerateCeilingType						FOUR_CHAR_CODE('clgt')
#define kMapGenerateCorridorType					FOUR_CHAR_CODE('cort')
#define kMapGenerateDoorType						FOUR_CHAR_CODE('dort')

#define kMapGenerateStepSize						FOUR_CHAR_CODE('stsz')
#define kMapGenerateStepHigh						FOUR_CHAR_CODE('sthg')
#define kMapGenerateRampSize						FOUR_CHAR_CODE('rmsz')

#define kMapGenerateTexturePortalWall				FOUR_CHAR_CODE('pwtx')
#define kMapGenerateTexturePortalFloor				FOUR_CHAR_CODE('pftx')
#define kMapGenerateTexturePortalCeiling			FOUR_CHAR_CODE('pctx')
#define kMapGenerateTextureCorridorWall				FOUR_CHAR_CODE('cwtx')
#define kMapGenerateTextureCorridorFloor			FOUR_CHAR_CODE('cftx')
#define kMapGenerateTextureCorridorCeiling			FOUR_CHAR_CODE('cctx')
#define kMapGenerateTextureSecondStory				FOUR_CHAR_CODE('sstx')
#define kMapGenerateTextureDoor						FOUR_CHAR_CODE('drtx')
#define kMapGenerateTextureStep						FOUR_CHAR_CODE('sttx')
#define kMapGenerateTextureRamp						FOUR_CHAR_CODE('rmtx')

#define kMapGenerateIncludeDoors					FOUR_CHAR_CODE('idor')
#define kMapGenerateDoorPercentage					FOUR_CHAR_CODE('dprc')
#define kMapGenerateDoorSound						FOUR_CHAR_CODE('dsnd')

#define kMapGenerateIncludeLights					FOUR_CHAR_CODE('ilgt')
#define kMapGenerateLightFudgeFactor				FOUR_CHAR_CODE('lfdg')
#define kMapGenerateLightFillPercentage				FOUR_CHAR_CODE('lfpr')
#define kMapGenerateLightColorPercentage			FOUR_CHAR_CODE('lcol')
#define kMapGenerateLightFlickerPercentage			FOUR_CHAR_CODE('lflk')

#define kMapGenerateIncludeSpots					FOUR_CHAR_CODE('ispt')
#define kMapGenerateSpotCount						FOUR_CHAR_CODE('cspt')

extern auto_generate_settings_type			ag_settings;

bool						dialog_map_generate_cancel;
WindowRef					dialog_map_generate_wind;

/* =======================================================

      Map Generate Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_generate_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand					cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
					
				case kHICommandCancel:
					dialog_map_generate_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_generate_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_generate_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus map_generate_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	dialog_switch_tab(dialog_map_generate_wind,kMapGenerateTab,0,kMapGenerateTabCount);
	return(noErr);
}

/* =======================================================

      Run Map Auto-Generate Setting
      
======================================================= */

bool dialog_map_auto_generate_setting_run(bool first)
{
	int						n;
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_map_generate_wind,"MapGenerate");
	
		// tab
		
	dialog_set_tab(dialog_map_generate_wind,kMapGenerateTab,0,0,kMapGenerateTabCount);
	
	ctrl_id.signature=kMapGenerateTab;
	ctrl_id.id=0;
	GetControlByID(dialog_map_generate_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(map_generate_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_map_generate_wind,NULL);
	
		// set random seed
		
	dialog_set_int(dialog_map_generate_wind,kMapGenerateSeed,0,(int)time(NULL));
	
		// old settings
		
	if (!first) {
		dialog_set_value(dialog_map_generate_wind,kMapGenerateMapSize,0,ag_settings.map.map_sz);
		dialog_set_combo(dialog_map_generate_wind,kMapGenerateBlock,0,ag_settings.block);
		dialog_set_value(dialog_map_generate_wind,kMapGeneratePortalSize,0,ag_settings.map.portal_sz);
		
		for (n=0;n!=ag_floor_type_count;n++) {
			dialog_set_boolean(dialog_map_generate_wind,kMapGenerateFloorType,n,ag_settings.floor_type_on[n]);
		}
		
		for (n=0;n!=ag_ceiling_type_count;n++) {
			dialog_set_boolean(dialog_map_generate_wind,kMapGenerateCeilingType,n,ag_settings.ceiling_type_on[n]);
		}
		
		for (n=0;n!=ag_corridor_type_count;n++) {
			dialog_set_boolean(dialog_map_generate_wind,kMapGenerateCorridorType,n,ag_settings.corridor_type_on[n]);
		}
		
		for (n=0;n!=ag_door_type_count;n++) {
			dialog_set_boolean(dialog_map_generate_wind,kMapGenerateDoorType,n,ag_settings.door_type_on[n]);
		}
	
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE,ag_settings.texture.portal_wall);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE,ag_settings.texture.portal_floor);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling,0,FALSE,ag_settings.texture.portal_ceiling);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWall,0,FALSE,ag_settings.texture.corridor_wall);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE,ag_settings.texture.corridor_floor);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorCeiling,0,FALSE,ag_settings.texture.corridor_ceiling);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureSecondStory,0,FALSE,ag_settings.texture.second_story);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE,ag_settings.texture.door);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE,ag_settings.texture.steps);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRamp,0,FALSE,ag_settings.texture.ramp);
		
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeDoors,0,ag_settings.doors);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateDoorPercentage,0,ag_settings.door_percentage);
		dialog_set_text(dialog_map_generate_wind,kMapGenerateDoorSound,0,ag_settings.door_sound);
		
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeLights,0,ag_settings.lights);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightFudgeFactor,0,ag_settings.light_fudge_factor);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightFillPercentage,0,ag_settings.light_fill_percentage);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightColorPercentage,0,ag_settings.light_color_percentage);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightFlickerPercentage,0,ag_settings.light_flicker_percentage);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeSpots,0,ag_settings.spots);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateSpotCount,0,ag_settings.spot_count);
	}
	else {
	
			// always need to set textures
			
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE,0);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE,1);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling,0,FALSE,2);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWall,0,FALSE,3);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE,4);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorCeiling,0,FALSE,5);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureSecondStory,0,FALSE,6);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE,7);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE,8);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRamp,0,FALSE,9);
	}
	
		// show window
	
	ShowWindow(dialog_map_generate_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_generate_setting_event_proc);
	InstallWindowEventHandler(dialog_map_generate_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_generate_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_generate_wind);
	
		// dialog to data
		
	ag_settings.seed=dialog_get_int(dialog_map_generate_wind,kMapGenerateSeed,0);
	ag_settings.block=dialog_get_combo(dialog_map_generate_wind,kMapGenerateBlock,0);

	ag_settings.map.map_sz=dialog_get_value(dialog_map_generate_wind,kMapGenerateMapSize,0);
	ag_settings.map.portal_sz=dialog_get_value(dialog_map_generate_wind,kMapGeneratePortalSize,0);

	for (n=0;n!=ag_floor_type_count;n++) {
		ag_settings.floor_type_on[n]=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateFloorType,n);
	}
	
	for (n=0;n!=ag_ceiling_type_count;n++) {
		ag_settings.ceiling_type_on[n]=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateCeilingType,n);
	}
	
	for (n=0;n!=ag_corridor_type_count;n++) {
		ag_settings.corridor_type_on[n]=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateCorridorType,n);
	}
	
	for (n=0;n!=ag_door_type_count;n++) {
		ag_settings.door_type_on[n]=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateDoorType,n);
	}
	
	ag_settings.texture.portal_wall=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE);
	ag_settings.texture.portal_floor=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE);
	ag_settings.texture.portal_ceiling=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling,0,FALSE);
	ag_settings.texture.corridor_wall=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWall,0,FALSE);
	ag_settings.texture.corridor_floor=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE);
	ag_settings.texture.corridor_ceiling=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorCeiling,0,FALSE);
	ag_settings.texture.second_story=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureSecondStory,0,FALSE);
	ag_settings.texture.door=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE);
	ag_settings.texture.steps=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE);
	ag_settings.texture.ramp=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRamp,0,FALSE);
	
	ag_settings.doors=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeDoors,0);
	ag_settings.door_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateDoorPercentage,0);
	dialog_get_text(dialog_map_generate_wind,kMapGenerateDoorSound,0,ag_settings.door_sound,name_str_len);

	ag_settings.lights=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeLights,0);
	ag_settings.light_fudge_factor=dialog_get_value(dialog_map_generate_wind,kMapGenerateLightFudgeFactor,0);
	ag_settings.light_fill_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateLightFillPercentage,0);
	ag_settings.light_color_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateLightColorPercentage,0);
	ag_settings.light_flicker_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateLightFlickerPercentage,0);
	ag_settings.spots=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeSpots,0);
	ag_settings.spot_count=dialog_get_value(dialog_map_generate_wind,kMapGenerateSpotCount,0);
	
		// close window
		
	DisposeWindow(dialog_map_generate_wind);
	
	return(!dialog_map_generate_cancel);
}

