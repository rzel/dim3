/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Lighting Functions Header

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

extern int iface_halo_find(char *name);		// supergumba -- move

extern void halo_draw_clear(void);
extern void halo_draw_add(int x,int y,int z,int obj_idx,model_draw_halo *mdl_halo);
extern void halo_draw_setup(void);
extern void halo_draw_render(void);
