/**
 * File: prefix_mapping_int.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef PREFIX_TO_INT_H__
#define PREFIX_TO_INT_H__

// own stuff
#include "prefix_mapping.h"

using namespace std;

class Prefix_To_Int_Map
{
	// adapter pattern
	// --> delgation ...
	public:
		static const char* CODE_VERSION() {return "00.00.01";};
		static const int CODE_VERSION_NBR = 1;

	private:
		Prefix_Mapping<int> map;
		int value_not_found;

	public:
		Prefix_To_Int_Map(void);
		void insert(const Prefix& prefix, int value);
		int lookup(const Prefix& prefix);
		void set_value_not_found(int not_found);
		int get_value_not_found(void);
};

#endif // PREFIX_TO_INT_H__
