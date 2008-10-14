/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Host Client Message Handler Threads

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#ifdef D3_OS_MAC
	#include <ifaddrs.h>
#endif

#include "network.h"

extern int					net_host_player_count;
extern network_setup_type	net_setup;

/* =======================================================

      Host Client Networking Message Handlers
      
======================================================= */

void net_host_client_handle_info(int sock)
{
	network_reply_info		info;
	
	info.player_count=htons((short)net_host_player_count);
	info.player_max_count=htons((short)host_max_remote_count);
	strcpy(info.host_name,net_setup.host.name);
	strcpy(info.host_ip_resolve,net_setup.host.ip_resolve);
	strcpy(info.proj_name,net_setup.host.proj_name);
	strcpy(info.game_name,net_setup.host.game_name);
	strcpy(info.map_name,net_setup.host.map_name);

	network_send_packet(sock,net_action_reply_info,net_queue_mode_normal,net_remote_uid_host,(unsigned char*)&info,sizeof(network_reply_info));
}

int net_host_client_handle_join(int sock,network_request_join *request_join)
{
	int							remote_uid;
	network_reply_join			reply_join;
	network_request_remote_add	remote_add;

		// if correct version, add player to host

	if (strncmp(request_join->vers,dim3_version,name_str_len)==0) {
		remote_uid=net_host_player_join(sock,request_join->name,reply_join.deny_reason);
	}
	else {
		remote_uid=-1;
		sprintf(reply_join.deny_reason,"Client version (%s) differs from Host version (%s)",request_join->vers,dim3_version);
	}

		// construct the reply
	
	strcpy(reply_join.game_name,net_setup.host.game_name);
	strcpy(reply_join.map_name,net_setup.host.map_name);
	reply_join.join_uid=htons((short)remote_uid);
	
	if (remote_uid!=-1) {
		reply_join.remote_count=htons((short)net_host_player_create_remote_add_list(remote_uid,reply_join.remotes));
	}
	else {
		reply_join.remote_count=0;
	}
	
		// send reply
	
	network_send_packet(sock,net_action_reply_join,net_queue_mode_normal,net_remote_uid_host,(unsigned char*)&reply_join,sizeof(network_reply_join));
	
		// send all other players on host the new player for remote add
		
	if (remote_uid!=-1) {
		remote_add.uid=htons((short)remote_uid);
		strncpy(remote_add.name,request_join->name,name_str_len);
		remote_add.name[name_str_len-1]=0x0;
		remote_add.team_idx=htons((short)net_team_none);
		remote_add.score=0;
		remote_add.pnt_x=remote_add.pnt_y=remote_add.pnt_z=0;
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_add,net_queue_mode_normal,(unsigned char*)&remote_add,sizeof(network_request_remote_add),FALSE);
	}

	return(remote_uid);
}

int net_host_client_handle_local_join(network_request_join *request_join,char *err_str)
{
	int							remote_uid;
	network_request_remote_add	remote_add;

		// join directly to host

	remote_uid=net_host_player_join((d3socket)NULL,request_join->name,err_str);
	if (remote_uid==-1) return(-1);

		// send all other players on host the new player for remote add
		
	strncpy(remote_add.name,request_join->name,name_str_len);
	remote_add.name[name_str_len-1]=0x0;
	remote_add.team_idx=htons((short)net_team_none);
	remote_add.score=0;
	remote_add.pnt_x=remote_add.pnt_y=remote_add.pnt_z=0;
	net_host_player_send_others_packet(remote_uid,net_action_request_remote_add,net_queue_mode_normal,(unsigned char*)&remote_add,sizeof(network_request_remote_add),FALSE);

	return(remote_uid);
}

void net_host_client_handle_leave(int remote_uid)
{
		// leave the host
		
	net_host_player_leave(remote_uid);
	
		// now send all other players on host the remote remove
		
	net_host_player_send_others_packet(remote_uid,net_action_request_remote_remove,net_queue_mode_normal,NULL,0,FALSE);
}

void net_host_client_handle_set_team(int remote_uid,network_request_team *team)
{
	net_host_player_update_team(remote_uid,team);
	net_host_player_send_others_packet(remote_uid,net_action_request_team,net_queue_mode_normal,(unsigned char*)team,sizeof(network_request_team),FALSE);
}

void net_host_client_handle_update(int remote_uid,network_request_remote_update *update)
{
	net_host_player_update(remote_uid,update);
	net_host_player_send_others_packet(remote_uid,net_action_request_remote_update,net_queue_mode_replace,(unsigned char*)update,sizeof(network_request_remote_update),TRUE);
}

/* =======================================================

      Host Client Networking Message Thread
      
======================================================= */

void* net_host_client_handler_thread(void *arg)
{
	d3socket				sock;
	int						client_remote_uid,action,queue_mode,from_remote_uid,len,
							net_error_count;
	unsigned char			data[net_max_msg_size];
	
		// get sock from argument
		
	sock=(d3socket)arg;
	
		// no attached player until join request
		
	client_remote_uid=-1;
	
	net_error_count=0;
	
		// wait for messages
		
	while (TRUE) {
	
			// any messages?
			
		if (!network_receive_ready(sock)) {
			usleep(host_no_data_u_wait);
			continue;
		}
		
			// read the message
			// if error reach past error count, then assume socket has closed on other
			// end or some other fatal error and kick off user
			
		if (!network_receive_packet(sock,&action,&queue_mode,&from_remote_uid,data,&len)) {
			net_error_count++;
			if (net_error_count==host_max_network_error_reject) {
				if (client_remote_uid!=-1) net_host_client_handle_leave(client_remote_uid);
				client_remote_uid=-1;
				break;
			}
			continue;
		}

		net_error_count=0;
		
			// route messages

		switch (action) {
		
			case net_action_request_info:
				net_host_client_handle_info(sock);
				break;
				
			case net_action_request_join:
				client_remote_uid=net_host_client_handle_join(sock,(network_request_join*)data);
				break;

			case net_action_request_ready:
				net_host_player_ready(client_remote_uid,TRUE);
				break;
				
			case net_action_request_team:
				net_host_client_handle_set_team(client_remote_uid,(network_request_team*)data);
				break;
				
			case net_action_request_leave:
				net_host_client_handle_leave(client_remote_uid);
				client_remote_uid=-1;
				break;
				
			case net_action_request_remote_update:
				net_host_player_update(client_remote_uid,(network_request_remote_update*)data);
				net_host_player_send_others_packet(client_remote_uid,action,queue_mode,data,len,TRUE);
				break;
				
			case net_action_request_remote_death:
			case net_action_request_remote_chat:
			case net_action_request_remote_sound:
			case net_action_request_remote_fire:
				net_host_player_send_others_packet(client_remote_uid,action,net_queue_mode_normal,data,len,FALSE);
				break;

			case net_action_request_latency_ping:
				network_send_packet(sock,action,net_queue_mode_normal,net_remote_uid_host,NULL,0);
				break;
				
		}
		
			// if no player attached, close socket and exit thread
			
		if (client_remote_uid==-1) break;
	}
	
	network_close_socket(&sock);
	
	pthread_exit(NULL);
	return(NULL);
}


