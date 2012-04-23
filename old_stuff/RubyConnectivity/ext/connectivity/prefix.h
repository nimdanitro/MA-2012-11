/**
 * File: prefix.h 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 */

#ifndef PREFIX_H__
#define PREFIX_H__

// c libs
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <cassert>

// c++ libs
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class Prefix_E_Invalid_Prefix {};

class Prefix
{
/*
	 :: Prefix ::
	 	addr := network address       [32/128]
		size := how many bits are set [0,32/128]
*/
	public:
		static const char* CODE_VERSION() {return "00.00.01";};
		static const int CODE_VERSION_NBR = 1;

		static const int FAMILY_UNKNOWN = -1;
		static const int FAMILY_IPV4 = AF_INET;
		static const int FAMILY_IPV6 = AF_INET6;

		static const int SIZE_MAX_IPV4 = 32;
		static const int SIZE_MAX_IPV6 = 128;

		static const int MIN_STRING_LENGTH = INET6_ADDRSTRLEN + 5;

	private:
		int family;
		int size;
		char* addr;

	public:
		Prefix(void);
		Prefix(const Prefix& other);
		~Prefix(void);

		// assessor
		const int get_family(void) const;
		const int get_bit_at (int position) const;
		const int get_size(void) const;
		const int get_max_size(void) const;

		// HELPER
		void clear(void);

		void addr_alloc(int family_new);
		int addr_get_family(const char* prefix_txt) const;
		void addr_set_unused_bits_to_zero(int max_bits);
		bool eql(const Prefix& other) const;
		void cut(int bits);

		// FROM
		void from(const string& prefix_txt);
		void from(const char* prefix_txt);
		void from_nb(const char* prefix_bin, int size_, int family_);

		// TO
		const string to_s(void) const;

		// COMPARE
		const int common_bits(const Prefix& other) const ;
		const bool includes (const Prefix& other) const ;
};

#endif // PREFIX
