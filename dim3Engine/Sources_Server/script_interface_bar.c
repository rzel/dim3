/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.bar object

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
#include "interfaces.h"

extern js_type			js;

JSValueRef js_interface_bar_show_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bar_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bar_hide_all_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bar_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bar_resize_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bar_set_value_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bar_set_alpha_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	interface_bar_functions[]={
							{"show",				js_interface_bar_show_func,				kJSPropertyAttributeDontDelete},
							{"hide",				js_interface_bar_hide_func,				kJSPropertyAttributeDontDelete},
							{"hideAll",				js_interface_bar_hide_all_func,			kJSPropertyAttributeDontDelete},
							{"move",				js_interface_bar_move_func,				kJSPropertyAttributeDontDelete},
							{"resize",				js_interface_bar_resize_func,			kJSPropertyAttributeDontDelete},
							{"setValue",			js_interface_bar_set_value_func,		kJSPropertyAttributeDontDelete},
							{"setAlpha",			js_interface_bar_set_alpha_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			interface_bar_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_bar_object(void)
{
	interface_bar_class=script_create_class("interface_bar_class",NULL,interface_bar_functions);
}

void script_free_interface_bar_object(void)
{
	script_free_class(interface_bar_class);
}

JSObjectRef script_add_interface_bar_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,interface_bar_class,"bar"));
}

/* =======================================================

      Bar Functions
      
======================================================= */

JSValueRef js_interface_bar_show_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_bar_type			*bar;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	bar=script_find_bar_from_name(cx,argv[0],exception);
	if (bar!=NULL) bar->show=TRUE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bar_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_bar_type			*bar;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	bar=script_find_bar_from_name(cx,argv[0],exception);
	if (bar!=NULL) bar->show=FALSE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bar_hide_all_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	hud_bars_hide_all();
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bar_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_bar_type			*bar;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	bar=script_find_bar_from_name(cx,argv[0],exception);
	if (bar!=NULL) {
		bar->x=script_value_to_int(cx,argv[1]);
		bar->y=script_value_to_int(cx,argv[2]);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_bar_resize_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_bar_type			*bar;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	bar=script_find_bar_from_name(cx,argv[0],exception);
	if (bar!=NULL) {
		bar->x_size=script_value_to_int(cx,argv[1]);
		bar->y_size=script_value_to_int(cx,argv[2]);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_bar_set_value_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_bar_type			*bar;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	bar=script_find_bar_from_name(cx,argv[0],exception);
	if (bar!=NULL) bar->value=script_value_to_float(cx,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bar_set_alpha_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_bar_type			*bar;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	bar=script_find_bar_from_name(cx,argv[0],exception);
	if (bar!=NULL) bar->fill_alpha=script_value_to_float(cx,argv[1]);
	
	return(script_null_to_value(cx));
}