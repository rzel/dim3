/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.size object

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
#include "projectiles.h"

extern js_type			js;

JSBool js_proj_size_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_get_weight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_size_set_weight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

script_js_property	proj_size_props[]={
							{"x",					js_proj_size_get_x,					js_proj_size_set_x},
							{"y",					js_proj_size_get_y,					js_proj_size_set_y},
							{"z",					js_proj_size_get_z,					js_proj_size_set_z},
							{"weight",				js_proj_size_get_weight,			js_proj_size_set_weight},
							{0}};

JSClass				*proj_size_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_size_object(void)
{
	proj_size_class=script_create_class("proj_size_class",js_proj_size_get_property,js_proj_size_set_property);
}

void script_free_proj_size_object(void)
{
	script_free_class(proj_size_class);
}

JSObject* script_add_proj_size_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,proj_size_class,"size",proj_size_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_proj_size_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,proj_size_props));
}

JSBool js_proj_size_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,proj_size_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_proj_size_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_int_to_value(proj_setup->size.x);
	
	return(JS_TRUE);
}

JSBool js_proj_size_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_int_to_value(proj_setup->size.y);
	
	return(JS_TRUE);
}

JSBool js_proj_size_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_int_to_value(proj_setup->size.z);
	
	return(JS_TRUE);
}

JSBool js_proj_size_get_weight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_int_to_value(proj_setup->size.weight);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_proj_size_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->size.x=script_value_to_int(*vp);
	proj_setup_set_radius(proj_setup);
	
	return(JS_TRUE);
}

JSBool js_proj_size_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->size.y=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_size_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->size.z=script_value_to_int(*vp);
	proj_setup_set_radius(proj_setup);
	
	return(JS_TRUE);
}

JSBool js_proj_size_set_weight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->size.weight=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

