/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network Socket Code

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

/* =======================================================

      Socket Open/Close
      
======================================================= */

d3socket net_open_tcp_socket(void)
{
	d3socket		sock;
#ifndef D3_OS_WINDOWS
	int				val;
#else
	BOOL			val;
#endif
	
		// get socket
		
	sock=socket(AF_INET,SOCK_STREAM,0);
	if (sock==D3_NULL_SOCKET) return(D3_NULL_SOCKET);
	
		// turn off nagel algorithm
	
#ifndef D3_OS_WINDOWS
	val=1;
	setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,&val,sizeof(int));
#else
	val=TRUE;
	setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(const char*)&val,sizeof(BOOL));
#endif

	return(sock);
}

d3socket net_open_udp_socket(void)
{
	return(socket(AF_INET,SOCK_DGRAM,0));
}

void net_close_socket(d3socket *sock)
{
		// shutdown any more sending or receiving

#ifndef D3_OS_WINDOWS
	shutdown(*sock,SHUT_RDWR);
#else
	shutdown(*sock,SD_BOTH);
#endif

		// close socket

	closesocket(*sock);
	
	*sock=D3_NULL_SOCKET;
}

/* =======================================================

      Socket Options
      
======================================================= */

void net_socket_blocking(d3socket sock,bool blocking)
{
#ifndef D3_OS_WINDOWS

	long		opt;
	
	opt=fcntl(sock,F_GETFL,NULL);
	
	if (blocking) {
		opt&=(~O_NONBLOCK);
	}
	else {
		opt|=O_NONBLOCK;
	}
	
	fcntl(sock,F_SETFL,opt);

#else

	u_long		opt;

	opt=blocking?1:0;
	ioctlsocket(sock,FIONBIO,&opt);

#endif
}

void net_socket_enable_broadcast(d3socket sock)
{
	int				val;

#ifndef D3_OS_WINDOWS
	val=1;
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&val,sizeof(int));
#else
	val=TRUE;
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(const char*)&val,sizeof(BOOL));
#endif
}

/* =======================================================

      Network Connections
      
======================================================= */

bool net_connect(d3socket sock,char *ip,int port,int secs,char *err_str)
{
	int					err,count;
	unsigned long		ns_addr;
	bool				in_progress,connect_ok;
	socklen_t			len;
	struct sockaddr		name;
	struct sockaddr_in	addr;
	
		// setup host
		
	ns_addr=inet_addr(ip);
	if (ns_addr==INADDR_NONE) {
		sprintf(err_str,"Networking: Could not create address for %s",ip);
		return(FALSE);
	}

	memset(&addr,0x0,sizeof(struct sockaddr_in));
		
	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)port);
	addr.sin_addr.s_addr=ns_addr;
	
		// put socket into non-blocking
		
	net_socket_blocking(sock,FALSE);
	
		// try to connect
	
	err=connect(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	if (err<0) {

#ifndef D3_OS_WINDOWS
		in_progress=(errno==EINPROGRESS);
#else
		in_progress=(WSAGetLastError()==WSAEINPROGRESS);
#endif

		if (!in_progress) {
			net_socket_blocking(sock,TRUE);
			sprintf(err_str,"Networking: Could not connect to %s:%d",ip,port);
			return(FALSE);
		}
	}
	
		// we figure out if we've connected
		// when we can get the peer name

	count=secs*1000;
	connect_ok=FALSE;
	
	while (count>0) {
	
		len=sizeof(name);
		if (getpeername(sock,&name,&len)==0) {
			connect_ok=TRUE;
			break;
		}
	
		usleep(1000);
		count--;
	}

		// put socket back into blocking mode

	net_socket_blocking(sock,TRUE);
	
		// return connection state

	if (!connect_ok) {
		sprintf(err_str,"Networking: No connection to %s:%d",ip,port);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Network Binds
      
======================================================= */

bool net_bind(d3socket sock,char *ip,int port,char *err_str)
{
	int					err;
	unsigned long		ns_addr;
	struct sockaddr_in	addr;
	
		// setup address
		
	ns_addr=inet_addr(ip);
	if (ns_addr==INADDR_NONE) {
		sprintf(err_str,"Networking: Could not create address for %s",ip);
		return(FALSE);
	}
		
	memset(&addr,0x0,sizeof(struct sockaddr_in));
		
	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)port);
	addr.sin_addr.s_addr=ns_addr;
	
		// bind socket
		
	err=bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	if (err<0) {
		sprintf(err_str,"Networking: Could not bind to %s:%d",ip,port);
		return(FALSE);
	}
	
	return(TRUE);
}

bool net_bind_any(d3socket sock,int port,char *err_str)
{
	int					err;
	struct sockaddr_in	addr;
	
		// setup address
		
	memset(&addr,0x0,sizeof(struct sockaddr_in));
		
	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)port);
	addr.sin_addr.s_addr=INADDR_ANY;
	
		// bind socket
		
	err=bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	if (err<0) {
		sprintf(err_str,"Networking: Could not bind to port INADDR_ANY:%d",port);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Network Status Utilities
      
======================================================= */
	
bool net_receive_ready(d3socket sock)
{
	fd_set					fd;
	struct timeval			timeout;
	
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	
	FD_ZERO(&fd);
	FD_SET(sock,&fd);
	
#ifndef D3_OS_WINDOWS
	select((sock+1),&fd,NULL,NULL,&timeout);
#else
	select(0,&fd,NULL,NULL,&timeout);
#endif
	
	return(FD_ISSET(sock,&fd));
}

bool net_send_ready(d3socket sock)
{
	fd_set					fd;
	struct timeval			timeout;
	
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	
	FD_ZERO(&fd);
	FD_SET(sock,&fd);
	
#ifndef D3_OS_WINDOWS
	select((sock+1),NULL,&fd,NULL,&timeout);
#else
	select(0,NULL,&fd,NULL,&timeout);
#endif

	return(FD_ISSET(sock,&fd));
}

/* =======================================================

      Network Send and Recv Utilities
      
======================================================= */

bool net_recvfrom_mesage(d3socket sock,unsigned long *ip_addr,int *port,int *action,int *net_node_uid,unsigned char *msg,int *msg_len)
{
	int						len;
	unsigned char			data[net_max_msg_size];
	network_header			*head;
	socklen_t				addr_in_len;
	struct sockaddr_in		addr_in;

		// read next datagram
		
	addr_in_len=sizeof(addr_in);
	len=(int)recvfrom(sock,data,net_max_msg_size,0,(struct sockaddr*)&addr_in,&addr_in_len);
	
		// sock has closed
		
	if (len<0) return(FALSE);
	
		// setup return address
		
	if (ip_addr!=NULL) *ip_addr=addr_in.sin_addr.s_addr;
	if (port!=NULL) *port=(int)ntohs(addr_in.sin_port);
	
		// no data?
		
	if (len==0) {
		*action=-1;
		return(TRUE);
	}
	
		// get header and data
		
	head=(network_header*)data;
	*action=head->action;
	*net_node_uid=head->net_node_uid;

	len=head->len;
	if (len>net_max_msg_size) len=net_max_msg_size;
	
	memmove(msg,(data+sizeof(network_header)),len);

	if (msg_len!=NULL) *msg_len=len;

	return(TRUE);
}

bool net_sendto_msg(d3socket sock,unsigned long ip_addr,int port,int action,int net_node_uid,unsigned char *msg,int msg_len)
{
	int						send_sz;
	unsigned char			data[net_max_msg_size];
	network_header			*head;
	struct sockaddr_in		addr_in;
	
		// the header
		
	head=(network_header*)data;

	head->len=htons((short)msg_len);
	head->action=htons((short)action);
	head->net_node_uid=htons((short)net_node_uid);
	
		// the data

	if (msg_len!=0) memmove((data+sizeof(network_header)),data,msg_len);

		// send message
		
	addr_in.sin_family=AF_INET;
	addr_in.sin_port=htons((short)port);
	addr_in.sin_addr.s_addr=ip_addr;
	
	send_sz=sizeof(network_header)+msg_len;
		
	return(sendto(sock,data,send_sz,0,(struct sockaddr*)&addr_in,sizeof(struct sockaddr_in))==send_sz);
}

/* =======================================================

      Network Send Utilities
      
======================================================= */


// supergumba -- delete all this after fixing HTML sender
int net_send_data(d3socket sock,unsigned char *data,int len)
{
	int				sent_len,total_len,retry_count;

	retry_count=0;
	total_len=0;

	while (TRUE) {

			// if not able to send, retry a number of times

		if (!net_send_ready(sock)) {
			retry_count++;
			if (retry_count>socket_no_data_retry) return(total_len);
			usleep(socket_no_data_u_wait);
			continue;
		}
	
			// send the data
			
		sent_len=send(sock,(data+total_len),(len-total_len),0);
		if (sent_len==-1) return(-1);
		
			// add up to next data
		
		total_len+=sent_len;
		if (total_len>=len) return(total_len);
	}
	
	return(total_len);
}

void net_send_message(d3socket sock,int action,int net_node_uid,unsigned char *data,int len)
{
	unsigned char		net_data[net_max_msg_size];
	network_header		*head;

		// header

	head=(network_header*)net_data;

	head->len=htons((short)len);
	head->action=htons((short)action);
	head->net_node_uid=htons((short)net_node_uid);

		// the data

	if (len!=0) memmove((net_data+sizeof(network_header)),data,len);

		// send the data

	net_send_data(sock,net_data,(sizeof(network_header)+len));
}

/* =======================================================

      Network HTTP File
      
======================================================= */

char* net_get_http_file(char *host_name,int port,char *url,char *err_str)
{
	int				len,sent_len,max_len,
					rcv_size,rbyte,content_offset,content_length;
	char			*ip,http[1024],str[256];
	char			*c,*data,*content_data;
	bool			ok;
	struct hostent	*hent;
	d3socket		sock;

		// get IP address
		
	hent=gethostbyname(host_name);
	if (hent==NULL) {
		sprintf(err_str,"Could not resolve host name %s",host_name);
		return(NULL);
	}
	
	ip=inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0]));

		// connect to server

	sock=net_open_tcp_socket();
	if (sock==D3_NULL_SOCKET) {
		strcpy(err_str,"Unable to create socket");
		return(NULL);
	}

	if (!net_connect(sock,ip,port,5,err_str)) {
		net_close_socket(&sock);
		return(NULL);
	}

		// send the get

	strcpy(http,"GET ");
	strcat(http,url);
	strcat(http," HTTP/1.1\r\n");
	strcat(http,"Host: ");
	strcat(http,host_name);
	strcat(http,"\r\n");
	strcat(http,"User-Agent: dim3\r\n");
	strcat(http,"Accept: text/plain\r\n");
	strcat(http,"Connection: close\r\n");
	strcat(http,"\r\n");

	len=strlen(http);

	sent_len=net_send_data(sock,(unsigned char*)http,len);
	if (sent_len!=len) {
		strcpy(err_str,"Unable to retrieve file");
		net_close_socket(&sock);
		return(NULL);
	}

		// retrieve the data

	max_len=10*1024;			// only get up to 10K

	data=malloc(max_len+1);
	if (data==NULL) {
		strcpy(err_str,"Out of memory");
		net_close_socket(&sock);
		return(NULL);
	}

	rcv_size=0;
	ok=FALSE;
	content_offset=content_length=-1;

	while (TRUE) {

		if (!net_receive_ready(sock)) {
			usleep(1000);
			continue;
		}

		rbyte=recv(sock,(data+rcv_size),(max_len-rcv_size),0);
		if (rbyte<=0) break;

		rcv_size+=rbyte;
		if (rcv_size>=max_len) break;

		*(data+rcv_size)=0x0;
		
			// check for 200 OK
		
		if (!ok) {
			c=strchr(data,'\n');
			if (c!=NULL) {
				strncpy(str,data,256);
				str[255]=0x0;
				
				c=strchr(str,'\n');
				if (c!=NULL) {
					*c=0x0;
					if (strstr(str,"200 OK")==NULL) {
						strcpy(err_str,"File not found");
						net_close_socket(&sock);
						return(NULL);
					}
					
					ok=TRUE;
				}
			}
		}
				
			// check for content length

		if (content_offset==-1) {
			c=strstr(data,"\r\n\r\n");
			if (c!=NULL) content_offset=(int)((c+4)-data);
		}

		if ((content_offset!=-1) && (content_length==-1)) {
			c=strstr(data,"Content-Length: ");
			if (c!=NULL) {
				c+=16;
				strncpy(str,c,32);
				str[31]=0x0;
				c=strchr(str,'\r');
				if (c!=NULL) {
					*c=0x0;
					content_length=atoi(str);

						// check for wild content lengths

					if ((content_length<0) || (content_length>(max_len-content_offset))) {
						strcpy(err_str,"Unable to retrieve file");
						net_close_socket(&sock);
						return(NULL);
					}
						
				}
			}
		}

			// are we at content length?

		if ((content_offset!=-1) && (content_length!=-1)) {
			if (rcv_size>=(content_offset+content_length)) break;
		}

		usleep(1000);
	}
	
	*(data+rcv_size)=0x0;

		// did we ever get content?

	if (content_length==-1) {
		strcpy(err_str,"Unable to retrieve file");
		net_close_socket(&sock);
		return(NULL);
	}

		// get the content data

	content_data=malloc(content_length+1);
	if (content_data==NULL) {
		strcpy(err_str,"Unable to retrieve file");
		net_close_socket(&sock);
		return(NULL);
	}

	memmove(content_data,(data+content_offset),content_length);
	content_data[content_length]=0x0;
	
	free(data);

		// close the socket

	net_close_socket(&sock);

	return(content_data);
}
