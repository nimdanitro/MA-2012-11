/**
 * File: prefix_mapping_node.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef PREFIX_MAPPING_NODE_H__
#define PREFIX_MAPPING_NODE_H__

// c++ libs
#include <string>
#include <cassert>

// own stuff
#include "prefix.h"

using namespace std;

template <class T>
class Prefix_Mapping_Node
{
	private:
		// content
		Prefix prefix;
		T value;
		bool value_set;

		// tree
		Prefix_Mapping_Node* left;  // next bit == 0
		Prefix_Mapping_Node* right; // next bit == 1

	public:
		Prefix_Mapping_Node(void)
		{
			value_set = false;
			left = NULL;
			right = NULL;
		};
		Prefix_Mapping_Node(const Prefix & p)
		:	prefix(p)
		{
			value_set = false;
			left = NULL;
			right = NULL;
		}
		Prefix_Mapping_Node(const Prefix & p, const T&  v)
		:	prefix(p), value(v)
		{
			value_set = true;
			left = NULL;
			right = NULL;
		}
//------------------------------------------------------------------------------
// ACCESSORS
//------------------------------------------------------------------------------
		bool value_is_set(void)
		{
			return(value_set);
		};
		const T* get_value(void)
		{
			return(&value);
		};
//------------------------------------------------------------------------------
// HELPER
//------------------------------------------------------------------------------
		const string to_s(void) const 
		{
			stringstream tmp;
			tmp << "Prefix_Mapping_Node " << (int) this << endl;
			tmp << "Prefix:      " << prefix.to_s() << endl;
			tmp << "Value is set:" << value_set << endl;
			tmp << "LEFT:        " << (int) left  << endl;
			tmp << "RIGHT:       " << (int) right;
			return(string(tmp.str()));
		};
//------------------------------------------------------------------------------
// TREE FUNCTIONS
//------------------------------------------------------------------------------
	Prefix_Mapping_Node<T>* get_next(const Prefix& other)
	{
		//cout << "GET NEXT: ";
		// no leafs
		if (left == NULL and right == NULL)
		{
			//cout << "GET NEXT:  No leafs found, return NULL" << endl;
			//cout.flush();
			return(NULL);
		}

		// no bits left
		if( other.get_size() <= prefix.get_size())
		{
			//cout << "GET NEXT:  No bit left" << endl;
			//cout.flush();
			return(NULL);
		}

		// we have at least one leaf 
		// ... but which one would be the right one
		int bit;
		Prefix_Mapping_Node* next;
		//cout << "GET NEXT: next bit ?" << endl;
		bit  = other.get_bit_at(prefix.get_size() + 1);
		next = (bit == 0) ? left : right;
		//cout << "GET NEXT: bit at " << prefix.get_size() + 1 << " is "  << bit << endl;

		// no node
		if(next == NULL)
			return(NULL);

		// no match
		if(other.includes(next->prefix))
			return(next);
		else
			return(NULL);
	};
	void add(const Prefix& other, T& v)
	{
		//cout << "ADD: STARTED " << other.to_s() << endl;

		// is this node the traget ?
		if(prefix.eql(other))
		{
			//cout << "ADD: EQUAL =  true --> replace or set value of this node " << endl;
			//cout.flush();
			value = v;
			value_set = true;
			return;
		}
		else
		{
			//cout << "ADD: EQUAL = false " << endl;
		}

		// we have to add a new node into the chain: Left or Right?
		int bit = other.get_bit_at(prefix.get_size() + 1);
		//cout << "ADD: BIT at position " << prefix.get_size() + 1 <<  " is "  << bit << endl;
		if(bit == 0)
		{
			//cout << "ADD: New LEAF on the LEFT " << endl;
			left = add(other, v, left);
			//cout << "ADD: FINISHED" << endl;
			return;
		}
		else if (bit == 1)
		{
			//cout << "ADD: New LEAF on the RIGHT " << endl;
			right = add(other, v, right);
			//cout << "ADD: FINISHED" << endl;
			return;
		}
		else
		{
			//cout << "ADD: We don't know if we have to add the node on the right or on the left ?!!" << endl;
			throw "WTF";
		}
		//cout << "ADD: FINISHED" << endl;
	};
	Prefix_Mapping_Node* add(const Prefix& other, T& v, Prefix_Mapping_Node* root)
	{

		//cout << "ADD NEW NODE: STARTED" << endl;
		if(root == NULL)
		{
			//cout << "ADD NEW NODE: No other node ... just add" << endl;
			root = new Prefix_Mapping_Node(other, v);
			//cout << "ADD NEW NODE: FINISHED" << endl;
			return(root);
		}

		if(root->prefix.includes(other))
		{
			// CASE ONE:
			// -->[other]-->[root]
			//           -->[NULL]
			// CASE DUE:
			// -->[other]-->[NULL]
			//           -->[root]
			//cout << "ADD NEW NODE: New Node includes the old prefix: Extend the chain" << endl;	
			Prefix_Mapping_Node* other_node = new Prefix_Mapping_Node(other, v);
			other_node->attach(root);
			//cout << "ADD NEW NODE: FINISHED" << endl;
			return(other_node);
		}
		else
		{
			//cout << "ADD NEW NODE: New Node is NOT TOTALLY included the old prefix" << endl;	
		}
		int common_bits = other.common_bits(root->prefix);
		//cout << "ADD NEW NODE: Common Bits: " << common_bits << endl;	
		if(common_bits <= root->prefix.get_size())
		{
			// CASE TRE
			// -->[new]-->[left]
			//         -->[other]
			// CASE QUATRO
			// -->[new]-->[other]
			//         -->[left]
			//cout << "ADD NEW NODE: New Node differs from the old prefix: Make a brunch" << endl;	
			// A new node having the prefix max common prefix length
			Prefix_Mapping_Node* new_node   = new Prefix_Mapping_Node(other);
			new_node->prefix.cut(common_bits);
			new_node->left = NULL;
			new_node->right = NULL;

			Prefix_Mapping_Node* other_node = new Prefix_Mapping_Node(other, v);
			other_node->left = NULL;
			other_node->right = NULL;

			new_node->attach(root);
			new_node->attach(other_node);

			assert( (new_node->left == other_node and new_node->right == root) or (new_node->left == root and new_node->right == other_node) );

			//cout << "ADD NEW NODE: BRUNCH:   " <<  new_node->to_s() << endl;
			//cout << "ADD NEW NODE: NEW NODE: " <<  other_node->to_s() << endl;
			//cout << "ADD NEW NODE: OLD NODE: " <<  root->to_s() << endl;

			//cout << "ADD NEW NODE: FINISHED" << endl;
			return(new_node);
		}
		//cout << "ADD NEW NODE: no case matched !??" << endl;	
		cout.flush();
		throw "WTF -- This line should never be reached";
	};
	void attach(Prefix_Mapping_Node* leaf)
	{
		int bit = leaf->prefix.get_bit_at(prefix.get_size() + 1);		
		if(bit == 0)
			left = leaf;
		else if (bit == 1)
			right = leaf;
		else
			throw "WTF";		
	};
};
#endif // PREFIX_MAPPING_NODE_H__
