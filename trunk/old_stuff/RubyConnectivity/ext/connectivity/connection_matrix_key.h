/**
 * File: connection_matrix_key.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */

#ifndef CONNECTION_MATRIX_KEY_H__
#define CONNECTION_MATRIX_KEY_H__

#define __STDC_LIMIT_MACROS
#include "ruby.h"

#include <cassert>

#include "lookup3.h"
#include "connections.h"

/* Changelog

V 00.00.01, 2010.02.17:
	- first version by Dominik Schatzmann
V 00.00.02, 2010.08.08:
	- key changed to in out by Dominik Schatzmann
	- ipv6 support prepared
*/
#define CONNECTION_MATRIX_KEY_VERSION "00.00.01"
#define CONNECTION_MATRIX_KEY_VERSION_NBR 000001

#define CONNECTION_MATRIX_KEY_LENGTH       38

#define CONNECTION_MATRIX_KEY_IP_IN         0
#define CONNECTION_MATRIX_KEY_PORT_IN      16
#define CONNECTION_MATRIX_KEY_IP_OUT       18
#define CONNECTION_MATRIX_KEY_PORT_OUT     34
#define CONNECTION_MATRIX_KEY_PROTOCOL     36
#define CONNECTION_MATRIX_KEY_ADDR_LENGTH  37

using namespace std;
using namespace __gnu_cxx;

struct Connection_Matrix_Key
{
	public:
	char key[CONNECTION_MATRIX_KEY_LENGTH];

	Connection_Matrix_Key(){};
	Connection_Matrix_Key(const Connection_Matrix_Key& other)
	{
		memcpy(key, other.key, CONNECTION_MATRIX_KEY_LENGTH);	
	};
	~Connection_Matrix_Key(){}
	size_t operator()(const Connection_Matrix_Key& key) const {
		return(hashlittle(key.key, CONNECTION_MATRIX_KEY_LENGTH, 0));
	}
	bool operator()(const Connection_Matrix_Key& k1, const Connection_Matrix_Key& k2) const {
		return(memcmp(k1.key, k2.key, CONNECTION_MATRIX_KEY_LENGTH) == 0);
	}
	void from(const Connection& con)
	{
		if(con.direction == CONNECTION_DIRECTION_IN_OUT)
		{
			memcpy(key + CONNECTION_MATRIX_KEY_IP_IN, con.addr_src, CONNECTION_ADDR_LENGTH_MAX);
			memcpy(key + CONNECTION_MATRIX_KEY_IP_OUT, con.addr_dst, CONNECTION_ADDR_LENGTH_MAX);

			// L4 ports are only well definend for TCP and UDP
			if(con.protocol == 6 or con.protocol == 17)
			{
				*((uint16_t *)(key +  CONNECTION_MATRIX_KEY_PORT_IN)) = con.port_src;
				*((uint16_t *)(key +  CONNECTION_MATRIX_KEY_PORT_OUT)) = con.port_dst;
			}
			else
			{
				*((uint16_t *)(key +  CONNECTION_MATRIX_KEY_PORT_IN))  = 0;
				*((uint16_t *)(key +  CONNECTION_MATRIX_KEY_PORT_OUT)) = 0;
			}
		}
		else if (con.direction == CONNECTION_DIRECTION_OUT_IN)
		{
			memcpy(key + CONNECTION_MATRIX_KEY_IP_IN, con.addr_dst, CONNECTION_ADDR_LENGTH_MAX);
			memcpy(key + CONNECTION_MATRIX_KEY_IP_OUT, con.addr_src, CONNECTION_ADDR_LENGTH_MAX);

			// L4 ports are only well definend for TCP and UDP
			if(con.protocol == 6 or con.protocol == 17)
			{
				*((uint16_t *)(key +  CONNECTION_MATRIX_KEY_PORT_IN)) = con.port_dst;
				*((uint16_t *)(key +  CONNECTION_MATRIX_KEY_PORT_OUT)) = con.port_src;
			}
			else
			{
				*((uint16_t *)(key +  CONNECTION_MATRIX_KEY_PORT_IN))  = 0;
				*((uint16_t *)(key +  CONNECTION_MATRIX_KEY_PORT_OUT)) = 0;
			}
		}
		else
		{
			cerr << "Connection Matrix Key From :: Accept only IN-OUT or OUT-IN Traffic" << endl;
			cerr << "DIRECTION :: "  << (int) con.direction << endl;
			cerr.flush();
			throw 200; // FIXME
		}

		*((uint8_t  *)(key + CONNECTION_MATRIX_KEY_PROTOCOL)) = con.protocol;
		*((uint8_t  *)(key + CONNECTION_MATRIX_KEY_ADDR_LENGTH)) = con.addr_length;

		return;
	};
	const char* addr_in(void) const 
	{
		return(key + CONNECTION_MATRIX_KEY_IP_IN);
	}
	const char* addr_out(void) const 
	{
		return(key + CONNECTION_MATRIX_KEY_IP_OUT);
	}
	uint8_t addr_length(void) const 
	{
		return(*((uint8_t*) (key + CONNECTION_MATRIX_KEY_ADDR_LENGTH)));
	}
	uint16_t port_in(void) const 
	{
		return(*((uint16_t*) (key + CONNECTION_MATRIX_KEY_PORT_IN)));
	}
	uint16_t port_out(void) const 
	{
		return(*((uint16_t*) (key + CONNECTION_MATRIX_KEY_PORT_OUT)));
	}
	uint16_t protocol(void) const 
	{
		return(*((uint8_t*) (key + CONNECTION_MATRIX_KEY_PROTOCOL)));
	}

	void to_s(string& buf) const
	{
		stringstream tmp;
		char addr_in_s[INET6_ADDRSTRLEN];
	 	char addr_out_s[INET6_ADDRSTRLEN];
		if(*( (uint8_t  *) (key + CONNECTION_MATRIX_KEY_ADDR_LENGTH) ) == 4)
		{
			inet_ntop(AF_INET, key + CONNECTION_MATRIX_KEY_IP_IN, addr_in_s, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET, key + CONNECTION_MATRIX_KEY_IP_OUT, addr_out_s, INET6_ADDRSTRLEN);
		}
		else if(*((uint8_t  *)(key + CONNECTION_MATRIX_KEY_ADDR_LENGTH)) == 16)
		{
			inet_ntop(AF_INET6, key + CONNECTION_MATRIX_KEY_IP_IN, addr_in_s, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, key + CONNECTION_MATRIX_KEY_IP_OUT, addr_out_s, INET6_ADDRSTRLEN);
		}
		else
		{
			cerr << "Connection Matrix Key to_s :: Unknown addr length ..." << endl;
			cerr.flush();
			throw 200; // FIXME
		}
		// 
		tmp << addr_in_s << ", ";
		tmp << (int) port_in() << ", ";
		tmp << addr_out_s << ", ";
		tmp << (int) port_out() << ", ";
		tmp << (int) protocol();
		buf = tmp.str();
		return;
	}

};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_CONNECTION_MATRIX_KEY_UNWRAP Connection_Matrix_Key* k; Data_Get_Struct(key, Connection_Matrix_Key, k);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_connection_matrix_key_free(void *p);
VALUE rb_connection_matrix_key_alloc(VALUE klass);

#endif // CONNECTION_MATRIX_KEY_H__
