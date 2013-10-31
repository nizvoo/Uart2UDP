/*
  Copyright (C) 2000-2013
  nizvoo (AT) gmail.com
*/


#ifndef __NET_UTILS_H__
#define __NET_UTILS_H__

#ifdef __cplusplus
extern "C"
{
#endif

struct net_t {
	SOCKET socket;
	struct sockaddr_in sockaddr;
};

BOOL BuildServerSocket(unsigned short port, BOOL block, struct net_t* net);
BOOL BuildClientSocket(const char* ip, unsigned short port, BOOL block, struct net_t*);
BOOL FreeClientSocket(struct net_t*);

BOOL InitSocket();
void UninitSocket();

int net_sendto(const struct net_t* net, const char* buf, int len);

#ifdef __cplusplus
}
#endif

#endif