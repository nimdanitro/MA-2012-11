/**
 * File: filter_ipv6.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "filter_ipv6.h"
Filter_IPv6::Filter_IPv6()
{
	// statistics
	time_s = 0;
	stat_next_export_s = 0;
	stat_reset();
};
Filter_IPv6::~Filter_IPv6(){};
void Filter_IPv6::filter(Connection* con)
{
	if(con->addr_length == 16)
	{
		con->valid = false;
		stat_connections_filtered++;
	}
	stat_connections_processed++;
};

//------------------------------------------------------------------------------
// STATISTICS
//------------------------------------------------------------------------------
void Filter_IPv6::stat_reset(void)
{
	stat_connections_processed = 0;
	stat_connections_filtered = 0;
	return;
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_filter_ipv6_free(void *p)
{
	delete (Filter_IPv6*) p;
	p = NULL;
};
VALUE rb_filter_ipv6_alloc(VALUE klass)
{
	Filter_IPv6* p = NULL;
	VALUE obj;
	p = new Filter_IPv6();
	if (p == NULL)
		rb_raise(rb_path2class("FilterIPv6Error"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_filter_ipv6_free, p);
	return(obj);
};
VALUE rb_filter_ipv6_filter(VALUE self, VALUE cons)
{
	RB_FILTER_IPV6_UNWRAP
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
		if (con->start_s > fip6->stat_next_export_s)
		{
			// update the time
			fip6->time_s = con->start_s;

			// lets call the export function (ruby)
			ID method_id = rb_intern("statistics_export");
			rb_funcall(self, method_id, 0);
		}

		// apply the filter
		fip6->filter(con);
	}
	return(self);
}
// Statistics
VALUE rb_filter_ipv6_stat_get(VALUE self)
{
	RB_FILTER_IPV6_UNWRAP
	VALUE array = rb_ary_new();

	rb_ary_push(array, ULL2NUM(fip6->time_s));

	rb_ary_push(array, ULL2NUM(fip6->stat_connections_processed));
	rb_ary_push(array, ULL2NUM(fip6->stat_connections_filtered));
	return(array);
};
VALUE rb_filter_ipv6_stat_reset(VALUE self)
{
	RB_FILTER_IPV6_UNWRAP
	fip6->stat_reset();
	return(self);
};
VALUE rb_filter_ipv6_stat_next_export_s(VALUE self, VALUE next_export_s)
{
	RB_FILTER_IPV6_UNWRAP
	fip6->stat_next_export_s = NUM2UINT(next_export_s);
	return(self);
};

