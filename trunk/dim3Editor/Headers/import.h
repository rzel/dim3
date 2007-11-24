/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Importing Functions

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

//
// import utility
//

extern void import_clear_map(void);
extern void import_fix_segments_uv(void);
extern void import_add_simple_lights(void);
extern void import_add_player_spot(void);
extern void import_nav_event_proc(const NavEventCallbackMessage callBackSelector,NavCBRecPtr callBackParms,NavCallBackUserData callBackUD);
extern Boolean import_nav_file_filter(AEDesc *theItem,void *info,void *callBackUD,NavFilterModes filterMode);
extern bool import_load_file(char *path,char *ext);

//
// import text decoder
//

extern int textdecode_count_linestarts(void);
extern void textdecode_linestarts(void);
extern bool textdecode_open(char *path,char p_piece_break);
extern void textdecode_close(void);
extern int textdecode_count(void);
extern int textdecode_find(int str_line,char *txt);
extern void textdecode_get_line(int i,char *txt);
extern void textdecode_get_piece(int i,int k,char *txt);

//
// import obj map
//

extern bool import_obj_segment_type(int npt,int *pvtx,segment_type *seg);
extern bool import_obj_run(char *path,char *err_str);
extern bool import_obj_fix_segment_xz_touch_wall_top(int x,int z,int y);
extern void import_obj_fix_segments(void);
extern void import_obj(void);

//
// import height map
//

extern float import_height_map_get_height(bitmap_type *bitmap,int portal_x_start,int portal_z_start,int portal_total_x_sz,int portal_total_z_sz,portal_type *portal,int seg_x,int seg_z);
extern void import_height_map(void);

//
// auto-generate maps
//

extern void import_auto_generate(void);

