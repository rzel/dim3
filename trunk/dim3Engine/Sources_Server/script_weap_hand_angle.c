/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.handAngle object

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "weapons.h"

extern js_type			js;

JSBool js_get_weap_hand_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_hand_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			weap_hand_angle_class={"weap_hand_angle_class",0,
							script_add_property,JS_PropertyStub,
							js_get_weap_hand_angle_property,js_set_weap_hand_angle_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	weap_hand_angle_props[]={
							{"x",				weap_hand_angle_prop_x,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"y",				weap_hand_angle_prop_y,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"z",				weap_hand_angle_prop_z,					JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_hand_angle_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"handAngle",&weap_hand_angle_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,weap_hand_angle_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_hand_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_hand_angle_prop_x:
            *vp=script_float_to_value(weap->hand.ang.x);
			break;
		case weap_hand_angle_prop_y:
            *vp=script_float_to_value(weap->hand.ang.y);
			break;
		case weap_hand_angle_prop_z:
            *vp=script_float_to_value(weap->hand.ang.z);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_hand_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_hand_angle_prop_x:
			weap->hand.ang.x=script_value_to_float(*vp);
			break;
		case weap_hand_angle_prop_y:
			weap->hand.ang.y=script_value_to_float(*vp);
			break;
		case weap_hand_angle_prop_z:
			weap->hand.ang.z=script_value_to_float(*vp);
			break;

	}
	
	return(JS_TRUE);
}



