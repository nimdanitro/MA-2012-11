/**
 * File: connections.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef CONNECTIONS_H__
#define CONNECTIONS_H__
#define __STDC_LIMIT_MACROS

#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <string>
#include <sstream>

#include "ruby.h"

#include "connection.h"

/* Changelog

V 00.00.01, 2010.02.28
	- first version by Dominik Schatzmann
*/

#define CONNECTIONS_VERSION "00.00.01"
#define CONNECTIONS_VERSION_NBR 000001

using namespace std;
class Connections
{
	public:
		int capacity;
		int used;
		Connection* connections_a;
//------------------------------------------------------------------------------
	Connections();
	~Connections();
//------------------------------------------------------------------------------
	void resize(int capacity_);
	void reset(void);
	Connection* get_first_unused(void);
	Connection* get_next_unused(void);
};

//******************************************************************************
// RUBY
//******************************************************************************
#define RB_CONNECTIONS_UNWRAP Connections* cons; Data_Get_Struct(self, Connections, cons);
void rb_connections_free(void *p);
VALUE rb_connections_alloc(VALUE klass);

VALUE rb_connections_resize(VALUE self, VALUE size);
VALUE rb_connections_capacity(VALUE self);
VALUE rb_connections_used(VALUE self);

VALUE rb_connections_get_first_unused(VALUE self);
VALUE rb_connections_get_next_unused(VALUE self);

VALUE rb_connections_reset(VALUE self);
VALUE rb_connections_each_used(VALUE self);
VALUE rb_connections_each_all(VALUE self);
#endif // CONNECTIONS_H__
