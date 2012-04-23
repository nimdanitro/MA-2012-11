/**
 * File: filter_in_out.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef FILTER_IN_OUT_H__
#define FILTER_IN_OUT_H__

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

#include "connections.h"
#include "prefix_to_int_map.h"

/* Changelog

V 00.00.01, 2010.08.06:
	- first version by Dominik Schatzmann
*/

#define FILTER_IN_OUT_VERSION "00.00.01"
#define FILTER_IN_OUT_VERSION_NBR 000001

using namespace std;
class Filter_In_Out
{
	public:

	Prefix_To_Int_Map prefix_map;

	// Statistics
	uint32_t time_s;
	uint32_t stat_next_export_s;
	uint32_t stat_connections_processed;
	uint32_t stat_connections_filtered;
	uint32_t stat_connections_in_in;
	uint32_t stat_connections_in_out;
	uint32_t stat_connections_out_in;
	uint32_t stat_connections_out_out;

	Filter_In_Out();
	~Filter_In_Out();
	void add_prefix(const string& prefix, int as);
	void filter(Connection* con, Prefix& p);
	void stat_reset(void);
};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_FILTER_IN_OUT_UNWRAP Filter_In_Out* fio; Data_Get_Struct(self, Filter_In_Out, fio);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_filter_in_out_free(void *p);
VALUE rb_filter_in_out_alloc(VALUE klass);

//------------------------------------------------------------------------------
// configuration
//------------------------------------------------------------------------------
VALUE rb_filter_in_out_add_prefix(VALUE self, VALUE prefix, VALUE as);

//------------------------------------------------------------------------------
// filter
//------------------------------------------------------------------------------
VALUE rb_filter_in_out_filter(VALUE self, VALUE cons);

//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_filter_in_out_stat_get(VALUE self);
VALUE rb_filter_in_out_stat_reset(VALUE self);
VALUE rb_filter_in_out_stat_next_export_s(VALUE self, VALUE next_export_s);

#endif // FILTER_IN_OUT_H__
