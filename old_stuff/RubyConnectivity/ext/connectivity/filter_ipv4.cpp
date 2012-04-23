/**
 * File: filter_ipv4.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "filter_ipv4.h"
Filter_IPv4::Filter_IPv4()
{
	// statistics
	time_s = 0;
	stat_next_export_s = 0;
	stat_reset();
};
Filter_IPv4::~Filter_IPv4(){};
void Filter_IPv4::filter(Connection* con)
{
	if(con->addr_length == 4)
	{
		con->valid = false;
		stat_connections_filtered++;
	}
	stat_connections_processed++;
};

//------------------------------------------------------------------------------
// STATISTICS
//------------------------------------------------------------------------------
void Filter_IPv4::stat_reset(void)
{
	stat_connections_processed = 0;
	stat_connections_filtered = 0;
	return;
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_filter_ipv4_free(void *p)
{
	delete (Filter_IPv4*) p;
	p = NULL;
};
VALUE rb_filter_ipv4_alloc(VALUE klass)
{
	Filter_IPv4* p = NULL;
	VALUE obj;
	p = new Filter_IPv4();
	if (p == NULL)
		rb_raise(rb_path2class("FilterIPv4Error"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_filter_ipv4_free, p);
	return(obj);
};
VALUE rb_filter_ipv4_filter(VALUE self, VALUE cons)
{
	RB_FILTER_IPV4_UNWRAP
	Connection* con; 
	Connections* cons_; 
	Data_Get_Struct(cons, Connections, cons_);
	for(int i = 0; i < cons_->used; i++)
	{
		// cast
		con = cons_->connections_a + i;

		// valid connection?
		if (con->valid == false)
			continue;

		// timing
		// periodic stat export					
		if (con->start_s > fip4->stat_next_export_s)
		{
			// update the time
			fip4->time_s = con->start_s;

			// lets call the export function (ruby)
			ID method_id = rb_intern("statistics_export");
			rb_funcall(self, method_id, 0);
		}

		// apply the filter
		fip4->filter(con);
	}
	return(self);
}
// Statistics
VALUE rb_filter_ipv4_stat_get(VALUE self)
{
	RB_FILTER_IPV4_UNWRAP
	VALUE array = rb_ary_new();

	rb_ary_push(array, ULL2NUM(fip4->time_s));

	rb_ary_push(array, ULL2NUM(fip4->stat_connections_processed));
	rb_ary_push(array, ULL2NUM(fip4->stat_connections_filtered));
	return(array);
};
VALUE rb_filter_ipv4_stat_reset(VALUE self)
{
	RB_FILTER_IPV4_UNWRAP
	fip4->stat_reset();
	return(self);
};
VALUE rb_filter_ipv4_stat_next_export_s(VALUE self, VALUE next_export_s)
{
	RB_FILTER_IPV4_UNWRAP
	fip4->stat_next_export_s = NUM2UINT(next_export_s);
	return(self);
};

