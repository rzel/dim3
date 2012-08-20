/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Create Grid Dialog

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
	#include "dim3Editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

d3pnt							*dialog_grid_divs;
bool							*dialog_grid_sides;

// controls

#define diag_prop_grid_x			5000
#define diag_prop_grid_y			5001
#define diag_prop_grid_z			5002
#define diag_prop_side_neg_x		5003
#define diag_prop_side_pos_x		5004
#define diag_prop_side_neg_y		5005
#define diag_prop_side_pos_y		5006
#define diag_prop_side_neg_z		5007
#define diag_prop_side_pos_z		5008
#define diag_prop_grid_cancel		5009
#define diag_prop_grid_ok			5010

os_dialog_ctrl_type		diag_property_grid_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"X:",15,8,35,20},
							{os_dialog_ctrl_type_text_right,0,"Y:",15,33,35,20},
							{os_dialog_ctrl_type_text_right,0,"Z:",15,58,35,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_grid_x,"",55,5,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_grid_y,"",55,30,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_grid_z,"",55,55,100,20},
							{os_dialog_ctrl_type_text_checkbox,diag_prop_side_neg_x,"Neg X",170,5,50,20},
							{os_dialog_ctrl_type_text_checkbox,diag_prop_side_pos_x,"Pos X",220,5,50,20},
							{os_dialog_ctrl_type_text_checkbox,diag_prop_side_neg_y,"Neg Y",170,30,50,20},
							{os_dialog_ctrl_type_text_checkbox,diag_prop_side_pos_y,"Pos Y",220,30,50,20},
							{os_dialog_ctrl_type_text_checkbox,diag_prop_side_neg_z,"Neg Z",170,55,50,20},
							{os_dialog_ctrl_type_text_checkbox,diag_prop_side_pos_z,"Pos Z",220,55,50,20},
							{os_dialog_ctrl_type_button,diag_prop_grid_cancel,"Cancel",180,85,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_grid_ok,"Create",270,85,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Grid Dialog
      
======================================================= */

void dialog_property_grid_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_set_int(diag_prop_grid_x,10);
			os_dialog_set_int(diag_prop_grid_y,10);
			os_dialog_set_int(diag_prop_grid_z,10);

			os_dialog_set_bool(diag_prop_side_neg_x,TRUE);
			os_dialog_set_bool(diag_prop_side_pos_x,TRUE);
			os_dialog_set_bool(diag_prop_side_neg_y,TRUE);
			os_dialog_set_bool(diag_prop_side_pos_y,TRUE);
			os_dialog_set_bool(diag_prop_side_neg_z,TRUE);
			os_dialog_set_bool(diag_prop_side_pos_z,TRUE);

			os_dialog_set_focus(diag_prop_grid_x,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_grid_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_grid_ok) {
				dialog_grid_divs->x=os_dialog_get_int(diag_prop_grid_x);
				dialog_grid_divs->y=os_dialog_get_int(diag_prop_grid_y);
				dialog_grid_divs->z=os_dialog_get_int(diag_prop_grid_z);

				dialog_grid_sides[0]=os_dialog_get_bool(diag_prop_side_neg_x);
				dialog_grid_sides[1]=os_dialog_get_bool(diag_prop_side_pos_x);
				dialog_grid_sides[2]=os_dialog_get_bool(diag_prop_side_neg_y);
				dialog_grid_sides[3]=os_dialog_get_bool(diag_prop_side_pos_y);
				dialog_grid_sides[4]=os_dialog_get_bool(diag_prop_side_neg_z);
				dialog_grid_sides[5]=os_dialog_get_bool(diag_prop_side_pos_z);

				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_grid_run(d3pnt *divs,bool *sides)
{
	dialog_grid_divs=divs;
	dialog_grid_sides=sides;

	return(os_dialog_run("Create Grid",355,115,diag_property_grid_ctrls,dialog_property_grid_proc));
}

