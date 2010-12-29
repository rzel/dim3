/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: List Palette Common

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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "dialog.h"

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

bool							list_palette_open;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void list_palette_initialize(void)
{
	list_palette_open=TRUE;
}

void list_palette_shutdown(void)
{
}

void list_palette_list_initialize(list_palette_type *list,char *title)
{
	strcpy(list->title,title);

	list->scroll_page=0;
	list->total_high=0;

	list->push_on=FALSE;
	list->push_idx=-1;

	list->item_type=-1;
	list->item_idx=-1;

	list->items=(list_palette_item_type*)malloc(list_max_item_count*sizeof(list_palette_item_type));
}

void list_palette_list_shutdown(list_palette_type *list)
{
	if (list->items!=NULL) free(list->items);
}

void list_palette_set_title(list_palette_type *list,char *title)
{
	strcpy(list->title,title);
}

/* =======================================================

      List Add Items
      
======================================================= */

list_palette_item_type* list_palette_create_item(list_palette_type *list,int ctrl_type)
{
	list_palette_item_type		*item;

	item=&list->items[list->item_count];
	list->item_count++;

	item->ctrl_type=ctrl_type;

	item->x=list->box.lx+(list_palette_border_sz+4);
	if (ctrl_type!=list_item_ctrl_header) item->x+=10;

	item->y=((list->item_count*list_item_font_high)+list_title_high)+list->box.ty;
	item->y-=(list->scroll_page*list_item_scroll_size);

	list->total_high+=list_item_font_high;

	return(item);
}

void list_palette_add_header(list_palette_type *list,int piece_type,char *name)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_header);

	item->type=piece_type;
	item->idx=-1;
	item->id=-1;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_item(list_palette_type *list,int piece_type,int piece_idx,char *name,bool selected,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_text);

	item->type=piece_type;
	item->idx=piece_idx;
	item->id=-1;

	item->selected=selected;
	item->disabled=disabled;

	strcpy(item->name,name);
}

void list_palette_add_color(list_palette_type *list,int piece_type,int piece_idx,d3col *col,bool selected,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_color);

	item->type=piece_type;
	item->idx=piece_idx;
	item->id=-1;

	item->selected=selected;
	item->disabled=disabled;

	memmove(&item->value.col,col,sizeof(d3col));
}

void list_palette_add_string(list_palette_type *list,int id,char *name,char *value,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_string);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	
	if (strlen(value)>=25) {
		strncpy(item->value.str,value,25);
		strcpy((char*)&item->value.str[25],"...");
	}
	else {
		strcpy(item->value.str,value);
	}
}

void list_palette_add_string_int(list_palette_type *list,int id,char *name,int value,bool disabled)
{
	char		str[32];
	
	sprintf(str,"%d",value);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_string_float(list_palette_type *list,int id,char *name,float value,bool disabled)
{
	char		str[32];
	
	sprintf(str,"%.2f",value);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool value,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_checkbox);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.checkbox=value;
}

void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_pick_color);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	memmove(&item->value.col,col,sizeof(d3col));
}

void list_palette_add_point(list_palette_type *list,int id,char *name,d3pnt *pnt,bool disabled)
{
	char		str[32];
	
	sprintf(str,"%d,%d,%d",pnt->x,pnt->y,pnt->z);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_angle(list_palette_type *list,int id,char *name,d3ang *ang,bool disabled)
{
	char		str[32];
	
	sprintf(str,"%.2f,%.2f,%.2f",ang->x,ang->y,ang->z);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_uv(list_palette_type *list,int id,char *name,d3fpnt *fpnt,bool disabled)
{
	char		str[32];
	
	sprintf(str,"%.2f,%.2f",fpnt->x,fpnt->y);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_texture(list_palette_type *list,int id,char *name,int txt_idx,bool disabled)
{
	char		str[name_str_len];

	str[0]=0x0;

	if (txt_idx!=-1) {
		if (map.textures[txt_idx].frames[0].name[0]==0x0) {
			strcpy(str,"(none)");
		}
		else {
			strcpy(str,map.textures[txt_idx].frames[0].name);
		}
	}
	
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_shader(list_palette_type *list,int id,char *name,char *shader_name,bool disabled)
{
	if (shader_name[0]==0x0) {
		list_palette_add_string(list,id,name,"Default",disabled);
		return;
	}
	
	list_palette_add_string(list,id,name,shader_name,disabled);
}

/* =======================================================

      List Delete Items
      
======================================================= */

void list_palette_delete_all_items(list_palette_type *list)
{
	list->item_count=0;
	list->total_high=0;
}

/* =======================================================

      List Palette Scroll Pages
      
======================================================= */

int list_palette_get_scroll_page_count(list_palette_type *list)
{
	int				high;
	
	high=list->total_high-(list->box.by-(list->box.ty+list_title_high));
	if (high<=0) return(0);
	
	return((high/list_item_scroll_size)+1);
}

/* =======================================================

      List Palette Draw Utilities
      
======================================================= */

void list_palette_draw_item_color_box(list_palette_type *list,list_palette_item_type *item,d3col *col)
{
	int					x;

	x=list->box.rx-(list_item_font_high+(list_palette_scroll_wid+4));

	glColor4f((col->r*0.5f),(col->g*0.5f),(col->b*0.5f),1.0f);

	glBegin(GL_QUADS);
	glVertex2i(x,((item->y-list_item_font_high)+2));
	glVertex2i((x+list_item_font_high),((item->y-list_item_font_high)+2));
	glVertex2i((x+list_item_font_high),(item->y-2));
	glVertex2i(x,(item->y-2));
	glEnd();

	glColor4f(col->r,col->g,col->b,1.0f);
			
	glBegin(GL_QUADS);
	glVertex2i((x+1),((item->y-list_item_font_high)+3));
	glVertex2i(((x+list_item_font_high)-1),((item->y-list_item_font_high)+3));
	glVertex2i(((x+list_item_font_high)-1),(item->y-3));
	glVertex2i((x+1),(item->y-3));
	glEnd();
}

void list_palette_draw_item_check_box(list_palette_type *list,list_palette_item_type *item,bool checked)
{
	int					lx,rx,ty,by;

	lx=list->box.rx-(list_item_font_high+(list_palette_scroll_wid+2));
	rx=lx+(list_item_font_high-2);

	ty=(item->y-list_item_font_high)+2;
	by=item->y;

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glColor4f(0.9f,0.9f,0.9f,1.0f);

	glBegin(GL_QUADS);
	glVertex2i((lx+1),(ty+1));
	glVertex2i((rx-1),(ty+1));
	glVertex2i((rx-1),(by-1));
	glVertex2i((lx+1),(by-1));
	glEnd();

	if (checked) {
		glLineWidth(3.0f);

		glColor4f(0.0f,0.8f,0.0f,1.0f);

		glBegin(GL_LINES);
		glVertex2i((lx+1),(ty+1));
		glVertex2i((rx-1),(by-1));
		glVertex2i((rx-1),(ty+1));
		glVertex2i((lx+1),(by-1));
		glEnd();

		glLineWidth(1.0f);
	}
}

void list_palette_draw_item_string(list_palette_type *list,list_palette_item_type *item)
{
	d3col						col;

	if (!item->disabled) {
		text_draw_right((list->box.rx-(list_palette_scroll_wid+4)),item->y,list_item_font_size,NULL,item->value.str);
		return;
	}

	col.r=col.g=0.0f;
	col.b=1.0f;
	text_draw_right((list->box.rx-(list_palette_scroll_wid+4)),item->y,list_item_font_size,&col,item->value.str);
}

/* =======================================================

      List Palette Draw
      
======================================================= */

void list_palette_draw_item(list_palette_type *list,int idx)
{
	bool						selected;
	list_palette_item_type		*item;
	
	item=&list->items[idx];

		// early exits

	if ((item->y<(list->box.ty+list_title_high)) || ((item->y-list_item_font_high)>list->box.by)) return;

		// draw header
		
	if (item->ctrl_type==list_item_ctrl_header) {

		glColor4f(0.9f,0.9f,0.9f,1.0f);

		glBegin(GL_QUADS);
		glVertex2i(list->box.lx,(item->y-list_item_font_high));
		glVertex2i((list->box.rx-list_palette_scroll_wid),(item->y-list_item_font_high));
		glVertex2i((list->box.rx-list_palette_scroll_wid),item->y);
		glVertex2i(list->box.lx,item->y);
		glEnd();
	}
		
		// draw selected item
	
	else {

		if (!list->push_on) {
			selected=item->selected;
		}
		else {
			selected=(idx==list->push_idx);
		}

		if (selected) {
			glColor4f(1.0f,1.0f,0.0f,1.0f);

			glBegin(GL_QUADS);
			glVertex2i(list->box.lx,((item->y-list_item_font_high)+1));
			glVertex2i((list->box.rx-list_palette_scroll_wid),((item->y-list_item_font_high)+1));
			glVertex2i((list->box.rx-list_palette_scroll_wid),item->y);
			glVertex2i(list->box.lx,item->y);
			glEnd();
		}
	}

		// draw item

	switch (item->ctrl_type) {

			// text or header

		case list_item_ctrl_header:
		case list_item_ctrl_text:
			text_draw(item->x,item->y,list_item_font_size,NULL,item->name);
			break;

			// color

		case list_item_ctrl_color:
			text_draw(item->x,item->y,list_item_font_size,NULL,"Light");
			list_palette_draw_item_color_box(list,item,&item->value.col);
			break;

			// string

		case list_item_ctrl_string:
			text_draw(item->x,item->y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_string(list,item);
			break;

			// checkbox

		case list_item_ctrl_checkbox:
			text_draw(item->x,item->y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_check_box(list,item,item->value.checkbox);
			break;

			// pick color

		case list_item_ctrl_pick_color:
			text_draw(item->x,item->y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_color_box(list,item,&item->value.col);
			break;

	}
}

void list_palette_draw(list_palette_type *list)
{
	int						n,lx,mx,rx,ty,by,
							thumb_ty,thumb_by,page_count;
	d3rect					wbox;

		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(list->box.lx,(wbox.by-list->box.by),(list->box.rx-list->box.lx),(list->box.by-list->box.ty));

	glViewport(list->box.lx,(wbox.by-list->box.by),(list->box.rx-list->box.lx),(list->box.by-list->box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)list->box.lx,(GLdouble)list->box.rx,(GLdouble)list->box.by,(GLdouble)list->box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

		// background

	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(list->box.lx,list->box.ty);
	glVertex2i(list->box.rx,list->box.ty);
	glVertex2i(list->box.rx,list->box.by);
	glVertex2i(list->box.lx,list->box.by);
	glEnd();

		// items

	for (n=0;n!=list->item_count;n++) {
		list_palette_draw_item(list,n);
	}
	
		// scroll bar
	
	lx=list->box.rx-list_palette_scroll_wid;
	ty=list->box.ty+list_title_high;
	by=list->box.by;
	
	glColor4f(0.5f,0.5f,0.5f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(lx,by);
	glEnd();

	page_count=list_palette_get_scroll_page_count(list);
	
	if ((list->item_count!=0) && (page_count!=0)) {
		
		thumb_ty=ty+(((by-ty)*list->scroll_page)/(page_count+1));
		if (thumb_ty<ty) thumb_ty=ty;
		
		thumb_by=ty+(((by-ty)*(list->scroll_page+1))/(page_count+1));
		if (thumb_by>by) thumb_by=by;

		glColor4f(0.0f,0.0f,0.0f,1.0f);
			
		glBegin(GL_QUADS);
		glVertex2i(lx,thumb_ty);
		glVertex2i(list->box.rx,thumb_ty);
		glVertex2i(list->box.rx,thumb_by);
		glVertex2i(lx,thumb_by);
		glEnd();

		glColor4f(0.9f,0.9f,0.9f,1.0f);
			
		glBegin(GL_QUADS);
		glVertex2i((lx+1),(thumb_ty+1));
		glVertex2i((list->box.rx-1),(thumb_ty+1));
		glVertex2i((list->box.rx-1),(thumb_by-1));
		glVertex2i((lx+1),(thumb_by-1));
		glEnd();
	}
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINES);
	glVertex2i(lx,list->box.ty);
	glVertex2i(lx,list->box.by);
	glEnd();

		// title

	ty=list->box.ty;
	by=ty+list_title_high;

	glColor4f(0.6f,0.6f,0.6f,1.0f);

	glBegin(GL_QUADS);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(list->box.lx,by);
	glEnd();
	
	text_draw_center(((list->box.lx+list->box.rx)>>1),(by-2),list_title_font_size,NULL,list->title);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
	glVertex2i(list->box.lx,(ty+1));
	glVertex2i(list->box.rx,(ty+1));
	glVertex2i(list->box.lx,by);
	glVertex2i(list->box.rx,by);
	glEnd();
	
		// click-close border
	
	lx=list->box.lx;
	rx=list->box.lx+list_palette_border_sz;
	mx=(lx+rx)>>1;
		
	glBegin(GL_QUADS);
	glColor4f(0.0f,0.0f,0.5f,1.0f);
	glVertex2i(lx,list->box.by);
	glVertex2i(lx,list->box.ty);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i(mx,list->box.ty);
	glVertex2i(mx,list->box.by);
	glColor4f(0.0f,0.0f,0.5f,1.0f);
	glVertex2i(rx,list->box.by);
	glVertex2i(rx,list->box.ty);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i(mx,list->box.ty);
	glVertex2i(mx,list->box.by);
	glEnd();

	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      List Palette Scrolling
      
======================================================= */

void list_palette_scroll_up(list_palette_type *list)
{
	if (list->scroll_page>0) {
		list->scroll_page--;
		main_wind_draw();
	}
}

void list_palette_scroll_down(list_palette_type *list)
{
	int				page_count;
	
	page_count=list_palette_get_scroll_page_count(list);
	
	if (list->scroll_page<page_count) {
		list->scroll_page++;
		main_wind_draw();
	}
}

void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move)
{
	if (move>0) list_palette_scroll_up(list);
	if (move<0) list_palette_scroll_down(list);
}

/* =======================================================

      Item Palette Click
      
======================================================= */

bool list_palette_click_item(list_palette_type *list,int item_idx)
{
	d3pnt					pt;
	list_palette_item_type	*item;

		// get clicked item

	item=&list->items[item_idx];
	
	if (item->disabled) return(FALSE);

		// do the hold and click

	list->push_on=TRUE;
	list->push_idx=item_idx;
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
		if ((pt.x<list->box.lx) || (pt.x>=(list->box.rx-list_palette_scroll_wid)) || (pt.y<(item->y-list_item_font_high)) || (pt.y>=item->y)) {
			if (list->push_idx!=-1) {
				list->push_idx=-1;
				main_wind_draw();
			}
		}
		else {
			if (list->push_idx!=item_idx) {
				list->push_idx=item_idx;
				main_wind_draw();
			}
		}
		
		usleep(10000);
	}

	list->push_on=FALSE;

	if (list->push_idx!=item_idx) {
		main_wind_draw();
		return(FALSE);
	}

		// pass back clicked item
		
	list->item_type=item->type;
	list->item_idx=item->idx;
	list->item_id=item->id;

	return(TRUE);
}

void list_palette_click_scroll_bar(list_palette_type *list)
{
	int						old_page,page,page_count,
							page_size,y,offset_y,thumb_y;
	d3pnt					pt,org_pt;
	
		// scrolling sizes
		
	page_count=list_palette_get_scroll_page_count(list);
	if (page_count==0) return;
	
	page_size=((list->box.by-list->box.ty)/(page_count+1));

		// scrolling
		
	old_page=list->scroll_page;
	os_get_cursor(&org_pt);

	thumb_y=list->box.ty+(((list->box.by-list->box.ty)*list->scroll_page)/(page_count+1));
	offset_y=thumb_y-org_pt.y;
	
	while (!os_track_mouse_location(&pt,NULL)) {
		
		y=(pt.y-org_pt.y)-offset_y;
		page=old_page+(y/page_size);
		if (page<0) page=0;
		if (page>page_count) page=page_count;
		
		if (page!=list->scroll_page) {
			list->scroll_page=page;
			main_wind_draw();
		}
		
		usleep(10000);
	}
}

bool list_palette_click(list_palette_type *list,d3pnt *pnt,bool double_click)
{
	int						item_idx;

	pnt->x-=list->box.lx;
	pnt->y-=list->box.ty;

		// click in close border

	if (pnt->x<=list_palette_border_sz) {
		list_palette_open=!list_palette_open;
		main_wind_draw();
		return(FALSE);
	}
	
		// click in title
		
	if (pnt->y<list_title_high) return(FALSE);
	
		// click in scroll bar
		
	if (pnt->x>=((list->box.rx-list->box.lx)-list_palette_scroll_wid)) {
		list_palette_click_scroll_bar(list);
		return(FALSE);
	}

		// click in item

	item_idx=((pnt->y-list_title_high)+(list->scroll_page*list_item_scroll_size))/list_item_font_high;
	if ((item_idx<0) || (item_idx>=list->item_count)) return(FALSE);
	
	return(list_palette_click_item(list,item_idx));
}