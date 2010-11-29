/**
 * File: Nfdata_Parser.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */

#ifndef Nfdata_Parser_H__
#define Nfdata_Parser_H__
#define __STDC_LIMIT_MACROS



#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <string>
#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

#include "ruby.h"
#include "connections.h"

/* Changelog

V 00.00.01, 2010.02.28
	- first version by Dominik Schatzmann
*/

#define DATA_PARSER_VERSION "00.00.01"
#define DATA_PARSER_VERSION_NBR 000001

using namespace std;
class Data_Parser
{
	public:
	// Statistics
	uint32_t time_s;
	uint32_t stat_next_export_s;
	uint32_t stat_connections_processed;

//------------------------------------------------------------------------------
	Data_Parser();
	~Data_Parser();

	void stat_reset(void);
};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_DATA_PARSER_UNWRAP Data_Parser* dp; Data_Get_Struct(self, Data_Parser, dp);
void rb_data_parser_free(void *p);
VALUE rb_data_parser_alloc(VALUE klass);

//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_data_parser_stat_get(VALUE self);
VALUE rb_data_parser_stat_reset(VALUE self);
VALUE rb_data_parser_stat_next_export_s(VALUE self, VALUE next_export_s);

//------------------------------------------------------------------------------
// data parser
//------------------------------------------------------------------------------
VALUE rb_data_parser_parse_file(
	VALUE self, 
	VALUE file_p,
	VALUE con_block
);
VALUE rb_data_parse_nf_parse_file_csv(
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
	VALUE byts,
	VALUE con_block
);

#endif // DATA_PARSER_H__