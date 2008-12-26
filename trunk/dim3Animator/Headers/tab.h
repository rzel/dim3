/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Tab Functions

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

extern void add_db_column(ControlRef ctrl,char *name,int idx,int type,int sz,int spot);

extern void start_pose_controls(WindowRef wind,Rect *box);
extern void end_pose_controls(void);
extern void resize_pose_controls(Rect *box);

extern void start_bone_controls(WindowRef wind,Rect *box);
extern void end_bone_controls(void);
extern void resize_bone_controls(Rect *box);

extern void start_animate_controls(WindowRef wind,Rect *bpx);
extern void end_animate_controls(void);
extern void resize_animate_controls(Rect *box);

extern void start_vertex_controls(WindowRef wind,Rect *box);
extern void end_vertex_controls(void);
extern void resize_vertex_controls(Rect *box);

extern void reset_pose_list(void);
extern void reset_bone_list(void);
extern void reset_animate_list(void);
extern void reset_vertex_tab(void);

extern void hilite_vertex_rows(void);




