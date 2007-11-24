/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.health object

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
#include "objects.h"

extern js_type			js;

JSBool js_get_obj_health_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_health_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_health_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_health_remove_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_health_reset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_health_class={"obj_health_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_health_property,js_set_obj_health_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_health_props[]={
							{"maximum",					obj_health_prop_maximum,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"start",					obj_health_prop_start,						JSPROP_PERMANENT|JSPROP_SHARED},
							{"current",					obj_health_prop_current,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"recoverTick",				obj_health_prop_recover_tick,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"recoverAmount",			obj_health_prop_recover_amount,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"fallDamageMinimumHeight",	obj_health_prop_fall_damage_minimum_height,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"fallDamageFactor",		obj_health_prop_fall_damage_factor,			JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	obj_health_functions[]={
							{"add",					js_obj_health_add_func,				1},
							{"remove",				js_obj_health_remove_func,			1},
							{"reset",				js_obj_health_reset_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_health_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"health",&obj_health_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_health_props);
	JS_DefineFunctions(js.cx,j_obj,obj_health_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_health_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_health_prop_maximum:
			*vp=INT_TO_JSVAL(obj->status.max_health);
			break;
		case obj_health_prop_start:
			*vp=INT_TO_JSVAL(obj->status.start_health);
			break;
		case obj_health_prop_current:
			*vp=INT_TO_JSVAL(obj->status.health);
			break;
		case obj_health_prop_recover_tick:
			*vp=INT_TO_JSVAL(obj->status.health_recover_tick);
			break;
		case obj_health_prop_recover_amount:
			*vp=INT_TO_JSVAL(obj->status.health_recover_amount);
			break;
		case obj_health_prop_fall_damage_minimum_height:
			*vp=INT_TO_JSVAL(obj->fall.damage_minimum_height);
			break;
		case obj_health_prop_fall_damage_factor:
			*vp=script_float_to_value(obj->fall.damage_factor);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_health_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_health_prop_maximum:
			obj->status.max_health=JSVAL_TO_INT(*vp);
			break;
		case obj_health_prop_start:
			obj->status.start_health=JSVAL_TO_INT(*vp);
			break;
		case obj_health_prop_recover_tick:
			obj->status.health_recover_tick=JSVAL_TO_INT(*vp);
			obj->status.health_recover_count=0;			// restart recover account
			break;
		case obj_health_prop_recover_amount:
			obj->status.health_recover_amount=JSVAL_TO_INT(*vp);
			break;
		case obj_health_prop_fall_damage_minimum_height:
			obj->fall.damage_minimum_height=JSVAL_TO_INT(*vp);
			break;
		case obj_health_prop_fall_damage_factor:
			obj->fall.damage_factor=script_value_to_float(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Health Functions
      
======================================================= */

JSBool js_obj_health_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	obj_status		*status;
	
	obj=object_find_uid(js.attach.thing_uid);
    status=&obj->status;
	
    status->health+=JSVAL_TO_INT(argv[0]);
    if (status->health>status->max_health) status->health=status->max_health;

	return(JS_TRUE);
}

JSBool js_obj_health_remove_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	obj_status		*status;
	
	obj=object_find_uid(js.attach.thing_uid);
    status=&obj->status;
	
	obj->damage_obj_uid=-1;			// self-inflicted damage
	
    status->health-=JSVAL_TO_INT(argv[0]);
    if (status->health<0) {
		status->health=0;
		obj->death_trigger=TRUE;
	}

	return(JS_TRUE);
}

JSBool js_obj_health_reset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	obj_status		*status;
	
	obj=object_find_uid(js.attach.thing_uid);
    status=&obj->status;
	
	status->health=status->max_health;

	return(JS_TRUE);
}


