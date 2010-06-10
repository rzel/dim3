/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Join

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

#include "network.h"
#include "objects.h"
#include "remotes.h"
#include "interfaces.h"
#include "video.h"
#include "sounds.h"

#define join_pane_news					0
#define join_pane_lan					1
#define join_pane_internet				2

#define join_tab_id						0

#define join_button_join_id				1
#define join_button_cancel_id			2
#define join_button_rescan_id			3
#define join_table_id					4
#define join_status_id					5

#define join_news_id					6

extern map_type				map;
extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							join_tab_value,join_thread_lan_start_tick;
char						*join_table_data;
bool						join_thread_started,join_thread_quit;
SDL_Thread					*join_thread;
SDL_mutex					*join_thread_lock;

int							join_count;
join_server_info			join_list[max_setup_network_host];
setup_network_hosts_type	join_combine_host;
			
/* =======================================================

      Build Join List
      
======================================================= */

char* join_create_list(void)
{
	int					n,sz;
	char				*c,*row_data;
	join_server_info	*info;
	
		// create row data
		
	sz=(join_count+1)*128;
	
	row_data=malloc(sz);
	bzero(row_data,sz);
	
		// build table
	
	info=join_list;
	c=row_data;
	
	for (n=0;n!=join_count;n++) {
		snprintf(c,128,"Bitmaps/Icons_Map;%s;%s\t%s @ %s\t%d/%d\t%dms",info->map_name,info->name,info->game_name,info->map_name,info->player_count,info->player_max_count,info->ping_msec);
		c[127]=0x0;
		
		c+=128;
		info++;
	}
	
	return(row_data);
}

void join_ping_thread_done(void)
{
	element_table_busy(join_table_id,NULL,-1,-1);
	element_enable(join_button_rescan_id,TRUE);
}

/* =======================================================

      Add Host to Info List
      
======================================================= */

bool join_ping_thread_add_host_to_table(int start_tick,network_reply_info *reply_info)
{
	int					n,cnt;
	char				*row_data;
	join_server_info	*info;

		// only add if same project

	if (strcasecmp(reply_info->proj_name,hud.proj_name)!=0) return(FALSE);

		// add to list

	SDL_mutexP(join_thread_lock);

	info=&join_list[join_count++];

		// host

	strcpy(info->name,reply_info->host_name);
	strcpy(info->ip,reply_info->host_ip_resolve);

	strcpy(info->game_name,reply_info->game_name);
	strcpy(info->map_name,reply_info->map_name);

	info->option_flags=ntohl(reply_info->option_flags);

		// players

	info->player_list.count=(int)ntohs(reply_info->player_list.count);
	info->player_list.max_count=(int)ntohs(reply_info->player_list.max_count);

	cnt=info->player_list.count;
	if (cnt>join_info_max_players) cnt=join_info_max_players;
	if (cnt<0) cnt=0;

	for (n=0;n!=cnt;n++) {
		strcpy(info->player_list.players[n].name,reply_info->player_list.players[n].name);
		info->player_list.players[n].bot=(((int)ntohs(reply_info->player_list.players[n].bot))!=0);
		info->player_list.players[n].score=(int)ntohs(reply_info->player_list.players[n].score);
	}

		// ping time

	info->ping_msec=time_get()-start_tick;

		// update table

	row_data=join_create_list();
	element_set_table_data(join_table_id,row_data);
	free(row_data);

	SDL_mutexV(join_thread_lock);

	return(TRUE);
}

/* =======================================================

      Join Ping LAN Threads
      
======================================================= */

void join_ping_thread_lan_run(void)
{
	int					max_tick,action,player_uid;
	unsigned char		msg[net_max_msg_size];
	bool				good_reply;
	d3socket			broadcast_sock;

		// create a socket and send out a broadcast
		// to all addresses on this subnet, looking for
		// replies from a dim3 server
		
	broadcast_sock=net_open_udp_socket();
	if (broadcast_sock==D3_NULL_SOCKET) return;

	net_socket_enable_broadcast(broadcast_sock);
	
	if (!net_sendto_msg(broadcast_sock,INADDR_BROADCAST,net_port_host,net_action_request_info,net_player_uid_none,NULL,0)) {
		net_close_socket(&broadcast_sock);
		return;
	}
	
		// now wait for any replies
	
	join_thread_lan_start_tick=time_get();
	max_tick=client_timeout_wait_seconds*1000;
	
		// put socket in non-blocking so UI runs
		
	net_socket_blocking(broadcast_sock,FALSE);
	
	while (((join_thread_lan_start_tick+max_tick)>time_get()) && (!join_thread_quit)) {

			// any replies?
			
		good_reply=FALSE;
		
		if (net_recvfrom_mesage(broadcast_sock,NULL,NULL,&action,&player_uid,msg,NULL)) {
			if (action==net_action_reply_info) {
				good_reply=join_ping_thread_add_host_to_table(join_thread_lan_start_tick,(network_reply_info*)msg);
			}
		}
		
			// if no good reply, then sleep for a bit
			
		if (!good_reply) {
			usleep(100000);
		}
		
		element_table_busy(join_table_id,"Looking for LAN Clients",(time_get()-join_thread_lan_start_tick),max_tick);
	}
	
	net_close_socket(&broadcast_sock);
}

int join_ping_thread_lan(void *arg)
{
		// run the lan search thread
		
	join_ping_thread_lan_run();
	
		// reset the UI
		
	element_table_busy(join_table_id,NULL,1,1);
	join_ping_thread_done();
	
	return(0);
}

/* =======================================================

      Join Ping Internet Threads
      
======================================================= */

void join_ping_thread_internet_server(char *ip)
{
	int						action,player_uid,msec,max_tick;
	unsigned long			ip_addr,recv_ip_addr;
	unsigned char			msg[net_max_msg_size];
	bool					got_reply;
	d3socket				sock;

	msec=time_get();
	
		// send request info to host
		
	sock=net_open_udp_socket();
	if (sock==D3_NULL_SOCKET) return;
	
	net_socket_blocking(sock,FALSE);
	
	ip_addr=inet_addr(ip);
	if (ip_addr==INADDR_NONE) return;
	
	if (!net_sendto_msg(sock,ip_addr,net_port_host,net_action_request_info,net_player_uid_none,NULL,0)) {
		net_close_socket(&sock);
		return;
	}

		// get the reply
	
	got_reply=FALSE;
	
	max_tick=client_timeout_wait_seconds*1000;
	
	while (((msec+max_tick)>time_get()) && (!join_thread_quit)) {
		if (net_recvfrom_mesage(sock,&recv_ip_addr,NULL,&action,&player_uid,msg,NULL)) {
			if ((recv_ip_addr==ip_addr) && (action==net_action_reply_info)) {
				got_reply=TRUE;
				break;
			}
		}
		usleep(100000);
	}
	
		// close socket
		
	net_close_socket(&sock);

	if (!got_reply) return;

		// add to list

	join_ping_thread_add_host_to_table(msec,(network_reply_info*)msg);
}

void join_ping_thread_internet_run(void)
{
	int							n;
	char						str[256];
	setup_network_host_type		*host;
	
		// run ping until finished or
		// join page ends

	for (n=0;n!=join_combine_host.count;n++) {
	
		if (join_thread_quit) return;

			// progress

		host=&join_combine_host.hosts[n];
		
		if (host->name[0]!=0x0) {
			sprintf(str,"Querying %s",host->name);
		}
		else {
			sprintf(str,"Querying %s",host->ip);
		}
		
		element_table_busy(join_table_id,str,n,join_combine_host.count);

			// ping host

		join_ping_thread_internet_server(host->ip);
	}
}

int join_ping_thread_internet(void *arg)
{
		// run the internet search thread
		
	join_ping_thread_internet_run();
	
		// reset the UI
		
	element_table_busy(join_table_id,NULL,1,1);
	join_ping_thread_done();
	
	return(0);
}

/* =======================================================

      Join Ping Threads Mainline
      
======================================================= */

void join_ping_thread_start(void)
{
		// empty join list
		
	join_count=0;
	join_thread_quit=FALSE;
	
		// table is busy
		
	element_set_table_data(join_table_id,NULL);
	element_table_busy(join_table_id,NULL,0,1);
	
		// table update locks
		
	join_thread_lock=SDL_CreateMutex();

		// start pinging hosts
		
	join_thread_started=TRUE;
	
	if (element_get_value(join_tab_id)==join_pane_lan) {
		join_thread=SDL_CreateThread(join_ping_thread_lan,NULL);
	}
	else {
		join_thread=SDL_CreateThread(join_ping_thread_internet,NULL);
	}
}

void join_ping_thread_end(void)
{
	if (!join_thread_started) return;
	
	join_thread_quit=TRUE;
	
	SDL_WaitThread(join_thread,NULL);
	
	SDL_DestroyMutex(join_thread_lock);
	
	join_thread_started=FALSE;
}

/* =======================================================

      Join Panes
      
======================================================= */

void join_news_pane(void)
{
	int			x,y,wid,high,margin,padding;
	
		// show and hide proper elements
		
	element_hide(join_button_rescan_id,TRUE);
	element_enable(join_button_join_id,FALSE);
	element_text_change(join_status_id,"");

		// news
		
	margin=element_get_tab_margin();
	padding=element_get_padding();
	
	x=margin+padding;
	y=(margin+element_get_tab_control_high())+padding;

	wid=hud.scale_x-((margin+padding)*2);
	high=(int)(((float)hud.scale_y)*0.85f)-y;

	element_text_box_add(net_get_news(),join_news_id,x,y,wid,high);
}

void join_lan_internet_pane(bool lan)
{
	int						x,y,wid,high,margin,padding;
	element_column_type		cols[4];

		// show and hide proper elements
		
	element_hide(join_button_rescan_id,FALSE);
	element_enable(join_button_rescan_id,FALSE);
	element_enable(join_button_join_id,FALSE);
	element_text_change(join_status_id,"");
	
		// hosts table
		
	margin=element_get_tab_margin();
	padding=element_get_padding();
	
	x=margin+padding;
	y=(margin+element_get_tab_control_high())+padding;

	wid=hud.scale_x-((margin+padding)*2);
	high=(int)(((float)hud.scale_y)*0.85f)-y;

	strcpy(cols[0].name,"Name");
	cols[0].percent_size=0.45f;
	strcpy(cols[1].name,"Game");
	cols[1].percent_size=0.37f;
	strcpy(cols[2].name,"Players");
	cols[2].percent_size=0.10f;
	strcpy(cols[3].name,"Ping");
	cols[3].percent_size=0.8f;

	element_table_add(cols,NULL,join_table_id,4,x,y,wid,high,FALSE,element_table_bitmap_data);
	element_table_busy(join_table_id,NULL,0,1);

		// start the thread to build the table
		
	join_ping_thread_start();
}

void join_create_pane(void)
{
	int						x,y,wid,high,tab_idx,pane;
	char					tab_list[][32]={"News","LAN","Internet"};
	
		// turn off any scanning threads
		
	join_ping_thread_end();

		// controls

	element_clear();
	
		// tabs
		
	tab_idx=0;
	if (hud.net_news.host[0]==0x0) {
		tab_idx=1;
		if (join_tab_value==join_pane_news) join_tab_value=join_pane_lan;
	}
	
	element_tab_add((char*)(tab_list[tab_idx]),join_tab_value,join_tab_id,(3-tab_idx));
	
		// buttons
		
	wid=(int)(((float)hud.scale_x)*0.2f);
	high=(int)(((float)hud.scale_x)*0.04f);

	element_get_button_bottom_left(&x,&y,wid,high);
	element_button_text_add("Rescan Hosts",join_button_rescan_id,x,y,wid,high,element_pos_left,element_pos_bottom);
	
	wid=(int)(((float)hud.scale_x)*0.1f);

	element_get_button_bottom_right(&x,&y,wid,high);
	element_button_text_add("Join",join_button_join_id,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(join_button_join_id)-element_get_padding();
	element_button_text_add("Cancel",join_button_cancel_id,x,y,wid,high,element_pos_right,element_pos_bottom);

		// status

	wid=(int)(((float)hud.scale_x)*0.2f);
	x=(element_get_x_position(join_button_rescan_id)+wid)+element_get_padding();
	y-=element_get_padding();
	
	element_text_add("",join_status_id,x,y,hud.font.text_size_small,tx_left,FALSE,FALSE);

		// specific pane controls
		
	pane=element_get_value(join_tab_id);
		
	switch (pane) {
		case join_pane_news:
			join_news_pane();
			break;
		case join_pane_lan:
			join_lan_internet_pane(TRUE);
			break;
		case join_pane_internet:
			join_lan_internet_pane(FALSE);
			break;
	}
}

/* =======================================================

      Build Combined Host List
      
======================================================= */

void join_add_to_host_list(setup_network_hosts_type *hosts)
{
	int			n;

	for (n=0;n!=hosts->count;n++) {
		if (join_combine_host.count>=max_setup_network_host) return;

		memmove(&join_combine_host.hosts[join_combine_host.count],&hosts->hosts[n],sizeof(setup_network_host_type));
		join_combine_host.count++;
	}
}

/* =======================================================

      Join Operations
      
======================================================= */

void join_open(void)
{
		// get the project hash and news

	net_create_project_hash();
	net_load_news();

		// create host list

	join_combine_host.count=0;
	join_add_to_host_list(net_news_get_hosts());
	join_add_to_host_list(&setup.network.host);
	
		// setup gui
		
	gui_initialize(NULL,NULL);
	
		// start with first tab
		
	join_tab_value=join_pane_news;
	join_thread_started=FALSE;
	
		// create panes
		
	join_create_pane();
}

void join_close(void)
{
	join_ping_thread_end();
	gui_shutdown();
}

/* =======================================================

      Enabling
	        
======================================================= */

void join_activity_start(void)
{
		// stop any pinging
		
	join_ping_thread_end();
		
		// disable buttons and redraw
		
	element_enable(join_button_join_id,FALSE);
	element_enable(join_button_cancel_id,FALSE);
	
	gui_draw(1.0f,FALSE);
}

void join_activity_complete(bool single,char *msg)
{
	element_enable(join_button_join_id,single);
	element_enable(join_button_cancel_id,TRUE);
	
	if (!single) element_set_value(join_table_id,-1);
	if (msg!=NULL) element_text_change(join_status_id,msg);
	
	gui_draw(1.0f,FALSE);
}

/* =======================================================

      Join Game
      
======================================================= */

void join_game(void)
{
	int								idx,remote_uid,tick_offset;
	char							deny_reason[64],err_str[256];
	obj_type						*player_obj;
	join_server_info				*info;
	network_reply_join_remote_list	remote_list;

		// get selected host

	idx=element_get_value(join_table_id);
	info=&join_list[idx];

		// reject if server is full

	if (info->player_count>=info->player_max_count) {
		error_setup("Unable to Join Game: Server if Full","Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// get the ul ip address
	
	if (!net_ip_to_address(join_list[idx].ip,&net_setup.client.host_ip_addr,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// setup game type

	net_setup.game_idx=net_client_find_game(info->game_name);
	if (net_setup.game_idx==-1) {
		sprintf(err_str,"Could not find game type: %s",info->game_name);
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,info->map_name);
	
	net_setup.option_flags=info->option_flags;
	
		// start game
	
	if (!game_start(skill_medium,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// start the map
		
	if (!map_start(TRUE,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// get player

	player_obj=server.obj_list.objs[server.player_obj_idx];
							
		// attempt to join

	remote_uid=net_client_join_host_start(player_obj,&tick_offset,deny_reason,&remote_list);
	if (remote_uid==-1) {
		map_end();
		game_end();
		sprintf(err_str,"Unable to Join Game: %s",deny_reason);
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// start client network thread
		
	if (!net_client_start_message_queue(err_str)) {
		net_client_send_leave_host(server.obj_list.objs[server.player_obj_idx]);
		net_client_join_host_end();
		map_end();
		game_end();
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// add in any remotes

	game_remotes_create(&remote_list);

		// mark player as ready to receive data from host

	net_client_send_ready(player_obj);

		// request moving group synchs

	net_client_request_group_synch_ping(player_obj);
	
		// game is running
	
	server.next_state=gs_running;
}

/* =======================================================

      Join Input
      
======================================================= */

void join_click(void)
{
	int			id;
	bool		enable;
	
		// is element being clicked
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run selection

	switch (id) {
			
		case join_tab_id:
			join_tab_value=element_get_value(join_tab_id);
			join_create_pane();
			return;
	
		case join_button_join_id:
			join_game();
			break;
			
		case join_button_cancel_id:
			server.next_state=gs_intro;
			break;
			
		case join_button_rescan_id:
			element_enable(join_button_rescan_id,FALSE);
			join_ping_thread_end();
			join_ping_thread_start();
			break;

		case join_table_id:
			enable=(element_get_value(join_table_id)!=-1);
			element_enable(join_button_join_id,enable);
			break;
	}
}
	
/* =======================================================

      Run Join
      
======================================================= */

void join_run(void)
{
	gui_draw(1.0f,TRUE);
	join_click();
}

