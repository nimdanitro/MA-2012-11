/**
 * File: con_matrix.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */

#ifndef CON_MATRIX_H__
#define CON_MATRIX_H__

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

#include "ruby.h"

// input
#include "connections.h"

// data storage
#include "connection_matrix_key.h"
#include "connection_matrix_entry.h"


/* Changelog

V 00.00.01, 2010.02.17:
	- first version by Dominik Schatzmann
V 00.00.01, 2010.08.06:
	- interface changed
*/

#define CONNECTION_MATRIX_VERSION "00.00.01"
#define CONNECTION_MATRIX_VERSION_NBR 000001

using namespace std;
using namespace __gnu_cxx;

typedef hash_map<Connection_Matrix_Key, Connection_Matrix_Entry, Connection_Matrix_Key, Connection_Matrix_Key > Connection_Matrix_HT;
class Connection_Matrix
{
	public:

	Connection_Matrix();
	~Connection_Matrix();

//------------------------------------------------------------------------------
// Cache
//------------------------------------------------------------------------------
	Connection_Matrix_HT connections_h;	 // store  the valid connections

//------------------------------------------------------------------------------
// Periodic Jobs
//------------------------------------------------------------------------------
	uint32_t time_s;
	uint32_t periodic_job_next_s;

//------------------------------------------------------------------------------
// Statistics
//------------------------------------------------------------------------------
	uint32_t stat_connections_processed;
	uint32_t stat_connections_state[CONNECTION_MATRIX_ENTRY_STATES];

	void statistics_reset(void);

//------------------------------------------------------------------------------
// Add Connections
//------------------------------------------------------------------------------
	void add(Connection* con, Connection_Matrix_Key& key);

//------------------------------------------------------------------------------
// Delete Connections
//------------------------------------------------------------------------------
	void prune(uint32_t older_s);

//------------------------------------------------------------------------------
// Analyze Connections
//------------------------------------------------------------------------------
	void analyze(uint32_t i_start_s, uint32_t i_stop_s);
};

//******************************************************************************
// RUBY
//******************************************************************************
#define 	RB_CONNECTION_MATRIX_UNWRAP Connection_Matrix* cm; Data_Get_Struct(self, Connection_Matrix, cm);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_connection_matrix_free(void *p);
VALUE rb_connection_matrix_alloc(VALUE klass);

//------------------------------------------------------------------------------
// add/delete
//------------------------------------------------------------------------------
VALUE rb_connection_matrix_add_connections(VALUE self, VALUE cons);
VALUE rb_connection_matrix_prune(VALUE self, VALUE older_s);

//------------------------------------------------------------------------------
// Analyze Connections
//------------------------------------------------------------------------------
VALUE rb_connection_matrix_analyze(VALUE self, VALUE i_start_s, VALUE i_stop_s);

//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_connection_matrix_stat_get(VALUE self);
VALUE rb_connection_matrix_stat_reset(VALUE self);
VALUE rb_connection_matrix_periodic_job_next_s(VALUE self, VALUE  periodic_job_next_s);

#endif // CONNECTION_MATRIX_H__
