/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility object

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

extern void script_add_utility_angle_object(JSObject *parent_obj);
extern void script_add_utility_point_object(JSObject *parent_obj);
extern void script_add_utility_random_object(JSObject *parent_obj);
extern void script_add_utility_pack_object(JSObject *parent_obj);

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_utility_object(void)
{
}

void script_free_global_utility_object(void)
{
}

void script_add_global_utility_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"utility",NULL,NULL,0);
	
	script_add_utility_angle_object(j_obj);
	script_add_utility_point_object(j_obj);
	script_add_utility_random_object(j_obj);
	script_add_utility_pack_object(j_obj);
}

