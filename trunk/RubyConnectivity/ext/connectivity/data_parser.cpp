/**
 * File: data_parser.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */
#include "data_parser.h"
Data_Parser::Data_Parser()
{
	// statistics
	time_s = 0;
	stat_next_export_s = 0;
	stat_reset();
};
Data_Parser::~Data_Parser(){};

//------------------------------------------------------------------------------
// STATISTICS
//------------------------------------------------------------------------------
void Data_Parser::stat_reset(void)
{
	stat_connections_processed = 0;
	return;
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_data_parser_free(void *p)
{
	delete (Data_Parser*) p;
	p = NULL;
};
VALUE rb_data_parser_alloc(VALUE klass)
{
	Data_Parser* p = NULL;
	VALUE obj;
	p = new Data_Parser();
	if (p == NULL)
  		rb_raise(rb_path2class("DataParserError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_data_parser_free, p);
	return(obj);
};

// Statistics
VALUE rb_data_parser_stat_get(VALUE self)
{
	RB_DATA_PARSER_UNWRAP
	VALUE array = rb_ary_new();

	rb_ary_push(array, ULL2NUM(dp->time_s));

	rb_ary_push(array, ULL2NUM(dp->stat_connections_processed));
	return(array);
};
VALUE rb_data_parser_stat_reset(VALUE self)
{
	RB_DATA_PARSER_UNWRAP
	dp->stat_reset();
	return(self);
};
VALUE rb_data_parser_stat_next_export_s(VALUE self, VALUE next_export_s)
{
	RB_DATA_PARSER_UNWRAP
	dp->stat_next_export_s = NUM2UINT(next_export_s);
	return(self);
};

// PARSE DATA
VALUE rb_data_parser_parse_file(
	VALUE self, 
	VALUE file_p,
	VALUE con_block
)
{
	RB_DATA_PARSER_UNWRAP

	// the container
	Connections* cons;
	Connection* con;
	Data_Get_Struct(con_block, Connections, cons);

	// iterator
	con = cons->get_first_unused();
	if(con == NULL)
	{ // no space left ...
		rb_yield(con_block);
		cons->reset();
		con = cons->get_first_unused();
		if(con == NULL)
		{
			rb_raise(rb_path2class("DataParserError"), "A misconfigured 'Connections' Container ");
		}
	}

	// input stream
	Check_Type(file_p, T_STRING);

  boost::iostreams::filtering_istream in;
	ifstream input_f;
	string file_path_s = (RSTRING_PTR(file_p));
	// decompression
	if(file_path_s.find(".gz") != string::npos)
	  in.push(boost::iostreams::gzip_decompressor());

	if(file_path_s.find(".bz2") != string::npos)
	  in.push(boost::iostreams::bzip2_decompressor());

	// file
	input_f.open(file_path_s.c_str(), ifstream::in | ifstream::binary);
  in.push(input_f);

	// the buffer
	char b[CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV6];
	size_t b_count = 0;

	if(input_f.is_open())
	{
		while(true)
		{
			// get the first chunk of data
			in.read(b, CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV4);
			b_count = in.gcount();

			if(b_count != CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV4)
			{
				if(b_count == 0) //EOF
					break;

				// Error
				rb_raise(rb_path2class("DataParserError"), "Brocken Data Stream: Some unused bytes in io-pipe");
			}
			uint8_t addr_type = *( (uint8_t*) (b + CONNECTION_FF_M_DATACUBE_ADDR_TYPE_POS));
			if(addr_type == 4)
			{
				if(con->import_from_m_data_cube(b) != CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV4)
					break;
			}
			else if(addr_type == 16)
			{
				in.read(b+CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV4, CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_DELTA);
				b_count += in.gcount();
				if(b_count != CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV6)
				{
					rb_raise(rb_path2class("DataParserError"), "Brocken Data Stream: Loading the delta bytes for IPv6 failed");
					break;
				}
				if(con->import_from_m_data_cube(b) != CONNECTION_FF_M_DATACUBE_FLOW_BIN_SIZE_IPV6)
				{
					rb_raise(rb_path2class("DataParserError"), "Brocken Data Stream: Parsing of IPv6 data failed");
					break;
				}
			}
			else
			{
				rb_raise(rb_path2class("DataParserError"), "Brocken Data Stream: Unknown addr type");
				break;
			}
			dp->stat_connections_processed++;

			// periodic stat export					
			if (con->start_s > dp->stat_next_export_s)
			{
				// update the time
				dp->time_s = con->start_s;

				// lets call the export function (ruby)
				ID method_id = rb_intern("statistics_export");
				rb_funcall(self, method_id, 0);
			}	

			// next			
			con = cons->get_next_unused();
			if(con == NULL)
			{
				rb_yield(con_block);
				cons->reset();
				con = cons->get_first_unused();
				if (con == NULL)
				{
					rb_raise(rb_path2class("DataParserError"), "A misconfigured 'Connections' ");
				}
			}
		}
		input_f.close();
	}
	// last connections
	rb_yield(con_block);
	return(self);
};
// PARSE DATA
VALUE rb_data_parser_nf_parse(
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
	VALUE byts,
	VALUE con_block
)
{
	RB_DATA_PARSER_UNWRAP

	// the container
	Connections* cons;
	Connection* con;
	Data_Get_Struct(con_block, Connections, cons);

	// iterator
	con = cons->get_first_unused();
	if(con == NULL)
	{ // no space left ...
		rb_yield(con_block);
		cons->reset();
		con = cons->get_first_unused();
		if(con == NULL)
		{
			rb_raise(rb_path2class("DataParserError"), "A misconfigured 'Connections' Container ");
		}
	}
	int protocol = NUM2INT(proto);
	char* src_addr = RSTRING_PTR(addrsrc);
	int src_port = NUM2INT(portsrc);
	char* dst_addr = RSTRING_PTR(addrdst);
	int dst_port = NUM2INT(portdst);
	
	int time_s = NUM2INT(ts);
	int time_e = NUM2INT(te);
	
	char* nh = RSTRING_PTR(addr_router);
	int in_interface = NUM2INT(intface_in);
	int out_interface = NUM2INT(intface_out);
	
	int packets = NUM2INT(pck);
	int bytes = NUM2INT(byts);
	
	int res = con->import_from_nfdump_data(protocol, src_addr, src_port, dst_addr, dst_port, time_s, time_e, nh , in_interface, out_interface, packets, bytes);
	if (res==1){
		return(1);
	}else{
		rb_raise(rb_path2class("NFDUMPDataParserError"), "INVALID NFDUMP CONNECTION IMPORTED!");
		return(0);
	}
	
	dp->stat_connections_processed++;
	// periodic stat export					
	if (con->start_s > dp->stat_next_export_s)
	{
		// update the time
		dp->time_s = con->start_s;

		// lets call the export function (ruby)
		ID method_id = rb_intern("statistics_export");
		rb_funcall(self, method_id, 0);
	}
	
	// last connections
	rb_yield(con_block);
	return(self);
};
