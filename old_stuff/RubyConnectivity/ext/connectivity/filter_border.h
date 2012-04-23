/**
 * File: filter_border.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef FILTER_BORDER_H__
#define FILTER_BORDER_H__

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

/* Changelog

V 00.00.01, 2010.08.06:
	- first version by Dominik Schatzmann
*/

#define FILTER_BORDER_VERSION "00.00.01"
#define FILTER_BORDER_VERSION_NBR 000001

#define FILTER_BORDER_ROUTER_MAX 10
#define FILTER_BORDER_INTERFACE_MAX 130

#define FILTER_BORDER_TYPE_UNKNOWN  0
#define FILTER_BORDER_TYPE_INTERNET 1
#define FILTER_BORDER_TYPE_CUSTOMER 2
#define FILTER_BORDER_TYPE_BACKBONE 3

using namespace std;
class Filter_Border
{
	public:

	// Statistics
	uint32_t time_s;
	uint32_t stat_next_export_s;
	uint32_t stat_connections_processed;
	uint32_t stat_connections_filtered;

	vector< vector<int> > interfaces;

	Filter_Border();
	~Filter_Border();
	void add_interface(int router, int interface, int what);
	void filter(Connection* con);
	void stat_reset(void);
};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_FILTER_BORDER_UNWRAP Filter_Border* fborder; Data_Get_Struct(self, Filter_Border, fborder);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_filter_border_free(void *p);
VALUE rb_filter_border_alloc(VALUE klass);
//------------------------------------------------------------------------------
// configuration
//------------------------------------------------------------------------------
VALUE rb_filter_border_add_interface(VALUE self, VALUE router, VALUE interface, VALUE what);
//------------------------------------------------------------------------------
// filter
//------------------------------------------------------------------------------
VALUE rb_filter_border_filter(VALUE self, VALUE cons);
//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_filter_border_stat_get(VALUE self);
VALUE rb_filter_border_stat_reset(VALUE self);
VALUE rb_filter_border_stat_next_export_s(VALUE self, VALUE next_export_s);

#endif // FILTER_BORDER_H__
