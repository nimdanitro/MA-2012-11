/**
 * File: connection_matrix_entry.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "connection_matrix_entry.h"
Connection_Matrix_Entry::Connection_Matrix_Entry()
{
	state = CONNECTION_MATRIX_ENTRY_STATE_UNKNOWN;

	in_out_start_s = 0;
	in_out_stop_s = 0;
	out_in_start_s = 0;
	out_in_stop_s = 0;

	in_out_if = 0;
	out_in_if = 0;
	in_out_router = 0;
	out_in_router = 0;

	in_out_packets = 0;
	out_in_packets = 0;
	in_out_bytes = 0;
	out_in_bytes = 0;

};
Connection_Matrix_Entry::~Connection_Matrix_Entry()
{
};
void Connection_Matrix_Entry::add_connection(Connection* con)
{
	if(con->direction == CONNECTION_DIRECTION_IN_OUT)
	{
		if(in_out_start_s == 0 or con->start_s < in_out_start_s)
		{
			in_out_start_s = con->start_s;
		}

		if(con->stop_s > in_out_stop_s)
		{
			in_out_stop_s = con->stop_s;
		}

		in_out_if = con->if_out;
		in_out_router = con->router;

		in_out_packets += con->packets;
		in_out_bytes   += con->bytes;
	}
	else if (con->direction == CONNECTION_DIRECTION_OUT_IN)
	{
		if(out_in_start_s == 0 or con->start_s < out_in_start_s)
		{
			out_in_start_s = con->start_s;
		}

		if(con->stop_s > out_in_stop_s)
		{
			out_in_stop_s = con->stop_s;
		}

		out_in_if = con->if_in;
		out_in_router = con->router;

		out_in_packets += con->packets;
		out_in_bytes   += con->bytes;
	}
	else
	{
		// this flow is not related to my job 
		// --> don't update the times
		//   --> will be deleted next time 
		assert(false); // filtering went crazy ?!?
	}
	return;
};
bool Connection_Matrix_Entry::prune(uint32_t older_s)
{
	// inactive?
	if(in_out_stop_s < older_s and out_in_stop_s < older_s)
	{
		return(true);
	}
	else
	{
		return(false);
	}
};
uint32_t Connection_Matrix_Entry::state_within(uint32_t i_start_s, uint32_t i_stop_s)
{
	// IN OUT
	if(in_out_start_s == 0)
	{ // NEVER
		state = 12;
	}
	else if(in_out_stop_s < i_start_s or in_out_start_s > i_stop_s)
	{ // NOT NOW
		state = 8;
	}
	else if(in_out_start_s < i_start_s and in_out_stop_s > i_stop_s)
	{ // SPAN
		state = 4;
	}
	else if(
		(i_start_s <= in_out_start_s and  in_out_start_s <= i_stop_s) or 
		(i_start_s <= in_out_stop_s  and  in_out_stop_s  <= i_stop_s)
	)
	{ // EVENT
		state = 0;
	}
	else
	{
		assert(false); // should never be reached ...
	}
	// OUT IN
	if(out_in_start_s == 0)
	{ // NEVER
		state += 4;
	}
	else if(out_in_stop_s < i_start_s or out_in_start_s > i_stop_s)
	{ // NOT NOW
		state += 3;
	}
	else if(out_in_start_s < i_start_s and  out_in_stop_s > i_stop_s)
	{ // SPAN
		state += 2;
	}
	else if(
		(i_start_s <= out_in_start_s and  out_in_start_s <= i_stop_s) or 
		(i_start_s <= out_in_stop_s  and  out_in_stop_s  <= i_stop_s)
	)
	{ // EVENT
		state += 1;
	}
	else
	{
		assert(false); // should never be reached ...
	}
	return(state);
}
void Connection_Matrix_Entry::to_s(string& buf) const
{
		stringstream tmp;
		tmp << (int) state << ", ";
		tmp << (int) in_out_start_s << ", ";
		tmp << (int) in_out_stop_s << ", ";
		tmp << (int) in_out_packets << ", ";
		tmp << (int) in_out_bytes << ", ";
		if (in_out_packets == 0 and in_out_bytes == 0)
			tmp << 0 << ", ";
		else if (in_out_packets == 0)
			tmp << -1 << ", ";
		else
			tmp << in_out_bytes/in_out_packets << ", ";

		tmp << (int) out_in_start_s << ", ";
		tmp << (int) out_in_stop_s << ", ";
		tmp << (int) out_in_packets << ", ";
		tmp << (int) out_in_bytes << ", ";
		if (out_in_packets == 0 and out_in_bytes == 0)
			tmp << 0 << ", ";
		else if (out_in_packets == 0)
			tmp << -1 << ", ";
		else
			tmp << out_in_bytes/out_in_packets << ", ";

		buf = tmp.str();
		return;
};
void Connection_Matrix_Entry::to_s_router(string& buf) const
{
		stringstream tmp;
		tmp << (int) in_out_router << ", ";
		tmp << (int) in_out_if << ", ";
		tmp << (int) out_in_router << ", ";
		tmp << (int) out_in_if << ", ";
		buf = tmp.str();
		return;
};
//******************************************************************************
// RUBY
//******************************************************************************
void rb_connection_matrix_entry_free(void *p)
{
	delete (Connection_Matrix_Entry*) p;
	p = NULL;
};
VALUE rb_connection_matrix_entry_alloc(VALUE klass)
{
	Connection_Matrix_Entry* p = NULL;
	VALUE obj;
	p = new Connection_Matrix_Entry();
	if (p == NULL)
    		rb_raise(rb_path2class("ConnectionMatrixEntryError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_connection_matrix_entry_free, p);
	return(obj);
};
