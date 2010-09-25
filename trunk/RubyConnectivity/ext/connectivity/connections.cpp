/**
 * File: data_parser.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "connections.h"
Connections::Connections()
{
	capacity = 0;
	used = 0;
	connections_a = NULL;
};
Connections::~Connections()
{
	if(connections_a != NULL)
	{
		delete [] connections_a;
	}
};
void Connections::resize(int capacity_)
{
	if(connections_a != NULL)	
	{
		delete [] connections_a;
	}
	capacity = capacity_;
	used = 0;
	connections_a = new Connection[capacity];
	return;
};
void Connections::reset(void)
{
	used = 0;
	return;
};
Connection* Connections::get_first_unused(void)
{
	if(used < capacity)
		return(connections_a+used);
	else
		return(NULL);
};
Connection* Connections::get_next_unused(void)
{
	used++;
	if(used < capacity)
		return(connections_a+used);
	else
		return(NULL);
};

//******************************************************************************
// RUBY
//******************************************************************************
void rb_connections_free(void *p)
{
	delete (Connections*) p;
	p = NULL;
};
VALUE rb_connections_alloc(VALUE klass)
{
	Connections* p = NULL;
	VALUE obj;
	p = new Connections();
	if (p == NULL)
    		rb_raise(rb_path2class("ConnectionsError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_connections_free, p);
	return(obj);
};

VALUE rb_connections_resize(VALUE self, VALUE size)
{
	RB_CONNECTIONS_UNWRAP
	int capacity_ = NUM2INT(size);
	cons->resize(capacity_);
	return(self);
};
VALUE rb_connections_capacity(VALUE self)
{
	RB_CONNECTIONS_UNWRAP
	return(ULL2NUM(cons->capacity));
};
VALUE rb_connections_used(VALUE self)
{
	RB_CONNECTIONS_UNWRAP
	return(ULL2NUM(cons->used));
};
VALUE rb_connections_get_first_unused(VALUE self)
{
	RB_CONNECTIONS_UNWRAP
	Connection* con = cons->get_first_unused();
	if(con != NULL)
	{
		VALUE klass_con = rb_path2class("Connection");
		return(Data_Wrap_Struct(klass_con, 0, 0, con));
	}
	else
		return(Qnil);
};
VALUE rb_connections_get_next_unused(VALUE self)
{
	RB_CONNECTIONS_UNWRAP
	Connection* con = cons->get_first_unused();
	if(con != NULL)
	{
		VALUE klass_con = rb_path2class("Connection");
		return(Data_Wrap_Struct(klass_con, 0, 0, con));
	}
	else
		return(Qnil);
};
VALUE rb_connections_reset(VALUE self)
{
	RB_CONNECTIONS_UNWRAP
	cons->reset();
	return(self);
}
VALUE rb_connections_each_used(VALUE self)
{
	RB_CONNECTIONS_UNWRAP
	VALUE klass_con = rb_path2class("Connection");
	for (int i = 0; i < cons->used; i++)
	{
		rb_yield(Data_Wrap_Struct(klass_con, 0, 0, cons->connections_a+i));
	}
	return(self);
};
VALUE rb_connections_each_all(VALUE self)
{
	RB_CONNECTIONS_UNWRAP
	VALUE klass_con = rb_path2class("Connection");
	for (int i = 0; i < cons->capacity; i++)
	{
		rb_yield(Data_Wrap_Struct(klass_con, 0, 0, cons->connections_a+i));
	}
	return(self);
};
