/**
 * File: connection_matrix_entry.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */

#ifndef CONNECTION_MATRIX_ENTRY_H__
#define CONNECTION_MATRIX_ENTRY_H__

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
#include <cassert>

#include "ruby.h"
#include "connections.h"

/* Changelog

V 00.00.01, 2010.02.17:
	- first version by Dominik Schatzmann
*/
#define CONNECTION_MATRIX_ENTRY_VERSION "00.00.01"
#define CONNECTION_MATRIX_ENTRY_VERSION_NBR 000001

#define CONNECTION_MATRIX_ENTRY_STATES               17
//                                  IN-OUT :: OUT-IN
#define CONNECTION_MATRIX_ENTRY_STATE_UNKNOWN           0
#define CONNECTION_MATRIX_ENTRY_STATE_EVENT__EVENT      1
#define CONNECTION_MATRIX_ENTRY_STATE_EVENT__SPAN       2
#define CONNECTION_MATRIX_ENTRY_STATE_EVENT__NOT_NOW    3        
#define CONNECTION_MATRIX_ENTRY_STATE_EVENT__NEVER      4
#define CONNECTION_MATRIX_ENTRY_STATE_SPAN__EVENT       5
#define CONNECTION_MATRIX_ENTRY_STATE_SPAN__SPAN        6
#define CONNECTION_MATRIX_ENTRY_STATE_SPAN__NOT_NOW     7        
#define CONNECTION_MATRIX_ENTRY_STATE_SPAN__NEVER       8
#define CONNECTION_MATRIX_ENTRY_STATE_NOT_NOW__EVENT    9
#define CONNECTION_MATRIX_ENTRY_STATE_NOT_NOW__SPAN     10
#define CONNECTION_MATRIX_ENTRY_STATE_NOT_NOW__NOT_NOW  11       
#define CONNECTION_MATRIX_ENTRY_STATE_NOT_NOW__NEVER    12
#define CONNECTION_MATRIX_ENTRY_STATE_NEVER__EVENT      13
#define CONNECTION_MATRIX_ENTRY_STATE_NEVER__SPAN       14
#define CONNECTION_MATRIX_ENTRY_STATE_NEVER__NOT_NOW    15        
#define CONNECTION_MATRIX_ENTRY_STATE_NEVER__NEVER      16

using namespace std;
using namespace __gnu_cxx;

class Connection_Matrix_Entry
{
	public:
	uint8_t state;

	uint32_t in_out_start_s;
	uint32_t in_out_stop_s;
	uint32_t out_in_start_s;
	uint32_t out_in_stop_s;

	uint16_t in_out_if;
	uint16_t out_in_if;
	uint8_t in_out_router;
	uint8_t out_in_router;

	uint32_t in_out_packets;
	uint32_t out_in_packets;
	uint32_t in_out_bytes;
	uint32_t out_in_bytes;

//-----------------------------------------------------------------------------
	Connection_Matrix_Entry();
	~Connection_Matrix_Entry();
//-----------------------------------------------------------------------------
	void add_connection(Connection* con);
	bool prune(uint32_t older_s);
	uint32_t state_within(uint32_t i_start_s, uint32_t i_stop_s);
	void to_s(string& buf) const ;
	void to_s_router(string& buf) const;
};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_CONNECTION_MATRIX_ENTRY_UNWRAP Connection_Matrix_Entry* e; Data_Get_Struct(self, Connection_Matrix_Entry, e);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_connection_matrix_entry_free(void *p);
VALUE rb_connection_matrix_entry_alloc(VALUE klass);

#endif // CONNECTION_MATRIX_ENTRY_H__
