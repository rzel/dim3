/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.kickback object

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
#include "weapons.h"

extern js_type			js;

JSValueRef js_weap_kickback_get_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_kickback_set_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_kickback_kick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_kickback_props[]={
							{"size",				js_weap_kickback_get_size,				js_weap_kickback_set_size,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	weap_kickback_functions[]={
							{"kick",				js_weap_kickback_kick_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			weap_kickback_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_kickback_object(void)
{
	weap_kickback_class=script_create_class("weap_kickback_class",weap_kickback_props,weap_kickback_functions);
}

void script_free_weap_kickback_object(void)
{
	script_free_class(weap_kickback_class);
}

JSObjectRef script_add_weap_kickback_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,weap_kickback_class,"kickback"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_kickback_get_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,weap->kickback.size));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_kickback_set_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->kickback.size=script_value_to_int(cx,vp);

	return(TRUE);
}

/* =======================================================

      Kickback Functions
      
======================================================= */

JSValueRef js_weap_kickback_kick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
	weapon_kickback(js.time.current_tick,obj,weap);

	return(script_null_to_value(cx));
}

