/**
 * File: analyser_bgp.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */

#include "analyser_bgp.h"
Analyser_BGP::Analyser_BGP()
{
	// working folder
	working_folder_p = "~/error_default";
	track_weird_connections_folder_p = working_folder_p;
	// current time
	time_s = 0;

	// network address aggregation
	net_length_in = 32; // Hosts
	net_length_out = 0; // BGP
	prefix_length.set_value_not_found(-1);

	// signal traffic 
	router = -1;
	interface = -1;

	// track weird things
	track_weird_connection_bpp_min = 100;

	net_net_h.clear();
	stat_reset();
};
Analyser_BGP::~Analyser_BGP()
{

};
void Analyser_BGP::add_prefix(const string& prefix, int length)
{
	if( length < -1 or length > 128 or prefix.size() < 3)
	{
		cout << "ANALYSER BGP PREFIX ADD: Invalid data '" << prefix << "'  " << length << "  "<< endl;
		cout.flush();
		throw 200; // FIXME
	}
	Prefix p;
	p.from(prefix);
	prefix_length.insert(p, length);
};

void Analyser_BGP::get_net_net_key(
	Analyser_BGP_Key_Net_Net& net_key, 
	const Connection_Matrix_Key& cm_key
)
{
	Prefix p;
	int prefix_length_in;
	int prefix_length_out;
	// in 
	if(net_length_in != 0)
		prefix_length_in = net_length_in;
	else
	{ // use bgp
		prefix_length_in = 32;
		p.from_nb(cm_key.addr_in(), 32, Prefix::FAMILY_IPV4);
		prefix_length_in = prefix_length.lookup(p);
	}
	// out
	if(net_length_out != 0)
		prefix_length_out = net_length_out;
	else
	{ // use bgp
		prefix_length_out = 32;
		p.from_nb(cm_key.addr_out(), 32, Prefix::FAMILY_IPV4);
		prefix_length_out = prefix_length.lookup(p);
	}
	net_key.from(cm_key, prefix_length_in, prefix_length_out);
	return;
}
void Analyser_BGP::analyse_con_mat(const Connection_Matrix& con_mat)
{
	// Analyses the connection matrix
	// -- build net-net matrix
  // -- extract weird connections

	Connection_Matrix_HT::const_iterator iter, end;
	uint8_t state;
	bool signal_traffic;

  Analyser_BGP_Key_Net_Net net_key;
	stringstream weird_connections_path_ss;
	weird_connections_path_ss << track_weird_connections_folder_p.string() << "/" << time_s /(24*3600) << "/";
	path tmp = weird_connections_path_ss.str().c_str();
	create_directory(tmp);
	weird_connections_path_ss << "/" << time_s << ".csv";
	ofstream weird_connections_f((weird_connections_path_ss.str()).c_str(), ios_base::binary| ios_base::trunc);
	int bpp;

	iter = con_mat.connections_h.begin();
	end = con_mat.connections_h.end();
	while(iter != end)
	{
		// topology
		if(
			(router == -1 or (router == (iter->second).in_out_router)) and
			(interface == -1 or (interface == (iter->second).in_out_if))
		)
		{ // process this
			state = (iter->second).state;
			stat_5tps_processed++;	
		}
		else
		{
			// skip this
			iter++;
			continue;
		}

		// STATE of this connection?
		// Balanced++
		if(
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__EVENT or
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__SPAN or
			state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__EVENT // or
			// state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__SPAN
		)
		{ // account
			stat_5tps_balanced++;
			get_net_net_key(net_key,iter->first);
			net_net_h[net_key].balanced++;
		}
		// Unbalanced++
		else if (
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__NEVER or
			state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__NEVER
		)
		{
			signal_traffic = ( 
				(iter->first).protocol() == 6 and 
				(iter->first).port_out() == 80 and 
				(iter->first).port_in() > 1024
			);
			if(signal_traffic == true)
			{ 
				// WEIRD CONNECTION TRACKING
				bpp = 0;  // Attention please: Avoid division by zero !!
				if((iter->second).in_out_packets > 0) 
					bpp = (iter->second).in_out_bytes/(iter->second).in_out_packets;

				if(bpp < track_weird_connection_bpp_min)
				{// account
					stat_5tps_unbalanced++;
					get_net_net_key(net_key,iter->first);
					net_net_h[net_key].unbalanced++;
				}
				else
				{// ignore: but log connection
					stat_5tps_unbalanced_weird_connection++;
					string buf;
					(iter->first).to_s(buf);
					weird_connections_f << buf << ", ";
					(iter->second).to_s(buf);
					weird_connections_f << buf;
					(iter->second).to_s_router(buf);
					weird_connections_f << buf;
					weird_connections_f << endl;
				}
			}
			else
			{
				// ignore: unbalanced non signal traffic
				stat_5tps_unbalanced_not_signal++;
			}
		}
		else
		{ 
			// ignore: other
			//  -> not active over this observation window, 
			//  -> unbalanced but was once balanced
			//  -> unbalanced but out-in
		}
		iter++;
	}
	weird_connections_f.close();
	return;
}
void Analyser_BGP::analyse_net_net(void)
{
	// -- build net matrix
	Analyser_BGP_Net_Net_h::const_iterator iter, end;
	Analyser_BGP_Key_Net key;

	iter = net_net_h.begin();
	end = net_net_h.end();
	while(iter != end)
	{
		stat_net_net_items++;
		key.from((iter->first));

		if((iter->second).balanced == 0)
		{
			net_h[key].unbalanced++;
			stat_net_net_unbalanced++;
		}
		else
		{
			net_h[key].balanced++;
			stat_net_net_balanced++;
		}
		iter++;
	}
	return;
}
void Analyser_BGP::analyse_net(void)
{
	// -- extract unbalanced prefixes
	// -- build unbalance prefixes density
	Analyser_BGP_Net_h::const_iterator iter, end;
	string tmp;

	iter = net_h.begin();
	end = net_h.end();
	while(iter != end)
	{
		stat_net_items++;
		if((iter->second).balanced == 0)
		{
			stat_net_unbalanced++;
			(iter->first).to_s(tmp);
		}
		else
		{
			stat_net_balanced++;
		}
		iter++;
	}
	return;
};
void Analyser_BGP::export_unbalanced_prefixes(string& path)
{
	ofstream file;
	Analyser_BGP_Net_h::const_iterator iter, end;
	string tmp;

	file.open(path.c_str(), ios_base::out |ios_base::trunc);

	iter = net_h.begin();
	end = net_h.end();
	while(iter != end)
	{
		if((iter->second).balanced == 0)
		{
			(iter->first).to_s(tmp);
			file << tmp << ", " << (iter->second).unbalanced << endl;
		}
		iter++;
	}
	file.close();
	return;
}
void Analyser_BGP::export_density(string& path)
{
	ofstream file;
	Analyser_BGP_Net_h::const_iterator iter, end;
	string tmp;

	const int density_max = 100;
	int density[density_max];
	int density_sum = 0;
	for(int i = 0; i<density_max; i++)
		density[i] = 0;

	iter = net_h.begin();
	end = net_h.end();
	while(iter != end)
	{
		if((iter->second).balanced == 0)
		{
			if((iter->second).unbalanced >= density_max - 2)
			{
				density[density_max - 2]++;
			}
			else if((iter->second).unbalanced >= 0)
			{
				density_sum++;
				density[(iter->second).unbalanced]++;
			}
			else
			{
				cout << "ERROR: negative unbalance counter !?!" << endl;
				cout.flush();
				throw 200;
			}
		}
		iter++;
	}
	file.open(path.c_str(), ios_base::out |ios_base::trunc);
	for(int i = 0; i<density_max; i++)
	{
		file << i << ", " << density[i] << ", " << density_sum << endl;
		density_sum -= density[i];
	}
	file.close();
	return;
};

void Analyser_BGP::analyse(
	const Connection_Matrix& con_mat_, 
	uint64_t time_s_
)
{
	time_s = time_s_;
	
	// summary the cons to net net items
	analyse_con_mat(con_mat_);

	// statistics the cons to net net items
	analyse_net_net();

	// analyse the net matrix
	analyse_net();
};
//------------------------------------------------------------------------------
// STATISTICS
//------------------------------------------------------------------------------
void Analyser_BGP::stat_reset(void)
{
	net_net_h.clear();
	net_h.clear();

	stat_5tps_processed = 0;
	stat_5tps_balanced = 0;
	stat_5tps_unbalanced = 0;
	stat_5tps_unbalanced_weird_connection = 0;
	stat_5tps_unbalanced_not_signal = 0;

	stat_net_net_items = 0;
	stat_net_net_balanced = 0;
	stat_net_net_unbalanced = 0;

	stat_net_items = 0;
	stat_net_balanced = 0;
	stat_net_unbalanced = 0;

	return;
}

//******************************************************************************
// RUBY
//******************************************************************************
void rb_analyser_bgp_free(void *p)
{
	delete (Analyser_BGP*) p;
	p = NULL;
};
VALUE rb_analyser_bgp_alloc(VALUE klass)
{
	Analyser_BGP* p = NULL;
	VALUE obj;
	p = new Analyser_BGP();
	if (p == NULL)
		rb_raise(rb_path2class("FilterInOutError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_analyser_bgp_free, p);
	return(obj);
};

//------------------------------------------------------------------------------
// configuration
//------------------------------------------------------------------------------
VALUE rb_analyser_bgp_set_working_folder(VALUE self, VALUE path)
{
	RB_ANALYSER_BGP_UNWRAP
	string working_path_s = (RSTRING_PTR(path));
	abgp->set_working_path(working_path_s);
	return(self);	
}
VALUE rb_analyser_bgp_set_net_sizes(VALUE self, VALUE in, VALUE out)
{
	RB_ANALYSER_BGP_UNWRAP
	abgp->set_net_in(NUM2INT(in));
	abgp->set_net_out(NUM2INT(out));
	return(self);
}
VALUE rb_analyser_bgp_add_prefix(VALUE self, VALUE prefix, VALUE as)
{
	RB_ANALYSER_BGP_UNWRAP
	string prefix_s = RSTRING_PTR(prefix);
	abgp->add_prefix(prefix_s , NUM2INT(as));
	return(self);
};
VALUE rb_analyser_bgp_set_router(VALUE self, VALUE router)
{
	RB_ANALYSER_BGP_UNWRAP
	abgp->set_router(NUM2INT(router));
	return(self);
}
VALUE rb_analyser_bgp_set_interface(VALUE self, VALUE interface)
{
	RB_ANALYSER_BGP_UNWRAP
	abgp->set_interface(NUM2INT(interface));
	return(self);
}

//------------------------------------------------------------------------------
// analyser
//------------------------------------------------------------------------------
VALUE rb_analyser_bgp_export_unbalanced_prefixes(VALUE self, VALUE file_p)
{
	RB_ANALYSER_BGP_UNWRAP
	string file_ps = RSTRING_PTR(file_p);
	abgp->export_unbalanced_prefixes(file_ps);
	return(self);
}
VALUE rb_analyser_bgp_export_density(VALUE self, VALUE file_p)
{
	RB_ANALYSER_BGP_UNWRAP
	string file_ps = RSTRING_PTR(file_p);
	abgp->export_density(file_ps);
	return(self);
};

VALUE rb_analyser_bgp_analyse(VALUE self, VALUE con_matrix, VALUE time_s)
{
	RB_ANALYSER_BGP_UNWRAP
	Connection_Matrix* con_matrix_;
	Data_Get_Struct(con_matrix, Connection_Matrix, con_matrix_);
	abgp->analyse(*con_matrix_, NUM2ULL(time_s));
	return(self);
};

//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_analyser_bgp_stat_get(VALUE self)
{
	RB_ANALYSER_BGP_UNWRAP
	VALUE array = rb_ary_new();
	rb_ary_push(array, ULL2NUM(abgp->stat_5tps_processed));
	rb_ary_push(array, ULL2NUM(abgp->stat_5tps_balanced));
	rb_ary_push(array, ULL2NUM(abgp->stat_5tps_unbalanced));
	rb_ary_push(array, ULL2NUM(abgp->stat_5tps_unbalanced_weird_connection));
	rb_ary_push(array, ULL2NUM(abgp->stat_5tps_unbalanced_not_signal));

	rb_ary_push(array, ULL2NUM(abgp->stat_net_net_items));
	rb_ary_push(array, ULL2NUM(abgp->stat_net_net_balanced));
	rb_ary_push(array, ULL2NUM(abgp->stat_net_net_unbalanced));

	rb_ary_push(array, ULL2NUM(abgp->stat_net_items));
	rb_ary_push(array, ULL2NUM(abgp->stat_net_balanced));
	rb_ary_push(array, ULL2NUM(abgp->stat_net_unbalanced));

	return(array);
};
VALUE rb_analyser_bgp_stat_reset(VALUE self)
{
	RB_ANALYSER_BGP_UNWRAP
	abgp->stat_reset();
	return(self);
};


