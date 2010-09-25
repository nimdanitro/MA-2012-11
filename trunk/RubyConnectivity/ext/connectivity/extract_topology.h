/**
 * File: extract_topology.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef EXTRACT_TOPOLOGY_H__
#define EXTRACT_TOPOLOGY_H__

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
#include <map>

#include "ruby.h"
#include "connections.h"

/* Changelog

V 00.00.01, 2010.09.06:
	- first version by Dominik Schatzmann
*/

#define EXTRACT_TOPOLOGY_VERSION "00.00.01"
#define EXTRACT_TOPOLOGY_VERSION_NBR 000001

#define EXTRACT_TOPOLOGY_ROUTER_MAX 10
#define EXTRACT_TOPOLOGY_INTERFACE_MAX 130
#define EXTRACT_TOPOLOGY_ROUTER_INTERFACES_MAX 500

using namespace std;
class Extract_Topology
{
	class RouterInterface
	{
		public:
			uint32_t router;
			uint32_t iterf;

			// statistics
			// number of flows received  by router 'router' over interface 'iterf'
			// since we don't have a 'from ip' field we can't say anything about hop befor
			uint64_t flows_in;
			// number of flows forwarded by router 'router' over interface 'iterf' toward the 'next hop ip'
			map<uint32_t, uint64_t> flows_out;

			RouterInterface()
			{
				router = 0;
				iterf = 0;
				flows_in = 0;
				flows_out.clear();
			};
			void reset()
			{
				flows_in = 0;
				flows_out.clear();
			}
			void add_in()
			{
				flows_in += 1;
			};
			void add_out(const char* addr_next)
			{
				flows_out[*((uint32_t *) addr_next)] += 1;
			};
			void to_s(string & buf) const
			{
				stringstream tmp;

				// in traffic				
				tmp << "# IN: " << router << ", " << iterf << ", " << flows_in << endl;
				map<uint32_t, uint64_t>::const_iterator iter;
				iter = flows_out.begin();
				while(iter != flows_out.end())
				{
					string buf;
					Connection::to_s(buf, (const char*) &(iter->first), 4);
					tmp << router << ", " << iterf << ", " << buf << ", " << (iter->second) << endl;
					iter++;
				}
				buf = tmp.str();
			};
	};

	public:
		uint32_t id_next;
		RouterInterface* interfaces;
		int** interface_id_aa;

		Extract_Topology();
		~Extract_Topology();
		void process(Connections& cons, string& path);

		uint32_t time_s;
		uint32_t stat_next_export_s;

};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_EXTRACT_TOPOLOGY_UNWRAP Extract_Topology* exttopo; Data_Get_Struct(self, Extract_Topology, exttopo);

//-----------------------------------------------------------------------------
// basic class operation
//-----------------------------------------------------------------------------
void rb_extract_topology_free(void *p);
VALUE rb_extract_topology_alloc(VALUE klass);

//------------------------------------------------------------------------------
// analyse
//------------------------------------------------------------------------------
VALUE rb_extract_topology_process(VALUE self, VALUE cons, VALUE path);

#endif // EXTRACT_TOPOLOGY_H__
