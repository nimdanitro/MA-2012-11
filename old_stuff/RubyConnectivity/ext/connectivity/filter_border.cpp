/**
 * File: filter_border.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "filter_border.h"
Filter_Border::Filter_Border()
:interfaces( FILTER_BORDER_ROUTER_MAX, vector<int>(FILTER_BORDER_INTERFACE_MAX, FILTER_BORDER_TYPE_UNKNOWN))
{
	// statistics
	time_s = 0;
	stat_next_export_s = 0;
	stat_reset();
};
Filter_Border::~Filter_Border(){};

void Filter_Border::add_interface(int router, int interface, int what)
{
	if(
		router < 0 or router >= FILTER_BORDER_ROUTER_MAX or 
		interface < 0 or interface >= FILTER_BORDER_INTERFACE_MAX or 
		what < 0 or what > 3
	)
	{
		cout << "ADD " << router << " " << interface << "  " << what << endl;
		cout << "Add Interface: Invalid data " << endl;
		cout.flush();
		throw 200; // FIXME
	}
	interfaces[router][interface] = what;
};

void Filter_Border::filter(Connection* con)
{

	stat_connections_processed++;

	if(
		con->router >= FILTER_BORDER_ROUTER_MAX or 
		con->if_in >= FILTER_BORDER_INTERFACE_MAX or
		con->if_out >= FILTER_BORDER_INTERFACE_MAX
	)
	{
		cout << "FILTER BORDER: Invalid data ROUTER " << (int) con->router << " IN " << (int) con->if_in << " OUT " << (int) con->if_out<< endl;
		cout.flush();
		con->valid = false;
		stat_connections_filtered++;
		return;
	}

	int if_in = interfaces[con->router][con->if_in];
	int if_out = interfaces[con->router][con->if_out];

	// Keep: INTERNET->CUSTOMER/BACKBONE/UNKNOWN
	if(

		(if_in == FILTER_BORDER_TYPE_INTERNET and 
		if_out != FILTER_BORDER_TYPE_INTERNET) or
		(if_out == FILTER_BORDER_TYPE_INTERNET and  
		if_in != FILTER_BORDER_TYPE_INTERNET)
	)
	{
		con->border = CONNECTION_BORDER_TRUE;
		return;
	}
	// Keep: UNKNOWN->CUSTOMER/BACKBONE
	if(

		(if_in == FILTER_BORDER_TYPE_INTERNET and 
		if_out != FILTER_BORDER_TYPE_INTERNET) or
		(if_out == FILTER_BORDER_TYPE_INTERNET and  
		if_in != FILTER_BORDER_TYPE_INTERNET)
	)
	{
		con->border = CONNECTION_BORDER_TRUE;
		return;
	}

	// Drop: CUSTOMER->CUSTOMER/BACKBONE
	// Drop: BACKBONE->CUSTOMER/BACKBONE
	con->border = CONNECTION_BORDER_FALSE;
	con->valid = false;
	stat_connections_filtered++;
};

//------------------------------------------------------------------------------
// STATISTICS
//------------------------------------------------------------------------------
void Filter_Border::stat_reset(void)
{
	stat_connections_processed = 0;
	stat_connections_filtered = 0;
	return;
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_filter_border_free(void *p)
{
	delete (Filter_Border*) p;
	p = NULL;
};
VALUE rb_filter_border_alloc(VALUE klass)
{
	Filter_Border* p = NULL;
	VALUE obj;
	p = new Filter_Border();
	if (p == NULL)
		rb_raise(rb_path2class("FilterBorderError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_filter_border_free, p);
	return(obj);
};

VALUE rb_filter_border_add_interface(VALUE self, VALUE router, VALUE interface, VALUE what)
{
	RB_FILTER_BORDER_UNWRAP
	fborder->add_interface(NUM2INT(router), NUM2INT(interface), NUM2INT(what));
	return(self);
}

VALUE rb_filter_border_filter(VALUE self, VALUE cons)
{
	RB_FILTER_BORDER_UNWRAP
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
		if (con->start_s > fborder->stat_next_export_s)
		{
			// update the time
			fborder->time_s = con->start_s;

			// lets call the export function (ruby)
			ID method_id = rb_intern("statistics_export");
			rb_funcall(self, method_id, 0);
		}

		// apply the filter
		fborder->filter(con);
	}
	return(self);
}
// Statistics
VALUE rb_filter_border_stat_get(VALUE self)
{
	RB_FILTER_BORDER_UNWRAP
	VALUE array = rb_ary_new();

	rb_ary_push(array, ULL2NUM(fborder->time_s));

	rb_ary_push(array, ULL2NUM(fborder->stat_connections_processed));
	rb_ary_push(array, ULL2NUM(fborder->stat_connections_filtered));
	return(array);
};
VALUE rb_filter_border_stat_reset(VALUE self)
{
	RB_FILTER_BORDER_UNWRAP
	fborder->stat_reset();
	return(self);
};
VALUE rb_filter_border_stat_next_export_s(VALUE self, VALUE next_export_s)
{
	RB_FILTER_BORDER_UNWRAP
	fborder->stat_next_export_s = NUM2UINT(next_export_s);
	return(self);
};

