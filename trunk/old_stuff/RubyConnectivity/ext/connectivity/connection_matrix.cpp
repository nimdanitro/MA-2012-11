/**
 * File: data_parser.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "connection_matrix.h"
Connection_Matrix::Connection_Matrix()
{
	connections_ipv4_h.clear();
	connections_ipv6_h.clear();
	time_s = 0;
	periodic_job_next_s = 0;
	statistics_reset();
};
Connection_Matrix::~Connection_Matrix()
{

};
void Connection_Matrix::add(Connection* con, Connection_Matrix_Key& key)
{
	stat_connections_processed++;
	key.from(*con);
	if(con->addr_length==4){
		connections_ipv4_h[key].add_connection(con);		
		return;
	}
	if(con->addr_length==16){
 		connections_ipv6_h[key].add_connection(con);
		return;	
	}
};
void Connection_Matrix::prune(uint32_t older_s)
{
	int before = connections_ipv6_h.size();
	Connection_Matrix_HT::iterator iter = connections_ipv6_h.begin();
	while(iter != connections_ipv6_h.end())
	{
		if( (iter->second).prune(older_s) )
			connections_ipv6_h.erase(iter++);
		else
			iter++;
	}
	int after = connections_ipv6_h.size();
	cout << "cmev6, " << older_s << ", " << before << ", " << after << ", " << connections_ipv6_h.bucket_count()<< endl;
	
	before = connections_ipv4_h.size();
	iter = connections_ipv4_h.begin();
	while(iter != connections_ipv4_h.end())
	{
		if( (iter->second).prune(older_s) )
			connections_ipv4_h.erase(iter++);
		else
			iter++;
	}
	after = connections_ipv4_h.size();
	cout << "cmev4, " << older_s << ", " << before << ", " << after << ", " << connections_ipv4_h.bucket_count()<< endl;
	return;
};
void Connection_Matrix::analyzev4(uint32_t i_start_s, uint32_t i_stop_s)
{
	int statev4;
	Connection_Matrix_HT::iterator iter = connections_ipv4_h.begin();
	while(iter != connections_ipv4_h.end())
	{
		// analyse the connection ...
		statev4 = (iter->second).state_within(i_start_s, i_stop_s);
		stat_connections_state[statev4]++;
		iter++;
	}
	return;
}

void Connection_Matrix::analyzev6(uint32_t i_start_s, uint32_t i_stop_s)
{
	int statev6;
	Connection_Matrix_HT::iterator iter = connections_ipv6_h.begin();
	while(iter != connections_ipv6_h.end())
	{
		// analyse the connection ...
		statev6 = (iter->second).state_within(i_start_s, i_stop_s);
		stat_connections_state[statev6]++;
		iter++;
	}	
	return;
}
void Connection_Matrix::analyze(uint32_t i_start_s, uint32_t i_stop_s)
{
	analyzev4(i_start_s, i_stop_s);
	analyzev6(i_start_s, i_stop_s);
	return;
}
void Connection_Matrix::statistics_reset(void)
{
	stat_connections_processed = 0;
	for(int i = 0; i< CONNECTION_MATRIX_ENTRY_STATES; i++)
		stat_connections_state[i] = 0;
	return;
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_connection_matrix_free(void *p)
{
	delete (Connection_Matrix*) p;
	p = NULL;
};
VALUE rb_connection_matrix_alloc(VALUE klass)
{
	Connection_Matrix* p = NULL;
	VALUE obj;
	p = new Connection_Matrix();
	if (p == NULL)
    		rb_raise(rb_path2class("ConnectionMatrixError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_connection_matrix_free, p);
	return(obj);
};
VALUE rb_connection_matrix_add_connections(VALUE self, VALUE cons)
{
	RB_CONNECTION_MATRIX_UNWRAP
	Connections* cons_; 
	Connection* con;
	Connection_Matrix_Key key;
	Data_Get_Struct(cons, Connections, cons_);

	for(int i = 0; i < cons_->used; i++)
	{
		// cast
		con = cons_->connections_a + i;

		// validity check
		if(con->valid == false)
			continue;

		// timing
		if(con->start_s > cm->periodic_job_next_s)
		{
			cm->time_s = con->start_s;
			rb_yield(INT2NUM(cm->periodic_job_next_s));
		}
			
		// add connection
		cm->add(con, key);
	}
	return(self);
};
VALUE rb_connection_matrix_prune(VALUE self, VALUE older_s)
{
	RB_CONNECTION_MATRIX_UNWRAP
	cm->prune(NUM2UINT(older_s));
	return(self);
};
VALUE rb_connection_matrix_stat_get(VALUE self)
{
	RB_CONNECTION_MATRIX_UNWRAP
	VALUE array = rb_ary_new();

	rb_ary_push(array, ULL2NUM(cm->time_s));
	rb_ary_push(array, ULL2NUM(cm->stat_connections_processed));
	rb_ary_push(array, ULL2NUM(cm->connections_ipv4_h.size()+cm->connections_ipv6_h.size()));
	for(int i = 0; i<CONNECTION_MATRIX_ENTRY_STATES; i++)
		rb_ary_push(array, ULL2NUM(cm->stat_connections_state[i]));
	return(array);
};
VALUE rb_connection_matrix_stat_reset(VALUE self)
{
	RB_CONNECTION_MATRIX_UNWRAP
	cm->statistics_reset();
	return(self);
};
VALUE rb_connection_matrix_periodic_job_next_s(VALUE self, VALUE  periodic_job_next_s)
{
	RB_CONNECTION_MATRIX_UNWRAP
	cm-> periodic_job_next_s = NUM2UINT(periodic_job_next_s);
	return(self);
};
VALUE rb_connection_matrix_analyze(VALUE self, VALUE i_start_s, VALUE i_stop_s)
{
	RB_CONNECTION_MATRIX_UNWRAP
	cm->analyze(NUM2UINT(i_start_s), NUM2UINT(i_stop_s));
	return(self);
};
