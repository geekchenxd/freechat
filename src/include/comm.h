#ifndef __COMMON_H_
#define __COMMON_H_

enum node_type {
	NODE_SERVER = 0,
	NODE_CLIENT
};

struct net {
	char serverip[16];
	char localip[16];
	uint16_t sport;
	uint16_t dport;
	uint16_t servport;
};

struct global_info {
	enum node_type type;
	struct net net;
	struct info info;
	int status;
};

#endif
