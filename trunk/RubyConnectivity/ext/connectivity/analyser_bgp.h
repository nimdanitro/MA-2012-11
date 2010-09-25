/**
 * File: analyser_bgp.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef ANALYSER_BGP_H__
#define ANALYSER_BGP_H__

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

#define ANALYSER_BGP_VERSION "00.00.01"
#define ANALYSER_BGP_VERSION_NBR 000001

using namespace std;
using namespace __gnu_cxx;
using namespace boost::filesystem;

//------------------------------------------------------------------------------
// NET NET 
//------------------------------------------------------------------------------
#define ANALYSER_BGP_KEY_NET_NET_LENGTH      35
#define ANALYSER_BGP_KEY_NET_NET_IN           0
#define ANALYSER_BGP_KEY_NET_NET_OUT         16
#define ANALYSER_BGP_KEY_NET_NET_IN_BITS     32
#define ANALYSER_BGP_KEY_NET_NET_OUT_BITS    33
#define ANALYSER_BGP_KEY_NET_NET_ADDR_LENGTH 34

struct Analyser_BGP_Key_Net_Net
{
	public:
	char key[ANALYSER_BGP_KEY_NET_NET_LENGTH];

	Analyser_BGP_Key_Net_Net(){};
	Analyser_BGP_Key_Net_Net(const Analyser_BGP_Key_Net_Net& other)
	{
		memcpy(key, other.key, ANALYSER_BGP_KEY_NET_NET_LENGTH);	
	};
	~Analyser_BGP_Key_Net_Net(){}
	size_t operator()(const Analyser_BGP_Key_Net_Net& key) const {
		return(hashlittle(key.key, ANALYSER_BGP_KEY_NET_NET_LENGTH, 0));
	}
	bool operator()(const Analyser_BGP_Key_Net_Net& k1, const Analyser_BGP_Key_Net_Net& k2) const {
		return(memcmp(k1.key, k2.key, ANALYSER_BGP_KEY_NET_NET_LENGTH) == 0);
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
		memcpy(key + ANALYSER_BGP_KEY_NET_NET_IN, con_key.key+CONNECTION_MATRIX_KEY_IP_IN, 16);
		addr_set_unused_bits_to_zero(key + ANALYSER_BGP_KEY_NET_NET_IN, in_net_length);
		memcpy(key + ANALYSER_BGP_KEY_NET_NET_OUT, con_key.key+CONNECTION_MATRIX_KEY_IP_OUT, 16);
		addr_set_unused_bits_to_zero(key + ANALYSER_BGP_KEY_NET_NET_OUT, out_net_length);
		*((uint8_t *)(key +  ANALYSER_BGP_KEY_NET_NET_IN_BITS)) = in_net_length;
		*((uint8_t *)(key +  ANALYSER_BGP_KEY_NET_NET_OUT_BITS)) = out_net_length;
		memcpy(key + ANALYSER_BGP_KEY_NET_NET_ADDR_LENGTH, con_key.key+CONNECTION_MATRIX_KEY_ADDR_LENGTH, 1);
		
		return;
	};
	void to_s(string& buf) const
	{
		stringstream tmp;
		char addr_in_s[INET6_ADDRSTRLEN];
	 	char addr_out_s[INET6_ADDRSTRLEN];
		if(*( (uint8_t  *) (key + ANALYSER_BGP_KEY_NET_NET_ADDR_LENGTH) ) == 4)
		{
			inet_ntop(AF_INET, key + ANALYSER_BGP_KEY_NET_NET_IN, addr_in_s, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET, key + ANALYSER_BGP_KEY_NET_NET_OUT, addr_out_s, INET6_ADDRSTRLEN);
		}
		else if(*((uint8_t  *)(key + ANALYSER_BGP_KEY_NET_NET_ADDR_LENGTH)) == 16)
		{
			inet_ntop(AF_INET6, key + ANALYSER_BGP_KEY_NET_NET_IN, addr_in_s, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, key + ANALYSER_BGP_KEY_NET_NET_OUT, addr_out_s, INET6_ADDRSTRLEN);
		}
		else
		{
			cout << "Analyser_BGP_Key_Net_Net to_s :: Unknown addr length ..." << endl;
			throw 200; // FIXME
		}
		// 
		tmp << addr_in_s << "/" << (int) *((uint8_t*) (key + ANALYSER_BGP_KEY_NET_NET_IN_BITS));
		tmp << " - " ;
		tmp << addr_out_s << "/" << (int) *((uint8_t*) (key + ANALYSER_BGP_KEY_NET_NET_OUT_BITS));
		buf = tmp.str();
		return;
	}
};
struct Analyser_BGP_Entry_Net_Net
{
	public:
		uint32_t balanced;
		uint32_t unbalanced;

	Analyser_BGP_Entry_Net_Net()
	{
		balanced = 0;
		unbalanced = 0;
	}
};
typedef hash_map<Analyser_BGP_Key_Net_Net, Analyser_BGP_Entry_Net_Net, Analyser_BGP_Key_Net_Net, Analyser_BGP_Key_Net_Net> Analyser_BGP_Net_Net_h;

//------------------------------------------------------------------------------
// NET 
//------------------------------------------------------------------------------
#define ANALYSER_BGP_KEY_NET_LENGTH      18
#define ANALYSER_BGP_KEY_NET_OUT          0
#define ANALYSER_BGP_KEY_NET_OUT_BITS    16
#define ANALYSER_BGP_KEY_NET_ADDR_LENGTH 17

struct Analyser_BGP_Key_Net
{
	public:
	char key[ANALYSER_BGP_KEY_NET_LENGTH];

	Analyser_BGP_Key_Net(){};
	Analyser_BGP_Key_Net(const Analyser_BGP_Key_Net& other)
	{
		memcpy(key, other.key, ANALYSER_BGP_KEY_NET_LENGTH);	
	};
	~Analyser_BGP_Key_Net(){}

	size_t operator()(const Analyser_BGP_Key_Net& key) const {
		return(hashlittle(key.key, ANALYSER_BGP_KEY_NET_LENGTH, 0));
	}
	bool operator()(const Analyser_BGP_Key_Net& k1, const Analyser_BGP_Key_Net& k2) const {
		return(memcmp(k1.key, k2.key, ANALYSER_BGP_KEY_NET_LENGTH) == 0);
	}
	void from(const Analyser_BGP_Key_Net_Net& key_net_net)
	{
		memcpy(key + ANALYSER_BGP_KEY_NET_OUT, key_net_net.key+ANALYSER_BGP_KEY_NET_NET_OUT, 16);
		memcpy(key + ANALYSER_BGP_KEY_NET_OUT_BITS , key_net_net.key+ANALYSER_BGP_KEY_NET_NET_OUT_BITS, 1);
		memcpy(key + ANALYSER_BGP_KEY_NET_ADDR_LENGTH, key_net_net.key+ANALYSER_BGP_KEY_NET_NET_ADDR_LENGTH, 1);
		return;
	};
	void to_s(string& buf) const
	{
		stringstream tmp;
		char addr_out_s[INET6_ADDRSTRLEN];
		if(*( (uint8_t  *) (key + ANALYSER_BGP_KEY_NET_ADDR_LENGTH) ) == 4)
		{
			inet_ntop(AF_INET, key + ANALYSER_BGP_KEY_NET_OUT, addr_out_s, INET6_ADDRSTRLEN);
		}
		else if(*((uint8_t  *)(key + ANALYSER_BGP_KEY_NET_ADDR_LENGTH)) == 16)
		{
			inet_ntop(AF_INET6, key + ANALYSER_BGP_KEY_NET_OUT, addr_out_s, INET6_ADDRSTRLEN);
		}
		else
		{
			cout << "Analyser_BGP_Key_Net to_s :: Unknown addr length ..." << endl;
			throw 200; // FIXME
		}
		// 
		tmp << addr_out_s << "/" << (int) *((uint8_t*) (key + ANALYSER_BGP_KEY_NET_OUT_BITS));
		buf = tmp.str();
		return;
	}
};
struct Analyser_BGP_Entry_Net
{
	public:
		uint32_t balanced;
		uint32_t unbalanced;

	Analyser_BGP_Entry_Net()
	{
		balanced = 0;
		unbalanced = 0;
	};
};

typedef hash_map<Analyser_BGP_Key_Net, Analyser_BGP_Entry_Net, Analyser_BGP_Key_Net, Analyser_BGP_Key_Net> Analyser_BGP_Net_h;

class Analyser_BGP
{
	public:

	// working folder
	path working_folder_p;

	// current time
	uint64_t time_s;

	// Network address aggregation
  // A) CIDR Prefixes having a fixed size
	//      net_length_in = 24 ==> X.X.X.X/24
  // B) BGP Prefixes 
	//      net_length_in = 0 
	int net_length_in;
	int net_length_out;
	Prefix_To_Int_Map prefix_length; // used to lookup the BGP prefix length

	// Signal traffic
	int router;
	int interface;

	// Track weird this:
	// A) unbalanced connections
	//    with more than Bytes per Packet
	path track_weird_connections_folder_p;
	int track_weird_connection_bpp_min;

	Analyser_BGP_Net_Net_h net_net_h;
	Analyser_BGP_Net_h net_h;

	// Statistics
	// connections
	uint32_t stat_5tps_processed;
	uint32_t stat_5tps_balanced;
	uint32_t stat_5tps_unbalanced;
	uint32_t stat_5tps_unbalanced_weird_connection;
	uint32_t stat_5tps_unbalanced_not_signal;

	// net net
	uint32_t stat_net_net_items;
	uint32_t stat_net_net_balanced;
	uint32_t stat_net_net_unbalanced;

	// net
	uint32_t stat_net_items;
	uint32_t stat_net_balanced;
	uint32_t stat_net_unbalanced;

	// net
	Analyser_BGP();
	~Analyser_BGP();

	void set_working_path(string path_s){
		working_folder_p = path_s.c_str();
		create_directory(working_folder_p);
		track_weird_connections_folder_p = working_folder_p / "weird_connections";
		create_directory(track_weird_connections_folder_p);
	};
	void add_prefix(const string& prefix, int length);
	void set_net_in(int what){net_length_in = what;}
	void set_net_out(int what){net_length_out = what;}
	void set_router(int what){router = what;}
	void set_interface(int what){interface = what;}

	void get_net_net_key(
		Analyser_BGP_Key_Net_Net& net_key, 
		const Connection_Matrix_Key& cm_key
	);
	void analyse_con_mat(const Connection_Matrix& con_mat);
	void analyse_net_net(void);
	void analyse_net(void);

	void export_density(string& path);
	void export_unbalanced_prefixes(string& path);

	void analyse(
		const Connection_Matrix& con_mat_, 
		uint64_t time_s_);

	void stat_reset(void);
};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_ANALYSER_BGP_UNWRAP Analyser_BGP* abgp; Data_Get_Struct(self, Analyser_BGP, abgp);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_analyser_bgp_free(void *p);
VALUE rb_analyser_bgp_alloc(VALUE klass);

//------------------------------------------------------------------------------
// configuration
//------------------------------------------------------------------------------
VALUE rb_analyser_bgp_set_working_folder(VALUE self, VALUE path);
VALUE rb_analyser_bgp_set_net_sizes(VALUE self, VALUE in, VALUE out);
VALUE rb_analyser_bgp_add_prefix(VALUE self, VALUE prefix, VALUE length);
VALUE rb_analyser_bgp_set_router(VALUE self, VALUE router);
VALUE rb_analyser_bgp_set_interface(VALUE self, VALUE interface);

//------------------------------------------------------------------------------
// analyser
//------------------------------------------------------------------------------
VALUE rb_analyser_bgp_export_density(VALUE self, VALUE file_p);
VALUE rb_analyser_bgp_export_unbalanced_prefixes(VALUE self, VALUE file_p);
VALUE rb_analyser_bgp_analyse(VALUE self, VALUE con_matrix, VALUE time_s);

//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_analyser_bgp_stat_get(VALUE self);
VALUE rb_analyser_bgp_stat_reset(VALUE self);

#endif // ANALYSER BGP
