/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Definitions Header

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

//
// GUI Defines
//

#define max_element								128
#define max_element_str_len						256
#define max_element_value_str_len				64
#define max_element_column						32
#define max_element_tab							8

//
// GUI elements
//

#define element_type_button						0
#define element_type_bitmap						1
#define element_type_text						2
#define element_type_text_field					3
#define element_type_number						4
#define element_type_checkbox					5
#define element_type_combo						6
#define element_type_slider						7
#define element_type_table						8
#define element_type_tab						9
#define element_type_color						10
#define element_type_text_box					11
#define element_type_info_field					12
#define element_type_model						13

//
// GUI position mode
//

#define element_pos_top							0
#define element_pos_bottom						1
#define element_pos_center						2

#define element_pos_left						0
#define element_pos_right						1

//
// GUI table bitmap mode
//

#define element_table_bitmap_none				0
#define element_table_bitmap_data				1
#define element_table_bitmap_document			2

//
// font selection
//

#define font_interface_index					0
#define font_hud_index							1

//
// GUI scaling factors
//

#define cursor_size_factor						0.05f

#define text_height_factor						1.2f

#define text_small_wid_factor					0.02f
#define text_medium_wid_factor					0.025f
#define text_large_wid_factor					0.038f

#define element_control_padding_factor			0.01f

#define element_control_draw_height				0.025f
#define element_control_draw_short_width		0.15f
#define element_control_draw_long_width			0.4f

#define element_table_max_image					16
#define element_table_bitmap_size				64
#define element_table_max_check					128
#define element_table_check_column_size			0.05f

//
// gradients
//

#define element_gradient_factor					0.75f

//
// screenshot backgrounds
//

#define gui_screenshot_blur_count				10

//
// GUI element structures
//

typedef struct		{
						int						image_idx,image_select_idx;
						char					name[name_str_len];
						bool					text_only;
					} element_button_type;

typedef struct		{
						int						size,just;
						bool					alert;
					} element_text_type;

typedef struct		{
						int						min,max;
					} element_number_type;
					
typedef struct		{
						float					value,min,max;
					} element_slider_type;

typedef struct		{
						float					percent_size;
						char					name[name_str_len];
					} element_column_type;

typedef struct		{
						int						image_idx;
						char					path[1024];
					} element_image_type;

typedef struct		{
						int						ncolumn,next_image_idx,bitmap_mode,
												busy_count,busy_total_count;
						char					busy_str[256],checks[element_table_max_check];
						bool					checkbox;
						element_column_type		cols[max_element_column];
						element_image_type		images[element_table_max_image];
					} element_table_type;

typedef struct		{
						int						ntab;
						char					name[max_element_tab][name_str_len];
					} element_tab_type;

typedef struct		{
						int						line_count,pos_y;
						bool					scroll_up_ok,scroll_down_ok;
					} element_text_box_type;

typedef struct		{
						model_draw				*draw;
					} element_model_type;

typedef union		{
						element_button_type		button;
						element_text_type		text;
						element_number_type		number;
						element_slider_type		slider;
						element_table_type		table;
						element_tab_type		tab;
						element_text_box_type	text_box;
						element_model_type		model;
					} element_setup_type;

typedef struct		{
						int						id,type,x,y,wid,high,
												value,offset,max_value_str_sz;
						bool					selectable,enabled,hidden,framed;
						char					str[max_element_str_len],
												value_str[max_element_value_str_len],
												*data;
						element_setup_type		setup;
					} element_type;
