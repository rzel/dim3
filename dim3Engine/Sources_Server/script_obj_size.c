/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.size object

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

JSBool js_obj_size_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_obj_size_get_x(jsval *vp);
bool js_obj_size_get_y(jsval *vp);
bool js_obj_size_get_z(jsval *vp);
bool js_obj_size_get_eyeOffset(jsval *vp);
bool js_obj_size_get_weight(jsval *vp);
bool js_obj_size_get_clickDistance(jsval *vp);
bool js_obj_size_get_cameraZAdjust(jsval *vp);
bool js_obj_size_set_x(jsval *vp);
bool js_obj_size_set_y(jsval *vp);
bool js_obj_size_set_z(jsval *vp);
bool js_obj_size_set_eyeOffset(jsval *vp);
bool js_obj_size_set_weight(jsval *vp);
bool js_obj_size_set_clickDistance(jsval *vp);
bool js_obj_size_set_cameraZAdjust(jsval *vp);
JSBool js_obj_size_grow_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_size_grow_over_time_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_size_grow_over_time_change_size_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_size_grow_over_time_change_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	obj_size_props[]={
							{"x",					js_obj_size_get_x,					js_obj_size_set_x},
							{"y",					js_obj_size_get_y,					js_obj_size_set_y},
							{"z",					js_obj_size_get_z,					js_obj_size_set_z},
							{"eyeOffset",			js_obj_size_get_eyeOffset,			js_obj_size_set_eyeOffset},
							{"weight",				js_obj_size_get_weight,				js_obj_size_set_weight},
							{"clickDistance",		js_obj_size_get_clickDistance,		js_obj_size_set_clickDistance},
							{"cameraZAdjust",		js_obj_size_get_cameraZAdjust,		js_obj_size_set_cameraZAdjust},
							{0}};

script_js_function	obj_size_functions[]={
							{"growTo",						js_obj_size_grow_to_func,						1},
							{"growOverTime",				js_obj_size_grow_over_time_func,				2},
							{"growOverTimeChangeSize",		js_obj_size_grow_over_time_change_size_func,	5},
							{"growOverTimeChangeOffset",	js_obj_size_grow_over_time_change_offset_func,	5},
							{0}};

JSClassRef			obj_size_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_size_object(void)
{
	obj_size_class=script_create_class("obj_size_class",js_obj_size_get_property,js_obj_size_set_property);
}

void script_free_obj_size_object(void)
{
	script_free_class(obj_size_class);
}

JSObject* script_add_obj_size_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_size_class,"size",obj_size_props,obj_size_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_size_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_size_props));
}

JSBool js_obj_size_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_size_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_size_get_x(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->size.x);
	
	return(JS_TRUE);
}

bool js_obj_size_get_y(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->size.y);
	
	return(JS_TRUE);
}

bool js_obj_size_get_z(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->size.z);
	
	return(JS_TRUE);
}

bool js_obj_size_get_eyeOffset(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->size.eye_offset);
	
	return(JS_TRUE);
}

bool js_obj_size_get_weight(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->size.weight);
	
	return(JS_TRUE);
}

bool js_obj_size_get_clickDistance(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->click.distance);
	
	return(JS_TRUE);
}

bool js_obj_size_get_cameraZAdjust(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->camera_z_adjust);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_size_set_x(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.x=script_value_to_int(*vp);
	object_set_radius(obj);
	
	return(JS_TRUE);
}

bool js_obj_size_set_y(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.y=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_obj_size_set_z(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.z=script_value_to_int(*vp);
	object_set_radius(obj);
	
	return(JS_TRUE);
}

bool js_obj_size_set_eyeOffset(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.eye_offset=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_obj_size_set_weight(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.weight=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_obj_size_set_clickDistance(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->click.distance=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_obj_size_set_cameraZAdjust(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->camera_z_adjust=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

/* =======================================================

      Function
      
======================================================= */

JSBool js_obj_size_grow_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			resize;
	obj_type		*obj;

	resize=script_value_to_float(argv[0]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_direct(obj,resize);
	
	return(JS_TRUE);
}

JSBool js_obj_size_grow_over_time_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				msec;
	float			resize;
	obj_type		*obj;

	resize=script_value_to_float(argv[0]);
	msec=script_value_to_int(argv[1]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,NULL,NULL);
	
	return(JS_TRUE);
}

JSBool js_obj_size_grow_over_time_change_size_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				msec;
	float			resize;
	d3pnt			size;
	obj_type		*obj;

	resize=script_value_to_float(argv[0]);
	
	size.x=script_value_to_int(argv[1]);
	size.z=script_value_to_int(argv[2]);
	size.y=script_value_to_int(argv[3]);

	msec=script_value_to_int(argv[4]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,&size,NULL);
	
	return(JS_TRUE);
}

JSBool js_obj_size_grow_over_time_change_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				msec;
	float			resize;
	d3pnt			offset;
	obj_type		*obj;

	resize=script_value_to_float(argv[0]);
	
	offset.x=script_value_to_int(argv[1]);
	offset.z=script_value_to_int(argv[2]);
	offset.y=script_value_to_int(argv[3]);

	msec=script_value_to_int(argv[4]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,NULL,&offset);
	
	return(JS_TRUE);
}
