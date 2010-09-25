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
	connections_h.clear();
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
	connections_h[key].add_connection(con);
	return;
};
void Connection_Matrix::prune(uint32_t older_s)
{

	int before = connections_h.size();
	Connection_Matrix_HT::iterator iter = connections_h.begin();
	while(iter != connections_h.end())
	{
		if( (iter->second).prune(older_s) )
			connections_h.erase(iter++);
		else
			iter++;
	}
	int after = connections_h.size();
	cout << "cme, " << older_s << ", " << before << ", " << after << ", " << connections_h.bucket_count()<< endl;

	return;
};
void Connection_Matrix::analyze(uint32_t i_start_s, uint32_t i_stop_s)
{
	int state;
	Connection_Matrix_HT::iterator iter = connections_h.begin();
	while(iter != connections_h.end())
	{
		// analyse the connection ...
		state = (iter->second).state_within(i_start_s, i_stop_s);
		stat_connections_state[state]++;
		iter++;
	}	
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
	rb_ary_push(array, ULL2NUM(cm->connections_h.size()));
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
