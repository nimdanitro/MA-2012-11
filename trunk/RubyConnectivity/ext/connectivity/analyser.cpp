/**
 * File: analyser.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * 
 */

#include "analyser.h"
Analyser::Analyser()
{
	working_p = "~/RubyConnectivityDefaultOut";
	weird_connections_folder_p = working_p / "weird_connections";
	stat_host_host_folder_p = working_p / "host_host";
	stat_host_folder_p = working_p / "host";
	stat_net_folder_p = working_p / "net";
	stat_bgp_folder_p = working_p / "bgp";
	prefixes_host_folder_p = working_p / "prefix_host";
	prefixes_net_folder_p = working_p / "prefix_net";
	prefixes_bgp_folder_p = working_p / "prefix_bgp";

	weird_connection_bpp_min = 100;
	bgp_prefixes.set_value_not_found(-1);
	for(int i=0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
			agg_host_host_h[i] = NULL;
			agg_host_h[i] = NULL;
			agg_net_h[i] = NULL;
			agg_bgp_h[i] = NULL;
	};
};
Analyser::~Analyser()
{
	reset();
	for(int i=0; i< ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_host_host_h[i] != NULL)
		{
			delete agg_host_host_h[i];
			delete agg_host_h[i];
			delete agg_net_h[i];
			delete agg_bgp_h[i];

			agg_host_host_h[i] = NULL;
			agg_host_h[i] = NULL;
			agg_net_h[i] = NULL;
			agg_bgp_h[i] = NULL;
		}
	};
};
void Analyser::reset()
{
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_host_host_h[i] != NULL)
		{
			agg_host_host_h[i]->clear();
			agg_host_h[i]->clear();
			agg_net_h[i]->clear();
			agg_bgp_h[i]->clear();
		}
	};
  agg_host_all_h.clear();
  agg_net_all_h.clear();
  agg_bgp_all_h.clear();

	// statistics

	// connections
	stat_cons_processed = 0;

	// interfaces
	stat_cons_if_invalid = 0;
	stat_cons_if_zero = 0;
	stat_cons_if_not_monitored = 0;
	stat_cons_if_monitored = 0;

	// state
	stat_cons_state_processed = 0;
	stat_cons_state_balanced = 0;
	stat_cons_state_unbalanced = 0;
	stat_cons_state_other = 0;

	// weird connections
  stat_cons_weird_cons_processed = 0;
	stat_cons_weird_cons_no_rule = 0;
	stat_cons_weird_cons_zero_packet = 0;
	stat_cons_weird_cons_bpp = 0;
	stat_cons_weird_cons_ok = 0;

	// signal traffic
	stat_cons_signal_processed = 0;
	stat_cons_signal_ok = 0;
	stat_cons_signal_other = 0;

	// accounted
	stat_cons_accounted_unbalanced = 0;

	return;
};
// -----------------------------------------------------------------------------
// CONFIG
// -----------------------------------------------------------------------------
void Analyser::set_working_path(string path_s)
{
	working_p = path_s.c_str();
	create_directory(working_p);
	weird_connections_folder_p = working_p / "weird_connections";

	create_directory(weird_connections_folder_p);
	stat_host_host_folder_p = working_p / "host_host";
	create_directory(stat_host_host_folder_p);
	stat_host_folder_p = working_p / "host";
	create_directory(stat_host_folder_p);
	stat_net_folder_p = working_p / "net";
	create_directory(stat_net_folder_p);
	stat_bgp_folder_p = working_p / "bgp";
	create_directory(stat_bgp_folder_p);

	prefixes_host_folder_p = working_p / "prefix_host";
	create_directory(prefixes_host_folder_p);
	prefixes_net_folder_p = working_p / "prefix_net";
	create_directory(prefixes_net_folder_p);
	prefixes_bgp_folder_p = working_p / "prefix_bgp";
	create_directory(prefixes_bgp_folder_p);

};
void Analyser::add_interface(int router, int interface)
{
	if(router >= ANALYSER_MAX_ROUTER or interface >= ANALYSER_MAX_INTERFACE)
	{
		cerr << "Analyser::add_interface: out of band error :: R:" << router << " I:" << interface  << endl;
		cerr.flush();
		throw 200; // FIXME
	}
	agg_host_host_h[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_Net_h;
	agg_host_h[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;
	agg_net_h[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;
	agg_bgp_h[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;

	return;
};
void Analyser::add_prefix(const string& prefix, int length)
{
	//cout << "ADD: " << prefix << " :: " << length << endl;
	//cout.flush();

	if(length < -1 or length > 128 or prefix.size() < 3)
	{
		cerr << " Analyser:add_prefix:: Invalid Data :: '" << prefix << "'  " << length << "  "<< endl;
		cerr.flush();
		throw 200; // FIXME
	}
	Prefix p;
	p.from(prefix);
//	cout << "PREFIX: "<< p.to_s() << endl;
//	cout.flush();
	bgp_prefixes.insert(p, length);
	return;
};

// -----------------------------------------------------------------------------
// WORK
// -----------------------------------------------------------------------------
void Analyser::analyse(
	const Connection_Matrix& con_mat_, 
	uint64_t time_s_
)
{
	analyse_connections(con_mat_,time_s_);
	analyse_host_host(time_s_);
	analyse_host(time_s_);
	analyse_net(time_s_);
	analyse_bgp(time_s_);
};

void Analyser::weird_connection_log (
	int status,
	const Connection_Matrix_Key& key, 
	const Connection_Matrix_Entry& entry,
	ofstream& file
)
{
	string buf;
	file << status << ", ";
	key.to_s(buf);
	file << buf << ", ";
	entry.to_s(buf);
	file << buf;
	entry.to_s_router(buf);
	file << buf;
	file << endl;
};

void Analyser::analyse_connections(
	const Connection_Matrix& con_mat, 
	uint64_t time_s
)
{
	// Analyses the connection matrix
	// -- build host-host table
  Analyser_Key_Net_Net host_host_key;
	Connection_Matrix_HT::const_iterator iter, end;
	iter = con_mat.connections_h.begin();
	end = con_mat.connections_h.end();

  // -- extract weird unbalanced signal connections
	stringstream weird_connections_path_ss;
	weird_connections_path_ss << weird_connections_folder_p.string();
	weird_connections_path_ss << "/" << time_s /(24*3600) << "/";
	path tmp = weird_connections_path_ss.str().c_str();
	create_directory(tmp);
	weird_connections_path_ss << "/" << time_s << ".csv";
	ofstream weird_connections_f(
		(weird_connections_path_ss.str()).c_str(),
		ios_base::trunc
	);

	uint8_t state;
	Analyser_Net_Net_h* host_host_p;
	int bpp;

	while(iter != end)
	{

		stat_cons_processed++;

//------------------------------------------------------------------------------
// INTERFACE -------------------------------------------------------------------
//------------------------------------------------------------------------------
		// Valid, Monitored ?
		if(
			(iter->second).in_out_router >= ANALYSER_MAX_ROUTER or 
			(iter->second).in_out_router < 0 or 
			(iter->second).in_out_if >=  ANALYSER_MAX_INTERFACE or
			(iter->second).in_out_if < 0 
		)
		{
			cerr << "Analyser::add_interface: out of band error :: ";
			cerr << "R:" << (iter->second).in_out_router << " ";
			cerr << "I:" << (iter->second).in_out_if << endl;
			cerr.flush();
			throw 200; // FIXME

			stat_cons_if_invalid++;
			iter++;
			continue;			
		}
		if ( 
			(iter->second).in_out_router == 0 and
			(iter->second).in_out_if == 0
		)
		{
			stat_cons_if_zero++;
			iter++;
			continue;
		}
		host_host_p = agg_host_host_h[(iter->second).in_out_router*ANALYSER_MAX_INTERFACE + (iter->second).in_out_if];
		if(host_host_p == NULL)
		{
			stat_cons_if_not_monitored++;
			iter++;
			continue;			
		}
		stat_cons_if_monitored++;

//------------------------------------------------------------------------------
// STATE -----------------------------------------------------------------------
//------------------------------------------------------------------------------

		// STATE of this connection?
		stat_cons_state_processed++;
		state = iter->second.state;
		if( // Balanced
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__EVENT or
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__SPAN or
			state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__EVENT
		)
		{
			host_host_key.from(iter->first, 32, 32);
			(*host_host_p)[host_host_key].balanced++;

			stat_cons_state_balanced++;
			iter++;
			continue;
		}
		else if ( // Unbalanced
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__NEVER or
			state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__NEVER
		)
		{
			stat_cons_state_unbalanced++;
		}
		else
		{ // Other
			stat_cons_state_other++;
			iter++;
			continue;
		}
		// only state unbalanced survives

//------------------------------------------------------------------------------
// WEIRD CONNECTIONS -----------------------------------------------------------
//------------------------------------------------------------------------------
// Packet Loss, Measurement Infrastructure Loss
		// TCP
		stat_cons_weird_cons_processed++;
		if( (iter->first).protocol() != 6 )
		{ // non TCP so no rules (at the moment)
			stat_cons_weird_cons_no_rule++;
			iter++;
			continue;
		}

		// ZERO PACKET
		if((iter->second).in_out_packets == 0) 
		{
			weird_connection_log(0, iter->first, iter->second, weird_connections_f);
			stat_cons_weird_cons_zero_packet++;
			iter++;
			continue;
		}

		// TOO BIG Byte Per Packet RATE
		bpp = (iter->second).in_out_bytes/(iter->second).in_out_packets;
		if(bpp > weird_connection_bpp_min)
		{
			weird_connection_log(1, iter->first, iter->second, weird_connections_f);
			stat_cons_weird_cons_bpp++;
			iter++;
			continue;
		}
		stat_cons_weird_cons_ok++;

//------------------------------------------------------------------------------
// SINGAL TRAFFIC --------------------------------------------------------------
//------------------------------------------------------------------------------
		stat_cons_signal_processed++;
		if( 
			(iter->first).protocol() != 6 or
			(iter->first).port_out() != 80 or 
			(iter->first).port_in()  < 1024
		)
		{ // non signal traffic
			stat_cons_signal_other++;
			iter++;
			continue;
		}
		stat_cons_signal_ok++;
//------------------------------------------------------------------------------
// ACCOUNT THE REST --------------------------------------------------------------
//------------------------------------------------------------------------------

		// ACCOUNT the unbalanced connection
		host_host_key.from(iter->first, 32, 32);
		(*host_host_p)[host_host_key].unbalanced++;
		stat_cons_accounted_unbalanced++;

		iter++;
		continue;

		// THIS LINE SHOULD NEVER BE REACHED
		throw 1000;
	}
	weird_connections_f.close();
	return;
};
void Analyser::analyse_host_host(uint64_t time_s)
{
	Analyser_Net_Net_h::iterator iter, end;
	Analyser_Key_Net key;
	string buf;

	uint64_t all_hosts_hosts_balanced = 0;
	uint64_t all_hosts_hosts_unbalanced = 0;

	Prefix p;
	int bgp_length;

	// aggregate the host-host tabels to host tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{

		if(agg_host_host_h[i] == NULL)
			continue;
		
		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interface_hosts_hosts_balanced = 0;
		uint64_t interface_hosts_hosts_unbalanced = 0;

		// output
		Analyser_Net_h& current_host_table_h(*(agg_host_h[i]));
		iter = agg_host_host_h[i]->begin();
		end = agg_host_host_h[i]->end();
		while(iter != end)
		{
			key.from(iter->first, 32);
			
			Analyser_Entry_Net& all = agg_host_all_h[key];
			Analyser_Entry_Net& intef = current_host_table_h[key];

			if(all.bgp_length == -1)
			{
				// resolve it
				p.from_nb(key.addr(), 32, Prefix::FAMILY_IPV4);
				bgp_length = bgp_prefixes.lookup(p);

				if(bgp_length == ANALYSER_BGP_AGGREGATION_UNKNOWN)
				{
					key.to_s(buf);
					bgp_length = ANALYSER_BGP_AGGREGATION_NOT_ANNOUNCED;
				}
				all.bgp_length = bgp_length;
				intef.bgp_length = bgp_length;
			}
			else
			{
				intef.bgp_length = all.bgp_length;
			}
			if((iter->second).balanced > 0)
			{
				all_hosts_hosts_balanced++;
				interface_hosts_hosts_balanced++;
				all.balanced++;
				intef.balanced++;
			}
			else
			{
				all_hosts_hosts_unbalanced++;
				interface_hosts_hosts_unbalanced++;
				all.unbalanced++;
				intef.unbalanced++;
			}
			iter++;
		}

		// log statistics
		stringstream host_host_path_ss;
		host_host_path_ss << stat_host_host_folder_p.string();
		host_host_path_ss << "/r_" << router << "_i_" << interface;
		host_host_path_ss << ".csv";
		ofstream host_host_f(
			(host_host_path_ss.str()).c_str(),
			ios_base::app
		);
		host_host_f << time_s << ", ";
		host_host_f << interface_hosts_hosts_balanced << ", ";
		host_host_f << interface_hosts_hosts_unbalanced << ", ";
		host_host_f << endl;
		host_host_f.close();

		agg_host_host_h[i]->clear(); // free memory as soon as possible ...
	}; //interfaces

	// log statistics
	stringstream host_host_all_path_ss;
	host_host_all_path_ss << stat_host_host_folder_p.string();
	host_host_all_path_ss << "/all.csv";
	ofstream host_host_all_f(
		(host_host_all_path_ss.str()).c_str(),
		ios_base::app
	);
	host_host_all_f << time_s << ", ";
	host_host_all_f << all_hosts_hosts_balanced << ", ";
	host_host_all_f << all_hosts_hosts_unbalanced << ", ";
	host_host_all_f << endl;
	host_host_all_f.close();
};
void log_key_net_pair(
	const Analyser_Key_Net& key, 
	const Analyser_Entry_Net& entry,
	string& buf
)
{
	stringstream tmp;
	// addr
	key.to_s(buf);
	tmp << buf << ", ";
	tmp << entry.balanced << ", ";
	tmp << entry.unbalanced << ", ";
#ifdef ANALYSER_BGP_AGGREGATION
	tmp << entry.bgp_length<< ", ";
#endif
	buf = tmp.str();
	return;
}
void Analyser::analyse_host(uint64_t time_s)
{
	Analyser_Net_h::iterator iter, end;
	Analyser_Key_Net key;
	string buf;

	// interface tables
	// aggregate the host tabels to net/24 and bgp tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{

		if(agg_host_h[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_hosts_not_routable = 0;
		uint64_t interf_hosts_balanced = 0;
		uint64_t interf_hosts_unbalanced = 0;
		uint64_t interf_hosts_unbalanced_but_reachable = 0;

		// output
		Analyser_Net_h& current_net_table_h(*(agg_net_h[i]));
		Analyser_Net_h& current_bgp_table_h(*(agg_bgp_h[i]));

		// log host prefixes
		stringstream host_unbalanced_path_ss;
		host_unbalanced_path_ss << prefixes_host_folder_p.string();
		host_unbalanced_path_ss << "/r_" << router << "_i_" << interface;
		path tmp = host_unbalanced_path_ss.str().c_str();
		create_directory(tmp);
		host_unbalanced_path_ss << "/" << time_s /(24*3600) << "";
		tmp = host_unbalanced_path_ss.str().c_str();
		create_directory(tmp);

		host_unbalanced_path_ss << "/"<< time_s <<".csv";
		ofstream host_unbalanced_f(
			(host_unbalanced_path_ss.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_host_h[i]->begin();
		end = agg_host_h[i]->end();
		while(iter != end)
		{

			// not routable
			if(iter->second.bgp_length == ANALYSER_BGP_AGGREGATION_NOT_ANNOUNCED)
			{
				interf_hosts_not_routable++;
				log_key_net_pair(iter->first,iter->second,buf);
				host_unbalanced_f << "-1, " << buf << endl;
				iter++;
				continue;
			}

			// net
			key.from(iter->first, 24);
			Analyser_Entry_Net& net_all = agg_net_all_h[key];
			Analyser_Entry_Net& net_intef = current_net_table_h[key];

			// bgp
			key.from(iter->first, (iter->second).bgp_length);
			Analyser_Entry_Net& bgp_all = agg_bgp_all_h[key];
			Analyser_Entry_Net& bgp_intef = current_bgp_table_h[key];

			if((iter->second).balanced > 0)
			{
				interf_hosts_balanced++;

				// net
				net_all.balanced++;
				net_intef.balanced++;

				// bgp
				bgp_all.balanced++;
				bgp_intef.balanced++;

			}
			else
			{ // unbalanced
				log_key_net_pair(iter->first,iter->second,buf);
				if(agg_host_all_h[(iter->first)].balanced > 0)
				{
					interf_hosts_unbalanced_but_reachable++;
					host_unbalanced_f << "1, " << buf << endl;
				}
				else
				{
					interf_hosts_unbalanced++;
					host_unbalanced_f << "0, " << buf << endl;
				}

				// net
				net_all.unbalanced++;
				net_intef.unbalanced++;

				// bgp
				bgp_all.unbalanced++;
				bgp_intef.unbalanced++;

			}
			iter++;
		}
		host_unbalanced_f.close();

		// log stat
		stringstream host_path_ss;
		host_path_ss << stat_host_folder_p.string();
		host_path_ss << "/r_" << router << "_i_" << interface;
		host_path_ss << ".csv";
		ofstream host_f(
			(host_path_ss.str()).c_str(),
			ios_base::app
		);
		host_f << time_s << ", ";
		host_f << interf_hosts_not_routable << ", ";
		host_f << interf_hosts_balanced << ", ";
		host_f << interf_hosts_unbalanced << ", ";
		host_f << interf_hosts_unbalanced_but_reachable << ", ";
		host_f << endl;
		host_f.close();

	}; //interfaces


	// All hosts
	uint64_t all_hosts_not_routable = 0;
	uint64_t all_hosts_balanced = 0;
	uint64_t all_hosts_unbalanced = 0;

	// log unbalanced hosts
	stringstream all_host_unbalanced_path_ss;
	all_host_unbalanced_path_ss << prefixes_host_folder_p.string();
	all_host_unbalanced_path_ss << "/all";
	path tmp = all_host_unbalanced_path_ss.str().c_str();
	create_directory(tmp);
	all_host_unbalanced_path_ss << "/" << time_s /(24*3600) << "";
	tmp = all_host_unbalanced_path_ss.str().c_str();
	create_directory(tmp);
	all_host_unbalanced_path_ss << "/"<< time_s <<".csv";
	ofstream all_host_unbalanced_f(
		(all_host_unbalanced_path_ss.str()).c_str(),
		ios_base::trunc
	);

	iter = agg_host_all_h.begin();
	end = agg_host_all_h.end();
	while(iter != end)
	{
		// not routable
		if(iter->second.bgp_length == ANALYSER_BGP_AGGREGATION_NOT_ANNOUNCED)
		{
			all_hosts_not_routable++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_host_unbalanced_f << "-1, " << buf << endl;
		}
		else if((iter->second).balanced > 0)
		{
			all_hosts_balanced++;
		}
		else
		{
			all_hosts_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_host_unbalanced_f << "0, " << buf << endl;
		}
		iter++;
	}
	all_host_unbalanced_f.close();

	// log stat
	stringstream host_all_path_ss;
	host_all_path_ss << stat_host_folder_p.string();
	host_all_path_ss << "/all.csv";
	ofstream host_all_f(
		(host_all_path_ss.str()).c_str(),
		ios_base::app
	);
	host_all_f << time_s << ", ";
	host_all_f << all_hosts_not_routable << ", ";
	host_all_f << all_hosts_balanced << ", ";
	host_all_f << all_hosts_unbalanced << ", ";
	host_all_f << 0 << ", ";
	host_all_f << endl;
	host_all_f.close();


};
void Analyser::analyse_net(uint64_t time_s)
{
	Analyser_Net_h::iterator iter, end;
	Analyser_Key_Net key;
	string buf;

	// interface tables
	// analyse the net/24 tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_net_h[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_net_balanced = 0;
		uint64_t interf_net_unbalanced = 0;
		uint64_t interf_net_unbalanced_but_reachable = 0;

		// log net prefixes
		stringstream net_unbalanced_path_ss;
		net_unbalanced_path_ss << prefixes_net_folder_p.string();
		net_unbalanced_path_ss << "/r_" << router << "_i_" << interface;
		path tmp = net_unbalanced_path_ss.str().c_str();
		create_directory(tmp);
		net_unbalanced_path_ss << "/" << time_s /(24*3600) << "";
		tmp = net_unbalanced_path_ss.str().c_str();
		create_directory(tmp);
		net_unbalanced_path_ss << "/"<< time_s <<".csv";
		ofstream net_unbalanced_f(
			(net_unbalanced_path_ss.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_net_h[i]->begin();
		end = agg_net_h[i]->end();
		while(iter != end)
		{
			if((iter->second).balanced > 0)
			{ // balanced
				interf_net_balanced++;
			}
			else
			{ // unbalanced
				log_key_net_pair(iter->first,iter->second,buf);
				if(agg_net_all_h[(iter->first)].balanced > 0)
				{
					interf_net_unbalanced_but_reachable++;
					net_unbalanced_f << "1, " << buf << endl;
				}
				else
				{
					interf_net_unbalanced++;
					net_unbalanced_f << "0, " << buf << endl;
				}
			}
			iter++;
		}
		net_unbalanced_f.close();

		// log stat
		stringstream net_path_ss;
		net_path_ss << stat_net_folder_p.string();
		net_path_ss << "/r_" << router << "_i_" << interface;
		net_path_ss << ".csv";
		ofstream net_f(
			(net_path_ss.str()).c_str(),
			ios_base::app
		);
		net_f << time_s << ", ";
		net_f << interf_net_balanced << ", ";
		net_f << interf_net_unbalanced << ", ";
		net_f << interf_net_unbalanced_but_reachable << ", ";
		net_f << endl;
		net_f.close();

	}; //interfaces

	// All hosts
	uint64_t all_net_balanced = 0;
	uint64_t all_net_unbalanced = 0;


	// log unbalanced net
	stringstream all_net_unbalanced_path_ss;
	all_net_unbalanced_path_ss << prefixes_net_folder_p.string();
	all_net_unbalanced_path_ss << "/all";
	path tmp = all_net_unbalanced_path_ss.str().c_str();
	create_directory(tmp);
	all_net_unbalanced_path_ss << "/" << time_s /(24*3600) << "";
	tmp = all_net_unbalanced_path_ss.str().c_str();
	create_directory(tmp);
	all_net_unbalanced_path_ss << "/"<< time_s <<".csv";
	ofstream all_net_unbalanced_f(
		(all_net_unbalanced_path_ss.str()).c_str(),
		ios_base::trunc
	);


	iter = agg_net_all_h.begin();
	end = agg_net_all_h.end();
	while(iter != end)
	{
		if((iter->second).balanced > 0)
			all_net_balanced++;
		else
		{
			all_net_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_net_unbalanced_f << "0, " << buf << endl;
		}
		iter++;
	}
	all_net_unbalanced_f.close();

	// log stat
	stringstream net_all_path_ss;
	net_all_path_ss << stat_net_folder_p.string();
	net_all_path_ss << "/all.csv";
	ofstream net_all_f(
		(net_all_path_ss.str()).c_str(),
		ios_base::app
	);
	net_all_f << time_s << ", ";
	net_all_f << all_net_balanced << ", ";
	net_all_f << all_net_unbalanced << ", ";
	net_all_f << 0 << ", ";
	net_all_f << endl;
	net_all_f.close();

};
void Analyser::analyse_bgp(uint64_t time_s)
{
	string buf;
	Analyser_Net_h::iterator iter, end;
	Analyser_Key_Net key;

	// interface tables
	// analyse the net/24 tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_bgp_h[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_bgp_balanced = 0;
		uint64_t interf_bgp_unbalanced = 0;
		uint64_t interf_bgp_unbalanced_but_reachable = 0;

		// log net prefixes
		stringstream bgp_unbalanced_path_ss;
		bgp_unbalanced_path_ss << prefixes_bgp_folder_p.string();
		bgp_unbalanced_path_ss << "/r_" << router << "_i_" << interface;
		path tmp = bgp_unbalanced_path_ss.str().c_str();
		create_directory(tmp);
		bgp_unbalanced_path_ss << "/" << time_s /(24*3600) << "";
		tmp = bgp_unbalanced_path_ss.str().c_str();
		create_directory(tmp);
		bgp_unbalanced_path_ss << "/"<< time_s <<".csv";
		ofstream bgp_unbalanced_f(
			(bgp_unbalanced_path_ss.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_bgp_h[i]->begin();
		end = agg_bgp_h[i]->end();
		while(iter != end)
		{
			if((iter->second).balanced > 0)
			{ // balanced
				interf_bgp_balanced++;
			}
			else
			{ // unbalanced
				log_key_net_pair(iter->first,iter->second,buf);
				if(agg_bgp_all_h[(iter->first)].balanced > 0)
				{
					interf_bgp_unbalanced_but_reachable++;
					bgp_unbalanced_f << "1, " << buf << endl;
				}
				else
				{
					interf_bgp_unbalanced++;
					bgp_unbalanced_f << "0, " << buf << endl;
				}
			}
			iter++;
		}
		bgp_unbalanced_f.close();

		// log data
		stringstream bgp_path_ss;
		bgp_path_ss << stat_bgp_folder_p.string();
		bgp_path_ss << "/r_" << router << "_i_" << interface;
		bgp_path_ss << ".csv";
		ofstream bgp_f(
			(bgp_path_ss.str()).c_str(),
			ios_base::app
		);
		bgp_f << time_s << ", ";
		bgp_f << interf_bgp_balanced << ", ";
		bgp_f << interf_bgp_unbalanced << ", ";
		bgp_f << interf_bgp_unbalanced_but_reachable << ", ";
		bgp_f << endl;
		bgp_f.close();

		agg_bgp_h[i]->clear(); // free memory as soon as possible ...
	}; //interfaces

	// All hosts
	uint64_t all_bgp_balanced = 0;
	uint64_t all_bgp_unbalanced = 0;

	// log unbalanced net
	stringstream all_bgp_unbalanced_path_ss;
	all_bgp_unbalanced_path_ss << prefixes_bgp_folder_p.string();
	all_bgp_unbalanced_path_ss << "/all";
	path tmp = all_bgp_unbalanced_path_ss.str().c_str();
	create_directory(tmp);
	all_bgp_unbalanced_path_ss << "/" << time_s /(24*3600) << "";
	tmp = all_bgp_unbalanced_path_ss.str().c_str();
	create_directory(tmp);
	all_bgp_unbalanced_path_ss << "/"<< time_s <<".csv";
	ofstream all_bgp_unbalanced_f(
		(all_bgp_unbalanced_path_ss.str()).c_str(),
		ios_base::trunc
	);

	iter = agg_bgp_all_h.begin();
	end = agg_bgp_all_h.end();
	while(iter != end)
	{
		if((iter->second).balanced > 0)
			all_bgp_balanced++;
		else
		{
			all_bgp_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_bgp_unbalanced_f << "0, " << buf << endl;
		}
		iter++;
	}
	all_bgp_unbalanced_f.close();
	// log data
	stringstream bgp_all_path_ss;
	bgp_all_path_ss << stat_bgp_folder_p.string();
	bgp_all_path_ss << "/all.csv";
	ofstream bgp_all_f(
		(bgp_all_path_ss.str()).c_str(),
		ios_base::app
	);
	bgp_all_f << time_s << ", ";
	bgp_all_f << all_bgp_balanced << ", ";
	bgp_all_f << all_bgp_unbalanced << ", ";
	bgp_all_f << 0 << ", ";
	bgp_all_f << endl;
	bgp_all_f.close();

	agg_bgp_all_h.clear();
};

//******************************************************************************
// RUBY
//******************************************************************************
void rb_analyser_free(void *p)
{
	delete (Analyser*) p;
	p = NULL;
};
VALUE rb_analyser_alloc(VALUE klass)
{
	Analyser* p = NULL;
	VALUE obj;
	p = new Analyser();
	if (p == NULL)
		rb_raise(rb_path2class("FilterInOutError"), "Out of Memory");

	obj = Data_Wrap_Struct(klass, 0, rb_analyser_free, p);
	return(obj);
};
VALUE rb_analyser_reset(VALUE self)
{
	RB_ANALYSER_UNWRAP
	analyser->reset();
	return(self);
};
//------------------------------------------------------------------------------
// configuration
//------------------------------------------------------------------------------
VALUE rb_analyser_set_working_folder(VALUE self, VALUE path)
{
	RB_ANALYSER_UNWRAP
	string working_path_s = (RSTRING_PTR(path));
	analyser->set_working_path(working_path_s);
	return(self);	
}
VALUE rb_analyser_add_interface(VALUE self, VALUE router, VALUE interface)
{
	RB_ANALYSER_UNWRAP
	analyser->add_interface(NUM2INT(router), NUM2INT(interface));
	return(self);
}
VALUE rb_analyser_add_prefix(VALUE self, VALUE prefix, VALUE length)
{
	RB_ANALYSER_UNWRAP
	string prefix_s = RSTRING_PTR(prefix);
	analyser->add_prefix(prefix_s , NUM2INT(length));
	return(self);
};
//------------------------------------------------------------------------------
// analyser
//------------------------------------------------------------------------------
VALUE rb_analyser_analyse(VALUE self, VALUE con_matrix, VALUE time_s)
{
	RB_ANALYSER_UNWRAP
	Connection_Matrix* con_matrix_;
	Data_Get_Struct(con_matrix, Connection_Matrix, con_matrix_);
	analyser->analyse(*con_matrix_, NUM2ULL(time_s));
	return(self);
};
//------------------------------------------------------------------------------
// statistics
//------------------------------------------------------------------------------
VALUE rb_analyser_stat_get(VALUE self)
{
	RB_ANALYSER_UNWRAP
	VALUE array = rb_ary_new();

	// connections
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_processed));

	// interfaces
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_invalid));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_zero));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_not_monitored));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_monitored));

	// state
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_processed));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_balanced));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_unbalanced));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_other));

	// weird connections
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_processed));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_no_rule));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_zero_packet));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_bpp));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_ok));

	// signal traffic
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_processed));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_ok));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_other));

	// accounted
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_accounted_unbalanced));

	return(array);
};
