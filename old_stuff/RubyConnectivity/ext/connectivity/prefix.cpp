#include "prefix.h"
Prefix::Prefix(void)
{
	family = FAMILY_UNKNOWN;
	size = 0;
	addr = NULL;
};
Prefix::Prefix(const Prefix& other)
{
	family = FAMILY_UNKNOWN;
	addr_alloc(other.family);
	size = other.size;

	if(family == FAMILY_IPV4)
		memcpy(addr, other.addr, SIZE_MAX_IPV4);
	else if (family == FAMILY_IPV6)
		memcpy(addr, other.addr, SIZE_MAX_IPV6);
	else
	{
		cout << "Prefix::Prefix(const Prefix& other) UNKNOWN ADDRESS" << endl;
		throw "UNKNOWN ADDRESS";
	}
};
Prefix::~Prefix(void)
{
	clear();
};
//-----------------------------------------------------------------------------
// Accessor
//-----------------------------------------------------------------------------
const int Prefix::get_family(void) const 
{
	return(family);
};
int Prefix::addr_get_family(const char* prefix_txt) const
{
		// extract prefix family
		if(strchr(prefix_txt, '.'))
			return(FAMILY_IPV4);
		else if(strchr (prefix_txt, ':'))
			return(FAMILY_IPV6);
		else
			return(FAMILY_UNKNOWN);
}
const int Prefix::get_size(void) const 
{
	return(size);
};
const int Prefix::get_max_size(void) const 
{
	if(family == FAMILY_IPV4)
		return(SIZE_MAX_IPV4);
	else if(family == FAMILY_IPV6)
		return(SIZE_MAX_IPV6);
	else if(family == FAMILY_UNKNOWN)
		return(0);
	else
		throw "WTF";
};

const int Prefix::get_bit_at (int position) const
{
	if(position > size or position == 0)
	{
		cout << "Prefix::get_bit_at :: invalid position :: " << position << endl; 
		throw "Invalid";
	}
	int byte = addr[(position-1) >> 3];
	int bit  = (byte >>( 7 - (position -1 & 0x07))) & 0x01;
	return(bit);
};
//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------
void Prefix::clear(void)
{
	family = FAMILY_UNKNOWN;
	size = 0;
	if (addr != NULL);
		delete [] addr;

	addr = NULL;	
};
void Prefix::addr_alloc(int family_new)
{
	if(family == family_new)
	{
		return;
	}
	else
	{
		if(family != FAMILY_UNKNOWN)
			delete [] addr;

		if (family_new == FAMILY_IPV4)
		{
			addr = new char[SIZE_MAX_IPV4];
			family = FAMILY_IPV4;
		}
		else if (family_new == FAMILY_IPV6)
		{
			addr = new char[SIZE_MAX_IPV6];
			family = FAMILY_IPV6;
		}
		else
		{
			addr = NULL;
			family = FAMILY_UNKNOWN;
		}
	};
	return;
};


void Prefix::addr_set_unused_bits_to_zero(int max_bits)
{
	// correction of malformed prefixes by 
	// set all bits above the prefix size to zero
	int max_bytes = max_bits >> 3;
	if(size < max_bits)
	{
		int byte = size >> 3;   // / 8
		int bits = size & 0x07; // % 8
		uint8_t bit_mask = 255 << (8-bits);
		addr[byte] = addr[byte] & bit_mask;
		for(int i = byte+1 ; i < max_bytes; i++)
			addr[i] = 0;
	}
}
bool Prefix::eql(const Prefix& other) const
{
	if(family != other.family)
		return(false);

	if(size != other.size)
		return(false);

	int i = 0;
	while(i < size)
	{
		if(addr[i] != other.addr[i])
			return(false);
		i++;
	}
	return(true);
}
void Prefix::cut(int bits)
{
	// cut the prefix to n bits
	if(bits > size)
		throw "WTF";

	size = bits;
	if (family == FAMILY_IPV4)
		addr_set_unused_bits_to_zero(SIZE_MAX_IPV4);
	else if (family == FAMILY_IPV6)
		addr_set_unused_bits_to_zero(SIZE_MAX_IPV6);
	else
		throw "WTF";
}

//-----------------------------------------------------------------------------
// FROM
//-----------------------------------------------------------------------------
void Prefix::from(const string& prefix_txt)
{
	from(prefix_txt.c_str());
};
void Prefix::from(const char* prefix_txt)
{
	char network_s[INET6_ADDRSTRLEN];
	const char* slash;

	// extract the 'addr family' and allocate the required memory
	addr_alloc(addr_get_family(prefix_txt));

	// extract the '/'
	slash = strchr(prefix_txt, '/');
	if(slash == NULL)
		throw Prefix_E_Invalid_Prefix();

	// extract prefix size
	size = atoi(slash + 1);

	// make some sanity checks and extract the network ordered addr
	if(family == FAMILY_IPV4)
	{
		// IPv4
		if((slash - prefix_txt) > INET_ADDRSTRLEN or size > SIZE_MAX_IPV4 or size < 0)
			throw Prefix_E_Invalid_Prefix();

		strncpy(network_s, prefix_txt, slash - prefix_txt);
		network_s[slash-prefix_txt] = '\0';

		if(inet_pton(AF_INET, network_s, addr) != 1)
			throw Prefix_E_Invalid_Prefix();

		addr_set_unused_bits_to_zero(SIZE_MAX_IPV4);
	}
	else if(family == AF_INET6)
	{ // IPv6
		if( (slash - prefix_txt > INET6_ADDRSTRLEN) or size > SIZE_MAX_IPV6 or size < 0)
			throw Prefix_E_Invalid_Prefix();

		strncpy(network_s, prefix_txt, slash - prefix_txt);
		network_s[slash-prefix_txt] = '\0';

		if(inet_pton(AF_INET6, network_s, addr) != 1)
			throw Prefix_E_Invalid_Prefix();

		addr_set_unused_bits_to_zero(SIZE_MAX_IPV6);
	}
	else
	{
		// unknown address family
		throw Prefix_E_Invalid_Prefix();
	}
	return;
};
void Prefix::from_nb(const char* prefix_bin, int size_, int family_)
{
	addr_alloc(family_);
	size = size_;
	if(family == FAMILY_IPV4)
	{
		memcpy(addr, prefix_bin, SIZE_MAX_IPV4);
		addr_set_unused_bits_to_zero(SIZE_MAX_IPV4);
	}
	else if (family == FAMILY_IPV6)
	{
		memcpy(addr, prefix_bin, SIZE_MAX_IPV6);
		addr_set_unused_bits_to_zero(SIZE_MAX_IPV6);
	}
	else
	{
		cout << "Prefix::Prefix(const Prefix& other) UNKNOWN ADDRESS" << endl;
		throw "UNKNOWN ADDRESS";
	}
};
//-----------------------------------------------------------------------------
// TO
//-----------------------------------------------------------------------------
const string Prefix::to_s(void) const
{
	stringstream tmp;
	char addr_s[INET6_ADDRSTRLEN];
	if(family == FAMILY_IPV4)
	{
		if(inet_ntop(AF_INET, addr, addr_s, INET6_ADDRSTRLEN) == NULL )
			throw Prefix_E_Invalid_Prefix();
	}
	else if (family == FAMILY_IPV6)
	{

		if(inet_ntop(AF_INET6, addr, addr_s, INET6_ADDRSTRLEN) == NULL )
			throw Prefix_E_Invalid_Prefix();
	}
	else
	{
		strcpy(addr_s, "NOTDEF");
	}

	tmp << addr_s << "/" << size;
	return(string(tmp.str()));
};


//------------------------------------------------------------------------------
// COMPARE
//------------------------------------------------------------------------------
const int Prefix::common_bits (const Prefix& other) const
{
	// How many bits have these prefixes in common?
	// THIS  [1000101000|111111#******************]
	// OTHER [1000101000|111010#100101************]

	// # = common size [0,32/128] --> how long is the shorter prefix?
	// | = common bits [0,32/128] --> how many bits have both prefixes in common?

	int bits = 0;
	int common_size = (size <= other.size) ? size : other.size;
	assert(common_size >= 0 && common_size <= 128);

	// at least one of the prefixes has zero length (nothing in common)
	if(common_size == 0)
		return(0);

	// find the byte that differs
	int byte_idx_stop = (common_size-1) >> 3;
	int byte_idx = 0;
	while (byte_idx <= byte_idx_stop && addr[byte_idx] == other.addr[byte_idx])
		byte_idx++;

	// no byte differs
	if(byte_idx > byte_idx_stop)
		return(common_size);

	// at least one bit is different ... but witch one is it?
	unsigned char difference = addr[byte_idx] ^ other.addr[byte_idx];
	int bit_idx = 0;

	while(bit_idx < 8 && ( difference & (0x80 >> bit_idx) ) == 0)
		bit_idx++;

	// common bits = index of changed bit
	bits = bit_idx + (byte_idx << 3);

	if(bits > common_size)
		return(common_size);

	return(bits);
};
const bool Prefix::includes (const Prefix& other) const
{
	//cout << "INCLUDES -- is the OTHER prefix included in THIS prefix? " << endl; 
	//cout << "INCLUDES -- THIS : " << to_s() << endl;
	//cout << "INCLUDES -- OTHER: " << other.to_s() << endl;

	// Is the OTHER prefix included in THIS prefix ?
	// THIS  [1000101000111010100101************]
	// OTHER [1000101000111*********************]
	// --> true

	// 1. Size
	// The OTHER prefix is longer than THIS prefix.
	// ... so it cant be a subpart of THIS prefix
	if(other.size > size)
	{
		//cout << "INCLUDES -- FALSE (size) " << endl;
		return(false);
	}

	// 2. Content
	// byte per byte compare  
	// THIS  [1000101000111010000'**************]
	// OTHER [10001010001110111*****************]
	// --> false
	// THIS  [1000101000111011100'**************]
	// OTHER [10001010001110111*****************]
	// --> true

	// 2.a compare the bytes

	int byte_idx_stop = (other.size - 1) >> 3;
	int byte_idx = 0;
	//cout << "INCLUDES -- COMPARE 2.a first  FROM "<< byte_idx << " TO " << byte_idx_stop << " bytes" << endl;
	while(byte_idx <= byte_idx_stop && addr[byte_idx] == other.addr[byte_idx])
		byte_idx++;

	// some bytes are different
	if(byte_idx < byte_idx_stop) 
	{
		//cout << "INCLUDES -- FALSE (Byte "<< byte_idx << " differes)" << endl;
		return(false);
	}

	// all bytes are similar
	if(byte_idx > byte_idx_stop) 
	{
		//cout << "INCLUDES -- TRUE (all bytes)" << endl;
		return(true);
	}

	// 2.b compare the last byte 
	// ... mask all bits that are irrelevant for the comparison
	unsigned char difference = addr[byte_idx] ^ other.addr[byte_idx];
	int bit_idx = 0;
	int bit_idx_stop = (other.size - 1) & 0x07; // % 8;
	while(bit_idx <= bit_idx_stop && ( difference & (0x80 >> bit_idx) ) == 0)
		bit_idx++;

	if(bit_idx > bit_idx_stop)
	{
		//cout << "INCLUDES -- TRUE (some bytes + some bits)" << endl;
		return(true);
	}
	
	//cout << "INCLUDES -- FALSE (some equal bytes ... but some bits not)" << endl;
	return(false);
}

