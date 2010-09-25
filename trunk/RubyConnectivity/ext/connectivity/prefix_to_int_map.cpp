/**
 * File: prefix_mapping_bool.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#include "prefix_to_int_map.h"
Prefix_To_Int_Map::Prefix_To_Int_Map(void)
{
	value_not_found = 0;
}
void Prefix_To_Int_Map::insert(const Prefix& prefix, int value)
{
	map.insert(prefix, value);
};
int Prefix_To_Int_Map::lookup(const Prefix& prefix)
{
	const int* best_match = map.lookup(prefix);
	if (best_match == NULL)
		return(value_not_found);
	else
		return(*best_match);
};
void Prefix_To_Int_Map::set_value_not_found(int not_found)
{
	value_not_found = not_found;
};
int Prefix_To_Int_Map::get_value_not_found(void)
{
	return(value_not_found);
};
