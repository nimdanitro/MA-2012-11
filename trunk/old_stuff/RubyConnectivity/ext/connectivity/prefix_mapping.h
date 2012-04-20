/**
 * File: prefix_mapping.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef PREFIX_MAPPING_H__
#define PREFIX_MAPPING_H__

// c++ libs
#include <string>
#include <cassert>

// own stuff
#include "prefix.h"
#include "prefix_mapping_node.h"

using namespace std;

template <class T>
class Prefix_Mapping
{
	private:
		 Prefix_Mapping_Node<T>* head_ipv4, *head_ipv6;

	public:
		Prefix_Mapping<T>(void)
		{
			Prefix p;
			p.from("0.0.0.0/0");
			head_ipv4 = new Prefix_Mapping_Node<T>(p);

			p.from("0::0/0");
			head_ipv6 = new Prefix_Mapping_Node<T>(p);
		};
		~Prefix_Mapping<T>(void)
		{
			delete head_ipv4;
			delete head_ipv6;
		};

		void insert(const Prefix& prefix, T& value)
		{
			//cout << "INSERT: STARTED " << prefix.to_s() << endl;
			Prefix_Mapping_Node<T>*node, *last;
			int family;

			// get the correct root node
			family = prefix.get_family();
			if(family == Prefix::FAMILY_IPV4)
			{
				node = head_ipv4;
			}
			else if(family == Prefix::FAMILY_IPV6)
			{
				node = head_ipv6;
			}			
			else
			{
				cout << "INSERT: Addr Type is not yet supported" << endl;
				throw "INSERT: Addr Type is not yet supported";
			}
			
			last = node;

			// walk down
			//int depth = 0;
			//cout << "INSERT: WALK DOWN -- START" << endl;
			while(node != NULL) 
			{
				//depth += 1;
				//cout << "INSERT: WALK DOWN " << depth << endl;
				//cout << node->to_s();
				//cout.flush();

				last = node;
				node = node->get_next(prefix);
				//cout << "INSERT: WALK DOWN NEXT: " << (int) node << endl;
			}
			//cout << "INSERT: WALK DOWN -- STOP" << endl;
			//cout << "INSERT: Prefix corresponds to this/direct child of this node (" << node << ")" << endl;
			//cout << "INSERT: ADD (Prefix, Value)" << endl;
			last->add(prefix, value);
			//cout << "INSERT: FINISHED" << endl;
		};

		const T* lookup(const Prefix& prefix)
		{
			//cout << "LOOKUP: STARTED " << prefix.to_s() << endl;
			Prefix_Mapping_Node<T>*node_best_match, *node;
			int family;

			// init
			family = prefix.get_family();
			if(family == Prefix::FAMILY_IPV4){
				node = head_ipv4;
			}else if(family == Prefix::FAMILY_IPV6){
				node = head_ipv6;
			}else{
				cout << "LOOKUP: Unknown Addr Type" << endl;
				throw "LOOKUP: Unknown Addr Type";
			}
			node_best_match = NULL;

			// walk
			//int depth = 0;
			//cout << "LOOKUP: WALK DOWN -- START" << endl;
			while(node != NULL) 
			{
				//depth += 1;
				//cout << "LOOKUP: WALK DOWN " << depth << endl;
				//cout << node->to_s() << endl;
				//cout.flush();

				// provides this node a valid prefix?
				if(node->value_is_set())
					node_best_match = node;

				node = node->get_next(prefix);
				//cout << "LOOKUP: WALK DOWN NEXT: " << (int) node << endl;
			}
			//cout << "LOOKUP: WALK DOWN -- STOP" << endl;
			if (node_best_match != NULL)
			{
				//cout << "LOOKUP: FINISHED -- MATCH" <<  endl;
				return(node_best_match->get_value());
			}
			else
			{
				//cout << "LOOKUP: FINISHED -- NOMATCH" << endl;
				return(NULL);	
			}
		};
};
#endif // PREFIX_MAPPING_H__
