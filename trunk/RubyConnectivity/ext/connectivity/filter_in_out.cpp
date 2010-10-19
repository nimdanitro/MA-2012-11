/**
 * File: filter_in_out.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "filter_in_out.h"

Filter_In_Out::Filter_In_Out()
{
	prefix_map.set_value_not_found(0);
	// statistics
	time_s = 0;
	stat_next_export_s = 0;
	stat_reset();
};
Filter_In_Out::~Filter_In_Out(){};

void Filter_In_Out::add_prefix(const string& prefix, int as)
{
	if( as < 0 or prefix.size() < 3)
	{
		cout << "FILTER IN OUT ADD: Invalid data '" << prefix << "'  " << as << "  "<< endl;
		cout.flush();
		throw 200; // FIXME
	}
	Prefix p;
	p.from(prefix);
	prefix_map.insert(p, as);
};

void Filter_In_Out::filter(Connection* con, Prefix& p)
{
	stat_connections_processed++;
	int as_src = 0;
	int as_dst = 0;
	string tmp;
	if(con->addr_length == 4){
		p.from_nb(con->addr_src, 32, Prefix::FAMILY_IPV4);
		as_src = prefix_map.lookup(p);
		p.from_nb(con->addr_dst, 32, Prefix::FAMILY_IPV4);
		as_dst = prefix_map.lookup(p);
	}else if(con->addr_length == 16){
		p.from_nb(con->addr_src, 128, Prefix::FAMILY_IPV6);
		as_src = prefix_map.lookup(p);
		p.from_nb(con->addr_dst, 128, Prefix::FAMILY_IPV6);
		as_dst = prefix_map.lookup(p);
	}else{
		cout << "FILTER IN OUT ADD: FILTER not implemented ... "<< endl;
		cout.flush();
		throw 200; // FIXME
	}

	if(as_src == 559 and as_dst != 559)
	{
		con->direction = CONNECTION_DIRECTION_IN_OUT;
		stat_connections_in_out++;		
		return;
	}
	if(as_src != 559 and as_dst == 559)
	{
		con->direction = CONNECTION_DIRECTION_OUT_IN;
		stat_connections_out_in++;		
		return;
	}
	if(as_src != 559 and as_dst != 559)
	{
		con->direction = CONNECTION_DIRECTION_OUT_OUT;
		con->valid = false;
		stat_connections_filtered++;
		stat_connections_out_out++;		
		return;
	}
	if(as_src == 559 and as_dst == 559)
	{
		con->direction = CONNECTION_DIRECTION_IN_IN;
		con->valid = false;
		stat_connections_filtered++;	
		stat_connections_in_in++;	
		return;
	}

	// this line shoul never be reached
	assert(false);
	throw 200; // WTF 
};

//------------------------------------------------------------------------------
// STATISTICS
//------------------------------------------------------------------------------
void Filter_In_Out::stat_reset(void)
{
	stat_connections_processed = 0;
	stat_connections_filtered = 0;
	stat_connections_in_in = 0;
	stat_connections_in_out = 0;
	stat_connections_out_in = 0;
	stat_connections_out_out = 0;
	return;
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_filter_in_out_free(void *p)
{
	delete (Filter_In_Out*) p;
	p = NULL;
};
VALUE rb_filter_in_out_alloc(VALUE klass)
{
	Filter_In_Out* p = NULL;
	VALUE obj;
	p = new Filter_In_Out();
	if (p == NULL)
		rb_raise(rb_path2class("FilterInOutError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_filter_in_out_free, p);
	return(obj);
};

VALUE rb_filter_in_out_add_prefix(VALUE self, VALUE prefix, VALUE as)
{
	RB_FILTER_IN_OUT_UNWRAP
	string prefix_s = RSTRING_PTR(prefix);
	fio->add_prefix(prefix_s , NUM2INT(as));
	return(self);
}
VALUE rb_filter_in_out_filter(VALUE self, VALUE cons)
{
	RB_FILTER_IN_OUT_UNWRAP
	Connection* con; 
	Connections* cons_;
	Prefix p;
	Data_Get_Struct(cons, Connections, cons_);
	for(int i = 0; i < cons_->used; i++)
	{
		// cast
		con = cons_->connections_a + i;

		// valid connection?
		if(con->valid == false)
			continue;

		// timing
		// periodic stat export					
		if (con->start_s > fio->stat_next_export_s)
		{
			// update the time
			fio->time_s = con->start_s;

			// lets call the export function (ruby)
			ID method_id = rb_intern("statistics_export");
			rb_funcall(self, method_id, 0);
		}

		// apply the filter
		fio->filter(con, p);
		assert(con->direction != CONNECTION_DIRECTION_UNKNOWN);
	}
	return(self);
}
// Statistics
VALUE rb_filter_in_out_stat_get(VALUE self)
{
	RB_FILTER_IN_OUT_UNWRAP
	VALUE array = rb_ary_new();

	rb_ary_push(array, ULL2NUM(fio->time_s));

	rb_ary_push(array, ULL2NUM(fio->stat_connections_processed));
	rb_ary_push(array, ULL2NUM(fio->stat_connections_filtered));
	rb_ary_push(array, ULL2NUM(fio->stat_connections_in_in));
	rb_ary_push(array, ULL2NUM(fio->stat_connections_in_out));
	rb_ary_push(array, ULL2NUM(fio->stat_connections_out_in));
	rb_ary_push(array, ULL2NUM(fio->stat_connections_out_out));
	return(array);
};
VALUE rb_filter_in_out_stat_reset(VALUE self)
{
	RB_FILTER_IN_OUT_UNWRAP
	fio->stat_reset();
	return(self);
};
VALUE rb_filter_in_out_stat_next_export_s(VALUE self, VALUE next_export_s)
{
	RB_FILTER_IN_OUT_UNWRAP
	fio->stat_next_export_s = NUM2UINT(next_export_s);
	return(self);
};

