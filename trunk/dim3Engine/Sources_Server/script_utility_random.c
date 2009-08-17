/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.random object

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

extern js_type			js;

JSValueRef js_utility_random_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_utility_random_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_utility_random_get_integer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_random_get_float_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_random_get_boolean_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_random_get_pos_or_neg_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	utility_random_functions[]={
							{"getInteger",			js_utility_random_get_integer_func,		2},
							{"getFloat",			js_utility_random_get_float_func,		2},
							{"getBoolean",			js_utility_random_get_boolean_func,		0},
							{"getPosOrNeg",			js_utility_random_get_pos_or_neg_func,	0},
							{0}};

JSClassRef			utility_random_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_utility_random_object(void)
{
	utility_random_class=script_create_class("utility_random_class",js_utility_random_get_property,js_utility_random_set_property);
}

void script_free_utility_random_object(void)
{
	script_free_class(utility_random_class);
}

JSObjectRef script_add_utility_random_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,utility_random_class,"random",NULL,utility_random_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_utility_random_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_utility_random_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Random Functions
      
======================================================= */

JSValueRef js_utility_random_get_integer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				min,max;

	min=script_value_to_int(cx,argv[0]);
	max=script_value_to_int(cx,argv[1]);

	return(script_int_to_value(cx,random_int(abs(max-min))+min));
}

JSValueRef js_utility_random_get_float_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float				min,max;

	min=script_value_to_float(cx,argv[0]);
	max=script_value_to_float(cx,argv[1]);

	return(script_float_to_value(cx,random_float(fabsf(max-min))+min));
}

JSValueRef js_utility_random_get_boolean_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	return(script_bool_to_value(cx,random_boolean()));
}

JSValueRef js_utility_random_get_pos_or_neg_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	return(script_int_to_value(cx,random_boolean()?1:(-1)));
}
