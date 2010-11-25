/**
 * File: connection.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef CONNECTION_H__
#define CONNECTION_H__
#define __STDC_LIMIT_MACROS

#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <string>
#include <sstream>
#include <arpa/inet.h>

#include "ruby.h"

/* Changelog

V 00.00.01, 2010.02.28
	- first version by Dominik Schatzmann
*/
#ifdef WORDS_BIGENDIAN
#	define ntohll(n)	((n))
#else
#	define ntohll(n)	((((uint64_t)ntohl(n)) << 32) + ntohl((n) >> 32))
#endif

#define CONNECTION_VERSION "00.00.01"
#define CONNECTION_VERSION_NBR 000001

#define CONNECTION_ADDR_LENGTH_MAX 16
#define CONNECTION_ADDR_IMPORT_IPV4(ip_to,ip_from) memcpy(ip_to,ip_from,4); memset(ip_to+4,0,12)
#define CONNECTION_ADDR_IMPORT_IPV6(ip_to,ip_from) memcpy(ip_to,ip_from,16)

#define CONNECTION_DIRECTION_UNKNOWN 0
#define CONNECTION_DIRECTION_IN_IN   1
#define CONNECTION_DIRECTION_IN_OUT  2
#define CONNECTION_DIRECTION_OUT_IN  3
#define CONNECTION_DIRECTION_OUT_OUT 4

#define CONNECTION_BORDER_UNKNOWN 0
#define CONNECTION_BORDER_TRUE    1
#define CONNECTION_BORDER_FALSE   2

//------------------------------------------------------------------------------
// Flow Formats
//------------------------------------------------------------------------------
#define CONNECTION_FF_M_CONNECTIONS_TYPE_ID 1
#define CONNECTION_FF_M_CONNECTIONS_BYTES 38

#define CONNECTION_FF_M_DATACUBE_TYPE_ID 1
#define CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV4 54
#define CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV6 78
#define CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_DELTA 24
#define CONNECTION_FF_M_DATACUBE_ADDR_TYPE_POS 45

using namespace std;

class eConnection_Unknown_Addr_Type: public exception
{
	virtual const char* what() const throw()
	{
		return "Unknown address type";
	};
};

class Connection
{
	public:

		// is this connection a valid data structure
		// instead of delete the data structure, just mark it as invalid
		// and reuse it later ...
		bool valid; 

		// (ip) addr of the hosts (network byte order)
		char addr_src[CONNECTION_ADDR_LENGTH_MAX];
		char addr_dst[CONNECTION_ADDR_LENGTH_MAX];
		uint8_t addr_length;
		
		// protocol
		uint8_t protocol;
		uint16_t port_src;
		uint16_t port_dst;

		// timing
		uint32_t start_s;
		uint32_t stop_s;

		// router
		char addr_next[CONNECTION_ADDR_LENGTH_MAX];
		uint8_t router;
		uint16_t if_in;
		uint16_t if_out;

		// network
		uint8_t direction;
		uint8_t border;

		// content
		uint32_t packets;
		uint32_t bytes;

//------------------------------------------------------------------------------
		Connection();
		~Connection();
//------------------------------------------------------------------------------
	int import_from_m_data_cube(char * b);
	int import_from_nfdump_data(int proto, char addrsrc, int portsrc, char addrdst, int portdst, int ts, int te, char nh, int intface_in, int intface_out, int pck, int byts);

	static void to_s(string& buf, const char* ip_, uint8_t addr_length_) 
	{
		char tmp_ip[INET6_ADDRSTRLEN];

		if(addr_length_ == 4)
			inet_ntop(AF_INET, ip_, tmp_ip, INET6_ADDRSTRLEN);
		else if(addr_length_ == 16)
			inet_ntop(AF_INET6, ip_, tmp_ip, INET6_ADDRSTRLEN);
		else
			strcpy(tmp_ip, "Not Def");

		buf = tmp_ip;
		return;
	};
};


//******************************************************************************
// RUBY
//******************************************************************************
#define RB_CONNECTION_UNWRAP Connection* con; Data_Get_Struct(self, Connection, con);
void rb_connection_free(void *p);
VALUE rb_connection_alloc(VALUE klass);

/*
VALUE rb_setter_addr_src(VALUE self, VALUE addr_src);
VALUE rb_setter_addr_dst(VALUE self, VALUE addr_dst);
VALUE rb_setter_addr_next(VALUE self, VALUE addr_next);
VALUE rb_setter_port_src(VALUE self, VALUE port_src_hn);
VALUE rb_setter_port_dst(VALUE self, VALUE port_dst_hn);
VALUE rb_setter_protocol(VALUE self, VALUE protocol);
VALUE rb_setter_start_s(VALUE self, VALUE start_s);
VALUE rb_setter_stop_s(VALUE self, VALUE stop_s);
VALUE rb_setter_packets(VALUE self, VALUE packets);
VALUE rb_setter_bytes(VALUE self, VALUE bytes);
VALUE rb_setter_router(VALUE self, VALUE router);
VALUE rb_setter_if_in(VALUE self, VALUE if_in);
VALUE rb_setter_if_out(VALUE self, VALUE if_out);
VALUE rb_setter_direction(VALUE self, VALUE in_out);
VALUE rb_setter_border(VALUE self, VALUE border);
*/
VALUE rb_getter_valid(VALUE self);
VALUE rb_getter_addr_src_s(VALUE self);
VALUE rb_getter_addr_dst_s(VALUE self);
VALUE rb_getter_addr_next_s(VALUE self);
VALUE rb_getter_port_src(VALUE self);
VALUE rb_getter_port_dst(VALUE self);
VALUE rb_getter_protocol(VALUE self);
VALUE rb_getter_start_s(VALUE self);
VALUE rb_getter_stop_s(VALUE self);
VALUE rb_getter_bytes(VALUE self);
VALUE rb_getter_packets(VALUE self);
VALUE rb_getter_router(VALUE self);
VALUE rb_getter_if_in(VALUE self);
VALUE rb_getter_if_out(VALUE self);
VALUE rb_getter_direction(VALUE self);
VALUE rb_getter_border(VALUE self);
VALUE rb_getter_addr_length(VALUE self);


//------------------------------------------------------------------------------
// IMPORT
//------------------------------------------------------------------------------
VALUE rb_nf_import(
	VALUE self,
	VALUE proto, 
	VALUE addrsrc,
	VALUE portsrc,
	VALUE addrdst,
	VALUE portdst,
	VALUE ts,
	VALUE te,
	VALUE addr_router,
	VALUE intface_in,
	VALUE intface_out,
	VALUE pck,
	VALUE byts);

#endif // CONNECTION_H__
