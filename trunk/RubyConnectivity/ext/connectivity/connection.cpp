/**
 * File: data_parser.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "connection.h"
Connection::Connection()
{
	valid = false;
};
Connection::~Connection()
{

};
int Connection::import_from_m_data_cube(char * b)
{
		// is this connection a valid data structure
		// instead of delete the data structure, just mark it as invalid
		// and reuse it later ... 

		// protocol
		protocol = *((uint8_t *)(b+40));
		port_src = ntohs(*((uint16_t *)(b+32)));
		port_dst = ntohs(*((uint16_t *)(b+34)));

		// timing
		uint64_t bla = ntohll(*((uint64_t *)(b+0)));
		start_s = bla/1000;
		stop_s  = ntohl(*((uint32_t *)(b+8)));
		stop_s = start_s + stop_s/1000;

		// router
		router = *((uint8_t *)(b+43));
		if_in = ntohs(*((uint16_t *)(b+36)));
		if_out = ntohs(*((uint16_t *)(b+38)));
		CONNECTION_ADDR_IMPORT_IPV4(addr_next, b + 28);

		// network
		direction = CONNECTION_DIRECTION_UNKNOWN;
		border = CONNECTION_BORDER_UNKNOWN;

		// content
		packets = ntohl(*((uint32_t *)(b+24)));
		bytes = ntohl(*((uint32_t *)(b+20)));

		// (ip) addr of the hosts (network byte order)
		addr_length = *((uint8_t *)(b+45));
		if (addr_length == 4)
		{	
			
			CONNECTION_ADDR_IMPORT_IPV4(addr_src, b + 46);
			CONNECTION_ADDR_IMPORT_IPV4(addr_dst, b + 50); 
			valid = true; 
			return(CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV4); 
		}
		else if (addr_length == 16)
		{
			CONNECTION_ADDR_IMPORT_IPV6(addr_src, b + 46);
			CONNECTION_ADDR_IMPORT_IPV6(addr_dst, b + 62);
			valid = true; 
			return(CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV6);
		}
		else
		{
			valid = false;
			throw 100; // FIXME
			return(0);
		}
		// tos = *((uint8_t *)(b+41));
		// flags = *((uint8_t *)(b+42));
		// router_engine_id  = *((uint8_t *)(b+44));
};

int Connection::import_from_nfdump_data(int proto, char addrsrc, int portsrc, char addrdst, int portdst, int ts, int te, char nh, int intface_in, int intface_out, int pck, int byts)
{
		// is this connection a valid data structure
		// instead of delete the data structure, just mark it as invalid
		// and reuse it later ... 

		// protocol
		protocol = (uint8_t) proto;
		port_src = (uint16_t) portsrc;
		port_dst = (uint16_t) portdst;

		// timing
		start_s = (uint32_t) ts;
		stop_s  = (uint32_t) te;

		// router
		router = 0;
		if_in = (uint16_t) intface_in;
		if_out = (uint16_t) intface_out;
		
		if (inet_pton(AF_INET, nh, &addr_next)==0){
			if (inet_pton(AF_INET6, nh, &addr_next)==-1 || inet_pton(AF_INET6, nh, &addr_next)==0 ){
				throw 100; // FIXME
				return(0);
			}
		}

		// network
		direction = CONNECTION_DIRECTION_UNKNOWN;
		border = CONNECTION_BORDER_UNKNOWN;

		// content
		packets = (uint32_t) pck;
		bytes = (uint32_t) byts;

		// (ip) addr of the hosts (network byte order)
		
		if (inet_pton(AF_INET, addrsrc, &addr_src)==1){
			addr_length = 4;
			valid = true;
			return(1);
		}
		else{
			if (inet_pton(AF_INET6, addrsrc, &addr_src)==1){
				addr_length = 16;
				valid = true;
				return(1);
			}else{
				valid = false;
				throw 100; // FIXME
				return(0);
			}
			
		}
};


/*
void Connection::import_from_m_custom_flow__(char* b)
{
	ip_src    = ntohl( *((uint32_t *)(b   )));
	port_src  = ntohs( *((uint16_t *)(b+4 )));
	ip_dst    = ntohl( *((uint32_t *)(b+6 )));
	port_dst  = ntohs( *((uint16_t *)(b+10)));
	protocol  =        *((uint8_t  *)(b+12));

	start_s   = ntohl( *((uint32_t *)(b+13)));
	stop_s    = ntohl( *((uint32_t *)(b+17)));

	packets   = ntohl( *((uint32_t *)(b+21)));
	bytes     = ntohl( *((uint32_t *)(b+25)));
	router    =        *((uint8_t *) (b+29)) ;
	if_in     = ntohs( *((uint16_t *)(b+30)));
	if_out    = ntohs( *((uint16_t *)(b+32)));

	direction = CONNECTION_DIRECTION_UNKNOWN;
	border    = CONNECTION_BORDER_UNKNOWN;

	return;
}
*/
//******************************************************************************
// RUBY
//******************************************************************************
void rb_connection_free(void *p)
{
	delete (Connection*) p;
	p = NULL;
};
VALUE rb_connection_alloc(VALUE klass)
{
	Connection* p = NULL;
	VALUE obj;
	p = new Connection();
	if (p == NULL)
    		rb_raise(rb_path2class("ConnectionError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_connection_free, p);
	return(obj);
};
/*
VALUE rb_setter_ip_src_hn(VALUE self, VALUE ip_src_hn)
{
	RB_CONNECTION_UNWRAP
	con->ip_src = NUM2ULL(ip_src_hn);
	return(self);
};
VALUE rb_setter_ip_dst_hn(VALUE self, VALUE ip_dst_hn)
{
	RB_CONNECTION_UNWRAP
	con->ip_dst = NUM2ULL(ip_dst_hn);
	return(self);
};
VALUE rb_setter_ip_next_hn(VALUE self, VALUE ip_next_hn)
{
	RB_CONNECTION_UNWRAP
	con->ip_next = NUM2ULL(ip_next_hn);
	return(self);
};
VALUE rb_setter_port_src_hn(VALUE self, VALUE port_src_hn)
{
	RB_CONNECTION_UNWRAP
	con->port_src = NUM2ULL(port_src_hn);
	return(self);
};
VALUE rb_setter_port_dst_hn(VALUE self, VALUE port_dst_hn)
{
	RB_CONNECTION_UNWRAP
	con->port_dst = NUM2ULL(port_dst_hn);
	return(self);
};
VALUE rb_setter_protocol(VALUE self, VALUE protocol)
{
	RB_CONNECTION_UNWRAP
	con->protocol = NUM2ULL(protocol);
	return(self);
};
VALUE rb_setter_start_s(VALUE self, VALUE start_s)
{
	RB_CONNECTION_UNWRAP
	con->start_s = NUM2ULL(start_s);
	return(self);
};
VALUE rb_setter_stop_s(VALUE self, VALUE stop_s)
{
	RB_CONNECTION_UNWRAP
	con->stop_s = NUM2ULL(stop_s);
	return(self);
};
VALUE rb_setter_packets(VALUE self, VALUE packets)
{
	RB_CONNECTION_UNWRAP
	con->packets = NUM2ULL(packets);
	return(self);
};
VALUE rb_setter_bytes(VALUE self, VALUE bytes)
{
	RB_CONNECTION_UNWRAP
	con->bytes = NUM2ULL(bytes);
	return(self);
};
VALUE rb_setter_router(VALUE self, VALUE router)
{
	RB_CONNECTION_UNWRAP
	con->router = NUM2ULL(router);
	return(self);
};
VALUE rb_setter_if_in(VALUE self, VALUE if_in)
{
	RB_CONNECTION_UNWRAP
	con->if_in = NUM2ULL(if_in);
	return(self);
};
VALUE rb_setter_if_out(VALUE self, VALUE if_out)
{
	RB_CONNECTION_UNWRAP
	con->if_out = NUM2ULL(if_out);
	return(self);
};
VALUE rb_setter_direction(VALUE self, VALUE direction)
{
	RB_CONNECTION_UNWRAP
	con->direction = NUM2ULL(direction);
	return(self);
};
VALUE rb_setter_border(VALUE self, VALUE border)
{
	RB_CONNECTION_UNWRAP
	con->border = NUM2ULL(border);
	return(self);
};
*/

//------------------------------------------------------------------------------
VALUE rb_getter_valid(VALUE self)
{
	RB_CONNECTION_UNWRAP
	if(con->valid == true)
		return(Qtrue);
	else
		return(Qfalse);
}
VALUE rb_getter_addr_src_s(VALUE self)
{
	RB_CONNECTION_UNWRAP
	string buf;
	try
	{
		Connection::to_s(buf, con->addr_src, con->addr_length);
	}
	catch (eConnection_Unknown_Addr_Type e)
	{
		rb_raise(rb_path2class("AddrError"), "Unknown Addr Type");
	}
	return(rb_str_new2(buf.c_str()));
};
VALUE rb_getter_addr_dst_s(VALUE self)
{
	RB_CONNECTION_UNWRAP
	string buf;
	try
	{
		Connection::to_s(buf, con->addr_dst, con->addr_length);
	}
	catch (eConnection_Unknown_Addr_Type e)
	{
		rb_raise(rb_path2class("AddrError"), "Unknown Addr Type");
	}
	return(rb_str_new2(buf.c_str()));
};
VALUE rb_getter_addr_next_s(VALUE self)
{
	RB_CONNECTION_UNWRAP
	string buf;
	try
	{
		Connection::to_s(buf, con->addr_next, con->addr_length);
	}
	catch (eConnection_Unknown_Addr_Type e)
	{
		rb_raise(rb_path2class("AddrError"), "Unknown Addr Type");
	}
	return(rb_str_new2(buf.c_str()));
};

VALUE rb_getter_port_src(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->port_src));
};
VALUE rb_getter_port_dst(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->port_dst));
};
VALUE rb_getter_protocol(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->protocol));
};
VALUE rb_getter_start_s(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->start_s));
};
VALUE rb_getter_stop_s(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->stop_s));
};
VALUE rb_getter_bytes(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->bytes));
};
VALUE rb_getter_packets(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->packets));
};
VALUE rb_getter_router(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->router));
};
VALUE rb_getter_if_in(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->if_in));
};
VALUE rb_getter_if_out(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->if_out));
};
VALUE rb_getter_direction(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->direction));
};
VALUE rb_getter_border(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->border));
};
VALUE rb_getter_addr_length(VALUE self)
{
	RB_CONNECTION_UNWRAP
	return(ULL2NUM(con->addr_length));
};


//------------------------------------------------------------------------------
// IMPORT
//------------------------------------------------------------------------------
VALUE rb_nf_import(
	VALUE self,
	VALUE proto, 
	VALUE addrsrc,
	VALUE portsrc,
	VALUE addrdst,
	VALUE portdst,
	VALUE ts,
	VALUE te,
	VALUE addr_router,
	VALUE intface_in,
	VALUE intface_out,
	VALUE pck,
	VALUE byts)
{
	RB_CONNECTION_UNWRAP
	int protocol = NUM2INT(proto);
	char* src_addr = RSTRING(addrsrc)->ptr;
	int src_port = NUM2INT(portsrc);
	char* dst_addr = RSTRING(addrdst)->ptr;
	int dst_port = NUM2INT(portdst);
	
	int time_s = NUM2INT(ts);
	int time_e = NUM2INT(te);
	
	char* nh = RSTRING(addr_router)->ptr;
	int in_interface = NUM2INT(intface_in);
	int out_interface = NUM2INT(intface_out);
	
	int packets = NUM2INT(pck);
	int bytes = NUM2INT(byts);
	
	int res = con->import_from_nfdump_data(proto, *src_addr, src_port, *dst_addr, dst_port, time_s, time_e, *nh , in_interface, out_interface, packets, bytes);
	if (res!=1){
		rb_raise(rb_path2class("NFDUMPDataParserError"), "INVALID NFDUMP CONNECTION IMPORTED!");
		
	}
	


}


