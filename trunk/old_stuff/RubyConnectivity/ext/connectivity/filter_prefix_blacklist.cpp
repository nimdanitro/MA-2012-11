/**
 * File: filter_prefix_blacklist.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "filter_prefix_blacklist.h"

Filter_Prefix_Blacklist::Filter_Prefix_Blacklist()
{
	prefix_map.set_value_not_found(0);
	// statistics
	time_s = 0;
	stat_next_export_s = 0;
	stat_reset();
};
Filter_Prefix_Blacklist::~Filter_Prefix_Blacklist(){};

void Filter_Prefix_Blacklist::add_prefix(const string& prefix, int as)
{
	if( as < 0 or prefix.size() < 3)
	{
		cout << "FILTER BLACKLIST ADD: Invalid data '" << prefix << "'  " << as << "  "<< endl;
		cout.flush();
		throw 200; // FIXME
	}
	Prefix p;
	p.from(prefix);
	prefix_map.insert(p, as);
};

void Filter_Prefix_Blacklist::filter(Connection* con, Prefix& p)
{
	stat_connections_processed++;
	int src = 0;
	int dst = 0;
	string tmp;
	if(con->addr_length == 4)
	{
		p.from_nb(con->addr_src, 32, Prefix::FAMILY_IPV4);
		src = prefix_map.lookup(p);

		p.from_nb(con->addr_dst, 32, Prefix::FAMILY_IPV4);
		dst = prefix_map.lookup(p);
	}else if(con->addr_length == 16){
			p.from_nb(con->addr_src, 128, Prefix::FAMILY_IPV6);
			src = prefix_map.lookup(p);

			p.from_nb(con->addr_dst, 128, Prefix::FAMILY_IPV6);
			dst = prefix_map.lookup(p);
	}else{
		cout << "FILTER BLACKLIST ADD: FILTER not implemented ... "<< endl;
		cout.flush();
		throw 200; // FIXME
	}
	if(src == 0 and dst == 0)
		return;

	stat_connections_filtered++;
	con->valid = false;
	return;
};

//------------------------------------------------------------------------------
// STATISTICS
//------------------------------------------------------------------------------
void Filter_Prefix_Blacklist::stat_reset(void)
{
	stat_connections_processed = 0;
	stat_connections_filtered = 0;
	return;
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_filter_prefix_blacklist_free(void *p)
{
	delete (Filter_Prefix_Blacklist*) p;
	p = NULL;
};
VALUE rb_filter_prefix_blacklist_alloc(VALUE klass)
{
	Filter_Prefix_Blacklist* p = NULL;
	VALUE obj;
	p = new Filter_Prefix_Blacklist();
	if (p == NULL)
		rb_raise(rb_path2class("FilterPrefixBlacklistError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_filter_prefix_blacklist_free, p);
	return(obj);
};

VALUE rb_filter_prefix_blacklist_add_prefix(VALUE self, VALUE prefix, VALUE v)
{
	RB_FILTER_PREFIX_BLACKLIST_UNWRAP
	string prefix_s = RSTRING_PTR(prefix);
	fpb->add_prefix(prefix_s , NUM2INT(v));
	return(self);
}
VALUE rb_filter_prefix_blacklist_filter(VALUE self, VALUE cons)
{
	RB_FILTER_PREFIX_BLACKLIST_UNWRAP
	Connection* con; 
	Connections* cons_;
	Prefix p;
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
		if (con->start_s > fpb->stat_next_export_s)
		{
			// update the time
			fpb->time_s = con->start_s;

			// lets call the export function (ruby)
			ID method_id = rb_intern("statistics_export");
			rb_funcall(self, method_id, 0);
		}

		// apply the filter
		fpb->filter(con, p);
	}
	return(self);
}
// Statistics
VALUE rb_filter_prefix_blacklist_stat_get(VALUE self)
{
	RB_FILTER_PREFIX_BLACKLIST_UNWRAP
	VALUE array = rb_ary_new();

	rb_ary_push(array, ULL2NUM(fpb->time_s));

	rb_ary_push(array, ULL2NUM(fpb->stat_connections_processed));
	rb_ary_push(array, ULL2NUM(fpb->stat_connections_filtered));
	return(array);
};
VALUE rb_filter_prefix_blacklist_stat_reset(VALUE self)
{
	RB_FILTER_PREFIX_BLACKLIST_UNWRAP
	fpb->stat_reset();
	return(self);
};
VALUE rb_filter_prefix_blacklist_stat_next_export_s(VALUE self, VALUE next_export_s)
{
	RB_FILTER_PREFIX_BLACKLIST_UNWRAP
	fpb->stat_next_export_s = NUM2UINT(next_export_s);
	return(self);
};

