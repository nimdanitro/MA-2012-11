/**
 * File: filter_ipv6.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef FILTER_IPV6_H__
#define FILTER_IPV6_H__

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

#define FILTER_IPV6_VERSION "00.00.01"
#define FILTER_IPV6_VERSION_NBR 000001

using namespace std;
class Filter_IPv6
{
	public:

	// Statistics
	uint32_t time_s;
	uint32_t stat_next_export_s;
	uint32_t stat_connections_processed;
	uint32_t stat_connections_filtered;

	Filter_IPv6();
	~Filter_IPv6();
	void filter(Connection* con);
	void stat_reset(void);
};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_FILTER_IPV6_UNWRAP Filter_IPv6* fip6; Data_Get_Struct(self, Filter_IPv6, fip6);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_filter_ipv6_free(void *p);
VALUE rb_filter_ipv6_alloc(VALUE klass);

//------------------------------------------------------------------------------
// filter
//------------------------------------------------------------------------------
VALUE rb_filter_ipv6_filter(VALUE self, VALUE cons);
//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_filter_ipv6_stat_get(VALUE self);
VALUE rb_filter_ipv6_stat_reset(VALUE self);
VALUE rb_filter_ipv6_stat_next_export_s(VALUE self, VALUE next_export_s);

#endif // FILTER_IPV6_H__
