/**
 * File: connection_matrix_key.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "connection_matrix_key.h"
//******************************************************************************
// RUBY
//******************************************************************************
void rb_connection_matrix_key_free(void *p)
{
	delete (Connection_Matrix_Key*) p;
	p = NULL;
};
VALUE rb_connection_matrix_key_alloc(VALUE klass)
{
	Connection_Matrix_Key* p = NULL;
	VALUE obj;
	p = new Connection_Matrix_Key();
	if (p == NULL)
    		rb_raise(rb_path2class("ConnectionMatrixKeyError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_connection_matrix_key_free, p);
	return(obj);
};
