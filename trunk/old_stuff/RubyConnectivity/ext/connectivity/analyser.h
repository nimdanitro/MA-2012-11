/**
 * File: analyser.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef ANALYSER_H__
#define ANALYSER_H__

#define __STDC_LIMIT_MACROS

#include <cstdio>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ext/hash_map>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

#include "ruby.h"

#include "connection_matrix.h"
#include "prefix_to_int_map.h"

/* Changelog

V 00.00.01, 2010.08.06:
	- first version by Dominik Schatzmann
*/

#define ANALYSER_VERSION "00.00.01"
#define ANALYSER_VERSION_NBR 000001

#define ANALYSER_KEY_NET_NET_LENGTH      35
#define ANALYSER_KEY_NET_NET_IN           0
#define ANALYSER_KEY_NET_NET_OUT         16
#define ANALYSER_KEY_NET_NET_IN_BITS     32
#define ANALYSER_KEY_NET_NET_OUT_BITS    33
#define ANALYSER_KEY_NET_NET_ADDR_LENGTH 34

#define ANALYSER_KEY_NET_LENGTH          18
#define ANALYSER_KEY_NET                  0
#define ANALYSER_KEY_NET_BITS            16
#define ANALYSER_KEY_NET_ADDR_LENGTH     17

#define ANALYSER_MAX_ROUTER               7
#define ANALYSER_MAX_INTERFACE          120

#define ANALYSER_BGP_AGGREGATION_UNKNOWN -1
#define ANALYSER_BGP_AGGREGATION_NOT_ANNOUNCED -2

using namespace std;
using namespace __gnu_cxx;
using namespace boost::filesystem;

//------------------------------------------------------------------------------
// NET  -  NET
//------------------------------------------------------------------------------
struct Analyser_Key_Net_Net
{
	public:
	char key[ANALYSER_KEY_NET_NET_LENGTH];

	Analyser_Key_Net_Net(){};
	Analyser_Key_Net_Net(const Analyser_Key_Net_Net& other)
	{
		memcpy(key, other.key, ANALYSER_KEY_NET_NET_LENGTH);	
	};
	~Analyser_Key_Net_Net(){}
	size_t operator()(const Analyser_Key_Net_Net& key) const {
		return(hashlittle(key.key, ANALYSER_KEY_NET_NET_LENGTH, 0));
	}
	bool operator()(const Analyser_Key_Net_Net& k1, const Analyser_Key_Net_Net& k2) const {
		return(memcmp(k1.key, k2.key, ANALYSER_KEY_NET_NET_LENGTH) == 0);
	}
	void addr_set_unused_bits_to_zero(char* addr, int size)
	{
		if(size < 128)
		{
			int byte = size >> 3;   // / 8
			int bits = size & 0x07; // % 8
			uint8_t bit_mask = 255 << (8-bits);
			addr[byte] = addr[byte] & bit_mask;
			for(int i = byte+1 ; i < 16; i++)
				addr[i] = 0;
		}
	}
	void from(
		const Connection_Matrix_Key& con_key, 
		uint8_t in_net_length, 
		uint8_t out_net_length
	)
	{
		memcpy(key + ANALYSER_KEY_NET_NET_IN, con_key.key+CONNECTION_MATRIX_KEY_IP_IN, 16);
		addr_set_unused_bits_to_zero(key + ANALYSER_KEY_NET_NET_IN, in_net_length);
		memcpy(key + ANALYSER_KEY_NET_NET_OUT, con_key.key+CONNECTION_MATRIX_KEY_IP_OUT, 16);
		addr_set_unused_bits_to_zero(key + ANALYSER_KEY_NET_NET_OUT, out_net_length);
		*((uint8_t *)(key +  ANALYSER_KEY_NET_NET_IN_BITS)) = in_net_length;
		*((uint8_t *)(key +  ANALYSER_KEY_NET_NET_OUT_BITS)) = out_net_length;
		memcpy(key + ANALYSER_KEY_NET_NET_ADDR_LENGTH, con_key.key+CONNECTION_MATRIX_KEY_ADDR_LENGTH, 1);
		return;
	};
	void to_s(string& buf) const
	{
		stringstream tmp;
		char addr_in_s[INET6_ADDRSTRLEN];
	 	char addr_out_s[INET6_ADDRSTRLEN];
		if(*( (uint8_t  *) (key + ANALYSER_KEY_NET_NET_ADDR_LENGTH) ) == 4)
		{
			inet_ntop(AF_INET, key + ANALYSER_KEY_NET_NET_IN, addr_in_s, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET, key + ANALYSER_KEY_NET_NET_OUT, addr_out_s, INET6_ADDRSTRLEN);
		}
		else if(*((uint8_t  *)(key + ANALYSER_KEY_NET_NET_ADDR_LENGTH)) == 16)
		{
			inet_ntop(AF_INET6, key + ANALYSER_KEY_NET_NET_IN, addr_in_s, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, key + ANALYSER_KEY_NET_NET_OUT, addr_out_s, INET6_ADDRSTRLEN);
		}
		else
		{
			cerr << "Analyser_Key_Net_Net to_s :: Unknown addr length ..." << endl;
			cerr.flush();
			throw 200; // FIXME
		}
		// 
		tmp << addr_in_s << "/" << (int) *((uint8_t*) (key + ANALYSER_KEY_NET_NET_IN_BITS));
		tmp << " - " ;
		tmp << addr_out_s << "/" << (int) *((uint8_t*) (key + ANALYSER_KEY_NET_NET_OUT_BITS));
		buf = tmp.str();
		return;
	}
};
struct Analyser_Entry_Net_Net
{
	public:
		uint32_t balanced;
		uint32_t unbalanced;

	Analyser_Entry_Net_Net()
	{
		balanced = 0;
		unbalanced = 0;
	}
};
typedef hash_map<Analyser_Key_Net_Net, Analyser_Entry_Net_Net, Analyser_Key_Net_Net, Analyser_Key_Net_Net> Analyser_Net_Net_h;

//------------------------------------------------------------------------------
// NET 
//------------------------------------------------------------------------------
struct Analyser_Key_Net
{
	public:
	char key[ANALYSER_KEY_NET_LENGTH];

	Analyser_Key_Net(){};
	Analyser_Key_Net(const Analyser_Key_Net& other)
	{
		memcpy(key, other.key, ANALYSER_KEY_NET_LENGTH);	
	};
	~Analyser_Key_Net(){}

	size_t operator()(const Analyser_Key_Net& key) const {
		return(hashlittle(key.key, ANALYSER_KEY_NET_LENGTH, 0));
	}
	bool operator()(const Analyser_Key_Net& k1, const Analyser_Key_Net& k2) const {
		return(memcmp(k1.key, k2.key, ANALYSER_KEY_NET_LENGTH) == 0);
	}
	void addr_set_unused_bits_to_zero(char* addr, int size)
	{
		if(size < 128)
		{
			int byte = size >> 3;   // / 8
			int bits = size & 0x07; // % 8
			uint8_t bit_mask = 255 << (8-bits);
			addr[byte] = addr[byte] & bit_mask;
			for(int i = byte+1 ; i < 16; i++)
				addr[i] = 0;
		}
	}
	void from(
		const Analyser_Key_Net_Net& key_net_net,
		uint8_t length
	)
	{
		memcpy(key + ANALYSER_KEY_NET, key_net_net.key+ANALYSER_KEY_NET_NET_OUT, 16);
		addr_set_unused_bits_to_zero(key + ANALYSER_KEY_NET, length);
		memcpy(key + ANALYSER_KEY_NET_BITS, key_net_net.key+ANALYSER_KEY_NET_NET_OUT_BITS, 1);
		memcpy(key + ANALYSER_KEY_NET_ADDR_LENGTH, key_net_net.key+ANALYSER_KEY_NET_NET_ADDR_LENGTH, 1);
		return;
	};
	void from(
		const Analyser_Key_Net& key_net,
		uint8_t length
	)
	{
		memcpy(key, key_net.key, ANALYSER_KEY_NET_LENGTH);
		addr_set_unused_bits_to_zero(key + ANALYSER_KEY_NET, length);
		*((uint8_t *)(key +  ANALYSER_KEY_NET_BITS)) = length;
		memcpy(key + ANALYSER_KEY_NET_ADDR_LENGTH, key_net.key+ANALYSER_KEY_NET_ADDR_LENGTH, 1);
		return;
	};
	void from(
		const Analyser_Key_Net& key_net
	)
	{
		memcpy(key, key_net.key, ANALYSER_KEY_NET_LENGTH);
		return;
	};
	void to_s(string& buf) const
	{
		stringstream tmp;
		char addr_out_s[INET6_ADDRSTRLEN];
		if(*( (uint8_t  *) (key + ANALYSER_KEY_NET_ADDR_LENGTH) ) == 4)
		{
			inet_ntop(AF_INET, key + ANALYSER_KEY_NET, addr_out_s, INET6_ADDRSTRLEN);
		}
		else if(*((uint8_t  *)(key + ANALYSER_KEY_NET_ADDR_LENGTH)) == 16)
		{
			inet_ntop(AF_INET6, key + ANALYSER_KEY_NET, addr_out_s, INET6_ADDRSTRLEN);
		}
		else
		{
			cout << "Analyser_Key_Net to_s :: Unknown addr length ..." << endl;
			throw 200; // FIXME
		}
		// 
		tmp << addr_out_s << "/" << (int) *((uint8_t*) (key + ANALYSER_KEY_NET_BITS));
		buf = tmp.str();
		return;
	}
	const char* addr(void) const 
	{
		return(key + ANALYSER_KEY_NET);
	}
};
struct Analyser_Entry_Net
{
	public:
		uint32_t balanced;
		uint32_t unbalanced;
		int bgp_length;
	Analyser_Entry_Net()
	{
		balanced = 0;
		unbalanced = 0;
		bgp_length = ANALYSER_BGP_AGGREGATION_UNKNOWN;
	};
};

typedef hash_map<Analyser_Key_Net, Analyser_Entry_Net, Analyser_Key_Net, Analyser_Key_Net> Analyser_Net_h;

class Analyser
{
	public:

	// working folder, one for IPv4 and one for IPv6
	path working_p4;
	path working_p6;

	// data aggregation (per interface) IPv4
	Analyser_Net_Net_h* agg_host_host_h4[ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE];
	Analyser_Net_h* agg_host_h4[ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE];
	Analyser_Net_h* agg_net_h4[ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE];
	Analyser_Net_h* agg_bgp_h4[ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE];

	// data aggregation all interface IPv4
	Analyser_Net_h agg_host_all_h4;
	Analyser_Net_h agg_net_all_h4;
	Analyser_Net_h agg_bgp_all_h4;
	
	// data aggregation (per interface) IPv6
	Analyser_Net_Net_h* agg_host_host_h6[ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE];
	Analyser_Net_h* agg_host_h6[ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE];
	Analyser_Net_h* agg_net_h6[ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE];
	Analyser_Net_h* agg_bgp_h6[ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE];

	// data aggregation all interface IPv6
	Analyser_Net_h agg_host_all_h6;
	Analyser_Net_h agg_net_all_h6;
	Analyser_Net_h agg_bgp_all_h6;

	// BGP Prefixes
	Prefix_To_Int_Map bgp_prefixes;
	
	int weird_connection_bpp_min;
	
//IPV4
	// extract weird connections (TCP)
	path weird_connections_folder_p4;

	// STATISTICS
	// connections
	uint64_t stat_cons_processed4;

	// interfaces
	uint64_t stat_cons_if_invalid4;
	uint64_t stat_cons_if_zero4;
	uint64_t stat_cons_if_not_monitored4;
	uint64_t stat_cons_if_monitored4;

	// state
	uint64_t stat_cons_state_processed4;
	uint64_t stat_cons_state_balanced4;
	uint64_t stat_cons_state_unbalanced4;
	uint64_t stat_cons_state_other4;

	// weird connections
	uint64_t stat_cons_weird_cons_processed4;
	uint64_t stat_cons_weird_cons_no_rule4;
	uint64_t stat_cons_weird_cons_zero_packet4;
	uint64_t stat_cons_weird_cons_bpp4;
	uint64_t stat_cons_weird_cons_ok4;

	// signal traffic
	uint64_t stat_cons_signal_processed4;
	uint64_t stat_cons_signal_ok4;
	uint64_t stat_cons_signal_other4;

	// accounted unbalanced traffic
	uint64_t stat_cons_accounted_unbalanced4;

	path stat_host_host_folder_p4;
	path stat_host_folder_p4;
	path stat_net_folder_p4;
	path stat_bgp_folder_p4;
	
	// Prefixes
	path prefixes_host_folder_p4;
	path prefixes_net_folder_p4;
	path prefixes_bgp_folder_p4;

//IPV6
	// extract weird connections (TCP)
	path weird_connections_folder_p6;
	
	// STATISTICS
	// connections
	uint64_t stat_cons_processed6;

	// interfaces
	uint64_t stat_cons_if_invalid6;
	uint64_t stat_cons_if_zero6;
	uint64_t stat_cons_if_not_monitored6;
	uint64_t stat_cons_if_monitored6;

	// state
	uint64_t stat_cons_state_processed6;
	uint64_t stat_cons_state_balanced6;
	uint64_t stat_cons_state_unbalanced6;
	uint64_t stat_cons_state_other6;

	// weird connections
	uint64_t stat_cons_weird_cons_processed6;
	uint64_t stat_cons_weird_cons_no_rule6;
	uint64_t stat_cons_weird_cons_zero_packet6;
	uint64_t stat_cons_weird_cons_bpp6;
	uint64_t stat_cons_weird_cons_ok6;

	// signal traffic
	uint64_t stat_cons_signal_processed6;
	uint64_t stat_cons_signal_ok6;
	uint64_t stat_cons_signal_other6;

	// accounted unbalanced traffic
	uint64_t stat_cons_accounted_unbalanced6;

	path stat_host_host_folder_p6;
	path stat_host_folder_p6;
	path stat_net_folder_p6;
	path stat_bgp_folder_p6;

	// Prefixes
	path prefixes_host_folder_p6;
	path prefixes_net_folder_p6;
	path prefixes_bgp_folder_p6;

// -----------------------------------------------------------------------------
	Analyser();
	~Analyser();

	void reset();

	// configuration
	void set_working_path(string path_s);
	void add_interface(int router, int interface);
	void add_prefix(const string& prefix, int length);

	// analyse
	void analyse(const Connection_Matrix& con_mat, uint64_t time_s);
	void analyse_connections(const Connection_Matrix& con_mat, uint64_t time_s);
	void analyse_host_host(uint64_t time_s);
	void analyse_host(uint64_t time_s);
	void analyse_net(uint64_t time_s);
	void analyse_bgp(uint64_t time_s);
	// logger
	void weird_connection_log (
		int status,
		const Connection_Matrix_Key& key, 
		const Connection_Matrix_Entry& entry,
		ofstream& file
	);
};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_ANALYSER_UNWRAP Analyser* analyser; Data_Get_Struct(self, Analyser, analyser);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_analyser_free(void *p);
VALUE rb_analyser_alloc(VALUE klass);

VALUE rb_analyser_reset(VALUE self);

//------------------------------------------------------------------------------
// configuration
//------------------------------------------------------------------------------
VALUE rb_analyser_set_working_folder(VALUE self, VALUE path);
VALUE rb_analyser_add_interface(VALUE self, VALUE router, VALUE interface);
VALUE rb_analyser_add_prefix(VALUE self, VALUE prefix, VALUE length);

//------------------------------------------------------------------------------
// analyser
//------------------------------------------------------------------------------
VALUE rb_analyser_analyse(VALUE self, VALUE con_matrix, VALUE time_s);

//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_analyser_stat_get(VALUE self);


#endif // ANALYSER

