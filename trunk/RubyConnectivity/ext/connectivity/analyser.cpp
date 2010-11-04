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
	// SETTING THE PATHS FOR IPv4
	working_p4 = "~/RubyConnectivityDefaultOut/IPv4";
	weird_connections_folder_p4 = working_p4 / "weird_connections";
	stat_host_host_folder_p4 = working_p4 / "host_host";
	stat_host_folder_p4 = working_p4 / "host";
	stat_net_folder_p4 = working_p4 / "net";
	stat_bgp_folder_p4 = working_p4 / "bgp";
	prefixes_host_folder_p4 = working_p4 / "prefix_host";
	prefixes_net_folder_p4 = working_p4 / "prefix_net";
	prefixes_bgp_folder_p4 = working_p4 / "prefix_bgp";
	
	// SETTING THE PATHS FOR IPv6
	working_p6 = "~/RubyConnectivityDefaultOut/IPv6";
	weird_connections_folder_p6 = working_p6 / "weird_connections";
	stat_host_host_folder_p6 = working_p6 / "host_host";
	stat_host_folder_p6 = working_p6 / "host";
	stat_net_folder_p6 = working_p6 / "net";
	stat_bgp_folder_p6 = working_p6 / "bgp";
	prefixes_host_folder_p6 = working_p6 / "prefix_host";
	prefixes_net_folder_p6 = working_p6 / "prefix_net";
	prefixes_bgp_folder_p6 = working_p6 / "prefix_bgp";

	weird_connection_bpp_min = 100;
	bgp_prefixes.set_value_not_found(-1);
	for(int i=0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
			agg_host_host_h4[i] = NULL;
			agg_host_h4[i] = NULL;
			agg_net_h4[i] = NULL;
			agg_bgp_h4[i] = NULL;
			
			agg_host_host_h6[i] = NULL;
			agg_host_h6[i] = NULL;
			agg_net_h6[i] = NULL;
			agg_bgp_h6[i] = NULL;
	};
};
Analyser::~Analyser()
{
	reset();
	for(int i=0; i< ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_host_host_h4[i] != NULL)
		{
			delete agg_host_host_h4[i];
			delete agg_host_h4[i];
			delete agg_net_h4[i];
			delete agg_bgp_h4[i];

			agg_host_host_h4[i] = NULL;
			agg_host_h4[i] = NULL;
			agg_net_h4[i] = NULL;
			agg_bgp_h4[i] = NULL;
		}
		if(agg_host_host_h6[i] != NULL)
		{
			delete agg_host_host_h6[i];
			delete agg_host_h6[i];
			delete agg_net_h6[i];
			delete agg_bgp_h6[i];

			agg_host_host_h6[i] = NULL;
			agg_host_h6[i] = NULL;
			agg_net_h6[i] = NULL;
			agg_bgp_h6[i] = NULL;
		}
	};
};
void Analyser::reset()
{
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_host_host_h4[i] != NULL)
		{
			agg_host_host_h4[i]->clear();
			agg_host_h4[i]->clear();
			agg_net_h4[i]->clear();
			agg_bgp_h4[i]->clear();
		}
		if(agg_host_host_h6[i] != NULL)
		{
			agg_host_host_h6[i]->clear();
			agg_host_h6[i]->clear();
			agg_net_h6[i]->clear();
			agg_bgp_h6[i]->clear();
		}
	};
	agg_host_all_h4.clear();
	agg_net_all_h4.clear();
	agg_bgp_all_h4.clear();

	agg_host_all_h6.clear();
	agg_net_all_h6.clear();
	agg_bgp_all_h6.clear();
	// IPv4 statistics

	// connections
	stat_cons_processed4 = 0;

	// interfaces
	stat_cons_if_invalid4 = 0;
	stat_cons_if_zero4 = 0;
	stat_cons_if_not_monitored4 = 0;
	stat_cons_if_monitored4 = 0;

	// state
	stat_cons_state_processed4 = 0;
	stat_cons_state_balanced4 = 0;
	stat_cons_state_unbalanced4 = 0;
	stat_cons_state_other4 = 0;

	// weird connections
  	stat_cons_weird_cons_processed4 = 0;
	stat_cons_weird_cons_no_rule4 = 0;
	stat_cons_weird_cons_zero_packet4 = 0;
	stat_cons_weird_cons_bpp4 = 0;
	stat_cons_weird_cons_ok4 = 0;

	// signal traffic
	stat_cons_signal_processed4 = 0;
	stat_cons_signal_ok4 = 0;
	stat_cons_signal_other4 = 0;

	// accounted
	stat_cons_accounted_unbalanced4 = 0;
	
	// IPv6 statistics

	// connections
	stat_cons_processed6 = 0;

	// interfaces
	stat_cons_if_invalid6 = 0;
	stat_cons_if_zero6 = 0;
	stat_cons_if_not_monitored6 = 0;
	stat_cons_if_monitored6 = 0;

	// state
	stat_cons_state_processed6 = 0;
	stat_cons_state_balanced6 = 0;
	stat_cons_state_unbalanced6 = 0;
	stat_cons_state_other6 = 0;

	// weird connections
  	stat_cons_weird_cons_processed6 = 0;
	stat_cons_weird_cons_no_rule6 = 0;
	stat_cons_weird_cons_zero_packet6 = 0;
	stat_cons_weird_cons_bpp6 = 0;
	stat_cons_weird_cons_ok6 = 0;

	// signal traffic
	stat_cons_signal_processed6 = 0;
	stat_cons_signal_ok6 = 0;
	stat_cons_signal_other6 = 0;

	// accounted
	stat_cons_accounted_unbalanced6 = 0;

	return;
};
// -----------------------------------------------------------------------------
// CONFIG
// -----------------------------------------------------------------------------
void Analyser::set_working_path(string path_s)
{
	working_p4 = path_s.c_str();
	working_p4 = working_p4 / "IPv4";
	create_directory(working_p4);
	
	weird_connections_folder_p4 = working_p4 / "weird_connections";
	create_directory(weird_connections_folder_p4);
	stat_host_host_folder_p4 = working_p4 / "host_host";
	create_directory(stat_host_host_folder_p4);
	stat_host_folder_p4 = working_p4 / "host";
	create_directory(stat_host_folder_p4);
	stat_net_folder_p4 = working_p4 / "net";
	create_directory(stat_net_folder_p4);
	stat_bgp_folder_p4 = working_p4 / "bgp";
	create_directory(stat_bgp_folder_p4);

	prefixes_host_folder_p4 = working_p4 / "prefix_host";
	create_directory(prefixes_host_folder_p4);
	prefixes_net_folder_p4 = working_p4 / "prefix_net";
	create_directory(prefixes_net_folder_p4);
	prefixes_bgp_folder_p4 = working_p4 / "prefix_bgp";
	create_directory(prefixes_bgp_folder_p4);
	
	working_p6 = path_s.c_str();
	working_p6 = working_p6 / "IPv6";
	create_directory(working_p6);
	
	weird_connections_folder_p6 = working_p6 / "weird_connections";
	create_directory(weird_connections_folder_p6);
	stat_host_host_folder_p6 = working_p6 / "host_host";
	create_directory(stat_host_host_folder_p6);
	stat_host_folder_p6 = working_p6 / "host";
	create_directory(stat_host_folder_p6);
	stat_net_folder_p6 = working_p6 / "net";
	create_directory(stat_net_folder_p6);
	stat_bgp_folder_p6 = working_p6 / "bgp";
	create_directory(stat_bgp_folder_p6);

	prefixes_host_folder_p6 = working_p6 / "prefix_host";
	create_directory(prefixes_host_folder_p6);
	prefixes_net_folder_p6 = working_p6 / "prefix_net";
	create_directory(prefixes_net_folder_p6);
	prefixes_bgp_folder_p6 = working_p6 / "prefix_bgp";
	create_directory(prefixes_bgp_folder_p6);

};
void Analyser::add_interface(int router, int interface)
{
	if(router >= ANALYSER_MAX_ROUTER or interface >= ANALYSER_MAX_INTERFACE)
	{
		cerr << "Analyser::add_interface: out of band error :: R:" << router << " I:" << interface  << endl;
		cerr.flush();
		throw 200; // FIXME
	}
	agg_host_host_h4[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_Net_h;
	agg_host_h4[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;
	agg_net_h4[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;
	agg_bgp_h4[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;
	
	agg_host_host_h6[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_Net_h;
	agg_host_h6[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;
	agg_net_h6[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;
	agg_bgp_h6[router*ANALYSER_MAX_INTERFACE + interface] = new Analyser_Net_h;

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
	//cout << "PREFIX: "<< p.to_s() << endl;
	//cout.flush();
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
  	Analyser_Key_Net_Net host_host_key4;
  	Analyser_Key_Net_Net host_host_key6;

	Connection_Matrix_HT::const_iterator iter, end, iter6, end6;
	iter = con_mat.connections_ipv4_h.begin();
	end = con_mat.connections_ipv4_h.end();

  // -- extract weird unbalanced signal connections
	stringstream weird_connections_path_ss4;
	stringstream weird_connections_path_ss6;
	
	weird_connections_path_ss4 << weird_connections_folder_p4.string();
	weird_connections_path_ss4 << "/" << time_s /(24*3600) << "/";
	path tmp = weird_connections_path_ss4.str().c_str();
	create_directory(tmp);
	weird_connections_path_ss4 << "/" << time_s << ".csv";
	ofstream weird_connections_f4(
		(weird_connections_path_ss4.str()).c_str(),
		ios_base::trunc
	);
	
	iter6 = con_mat.connections_ipv6_h.begin();
	end6 = con_mat.connections_ipv6_h.end();
	weird_connections_path_ss6 << weird_connections_folder_p6.string();
	weird_connections_path_ss6 << "/" << time_s /(24*3600) << "/";
	tmp = weird_connections_path_ss6.str().c_str();
	create_directory(tmp);
	weird_connections_path_ss6 << "/" << time_s << ".csv";
	ofstream weird_connections_f6(
		(weird_connections_path_ss6.str()).c_str(),
		ios_base::trunc
	);
	uint8_t state;
	Analyser_Net_Net_h* host_host_p4;
	Analyser_Net_Net_h* host_host_p6;
	
	int bpp;

	while(iter != end)
	{
		stat_cons_processed4++;

		//------------------------------------------------------------------------------
		// INTERFACE IPv4-------------------------------------------------------------------
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

			stat_cons_if_invalid4++;
			iter++;
			continue;			
		}
		if ( 
			(iter->second).in_out_router == 0 and
			(iter->second).in_out_if == 0
		)
		{
			stat_cons_if_zero4++;
			iter++;
			continue;
		}
		host_host_p4 = agg_host_host_h4[(iter->second).in_out_router*ANALYSER_MAX_INTERFACE + (iter->second).in_out_if];
		if(host_host_p4 == NULL)
		{
			stat_cons_if_not_monitored4++;
			iter++;
			continue;			
		}
		stat_cons_if_monitored4++;

		//------------------------------------------------------------------------------
		// STATE IPv4-----------------------------------------------------------------------
		//------------------------------------------------------------------------------

		// STATE of this connection?
		stat_cons_state_processed4++;
		state = iter->second.state;
		if( // Balanced
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__EVENT or
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__SPAN or
			state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__EVENT
		)
		{
			host_host_key4.from(iter->first, 32, 32);
			(*host_host_p4)[host_host_key4].balanced++;

			stat_cons_state_balanced4++;
			iter++;
			continue;
		}
		else if ( // Unbalanced
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__NEVER or
			state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__NEVER
		)
		{
			stat_cons_state_unbalanced4++;
		}
		else
		{ // Other
			stat_cons_state_other4++;
			iter++;
			continue;
		}
		// only state unbalanced survives

		//------------------------------------------------------------------------------
		// WEIRD CONNECTIONS -----------------------------------------------------------
		//------------------------------------------------------------------------------
		// Packet Loss, Measurement Infrastructure Loss
		// TCP
		stat_cons_weird_cons_processed4++;
		if( (iter->first).protocol() != 6 )
		{ // non TCP so no rules (at the moment)
			stat_cons_weird_cons_no_rule4++;
			iter++;
			continue;
		}

		// ZERO PACKET
		if((iter->second).in_out_packets == 0) 
		{
			weird_connection_log(0, iter->first, iter->second, weird_connections_f4);
			stat_cons_weird_cons_zero_packet4++;
			iter++;
			continue;
		}

		// TOO BIG Byte Per Packet RATE
		bpp = (iter->second).in_out_bytes/(iter->second).in_out_packets;
		if(bpp > weird_connection_bpp_min)
		{
			weird_connection_log(1, iter->first, iter->second, weird_connections_f4);
			stat_cons_weird_cons_bpp4++;
			iter++;
			continue;
		}
		stat_cons_weird_cons_ok4++;

		//------------------------------------------------------------------------------
		// SINGAL TRAFFIC IPv4--------------------------------------------------------------
		//------------------------------------------------------------------------------
		stat_cons_signal_processed4++;
		if( 
			(iter->first).protocol() != 6 or
			(iter->first).port_out() != 80 or 
			(iter->first).port_in()  < 1024
		)
		{ // non signal traffic
			stat_cons_signal_other4++;
			iter++;
			continue;
		}
		stat_cons_signal_ok4++;
		//------------------------------------------------------------------------------
		// ACCOUNT THE REST IPv4--------------------------------------------------------------
		//------------------------------------------------------------------------------

		// ACCOUNT the unbalanced connection
		host_host_key4.from(iter->first, 32, 32);
		(*host_host_p4)[host_host_key4].unbalanced++;
		stat_cons_accounted_unbalanced4++;

		iter++;
		continue;

		// THIS LINE SHOULD NEVER BE REACHED
		throw 1000;
	}
	weird_connections_f4.close();
	
	
	// IPv6 ANALYSER!
	while(iter6 != end6)
	{
		stat_cons_processed4++;

		//------------------------------------------------------------------------------
		// INTERFACE IPv6-------------------------------------------------------------------
		//------------------------------------------------------------------------------
		// Valid, Monitored ?
		if(
			(iter6->second).in_out_router >= ANALYSER_MAX_ROUTER or 
			(iter6->second).in_out_router < 0 or 
			(iter6->second).in_out_if >=  ANALYSER_MAX_INTERFACE or
			(iter6->second).in_out_if < 0 
		)
		{
			cerr << "Analyser::add_interface: out of band error :: ";
			cerr << "R:" << (iter6->second).in_out_router << " ";
			cerr << "I:" << (iter6->second).in_out_if << endl;
			cerr.flush();
			throw 200; // FIXME

			stat_cons_if_invalid6++;
			iter6++;
			continue;			
		}
		if ( 
			(iter6->second).in_out_router == 0 and
			(iter6->second).in_out_if == 0
		)
		{
			stat_cons_if_zero6++;
			iter6++;
			continue;
		}
		host_host_p6 = agg_host_host_h6[(iter6->second).in_out_router*ANALYSER_MAX_INTERFACE + (iter6->second).in_out_if];
		if(host_host_p6 == NULL)
		{
			stat_cons_if_not_monitored6++;
			iter6++;
			continue;			
		}
		stat_cons_if_monitored6++;

		//------------------------------------------------------------------------------
		// STATE IPv6-----------------------------------------------------------------------
		//------------------------------------------------------------------------------

		// STATE of this connection?
		stat_cons_state_processed6++;
		state = iter6->second.state;
		if( // Balanced
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__EVENT or
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__SPAN or
			state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__EVENT
		)
		{
			host_host_key6.from(iter6->first, 128, 128);
			(*host_host_p6)[host_host_key6].balanced++;

			stat_cons_state_balanced6++;
			iter6++;
			continue;
		}
		else if ( // Unbalanced
			state == CONNECTION_MATRIX_ENTRY_STATE_EVENT__NEVER or
			state == CONNECTION_MATRIX_ENTRY_STATE_SPAN__NEVER
		)
		{
			stat_cons_state_unbalanced6++;
		}
		else
		{ // Other
			stat_cons_state_other6++;
			iter6++;
			continue;
		}
		// only state unbalanced survives

		//------------------------------------------------------------------------------
		// WEIRD CONNECTIONS IPv6-----------------------------------------------------------
		//------------------------------------------------------------------------------
		// Packet Loss, Measurement Infrastructure Loss
		// TCP
		stat_cons_weird_cons_processed6++;
		if( (iter6->first).protocol() != 6 )
		{ // non TCP so no rules (at the moment)
			stat_cons_weird_cons_no_rule6++;
			iter6++;
			continue;
		}

		// ZERO PACKET
		if((iter6->second).in_out_packets == 0) 
		{
			weird_connection_log(0, iter6->first, iter6->second, weird_connections_f6);
			stat_cons_weird_cons_zero_packet6++;
			iter6++;
			continue;
		}

		// TOO BIG Byte Per Packet RATE
		bpp = (iter6->second).in_out_bytes/(iter6->second).in_out_packets;
		if(bpp > weird_connection_bpp_min)
		{
			weird_connection_log(1, iter6->first, iter6->second, weird_connections_f6);
			stat_cons_weird_cons_bpp6++;
			iter6++;
			continue;
		}
		stat_cons_weird_cons_ok6++;

		//------------------------------------------------------------------------------
		// SINGAL TRAFFIC IPv6--------------------------------------------------------------
		//------------------------------------------------------------------------------
		stat_cons_signal_processed6++;
		if( 
			(iter6->first).protocol() != 6 or
			(iter6->first).port_out() != 80 or 
			(iter6->first).port_in()  < 1024
		)
		{ // non signal traffic
			stat_cons_signal_other6++;
			iter6++;
			continue;
		}
		stat_cons_signal_ok6++;
		//------------------------------------------------------------------------------
		// ACCOUNT THE REST IPv6--------------------------------------------------------------
		//------------------------------------------------------------------------------

		// ACCOUNT the unbalanced connection
		host_host_key6.from(iter6->first, 128, 128);
		(*host_host_p6)[host_host_key6].unbalanced++;
		stat_cons_accounted_unbalanced6++;

		iter6++;
		continue;

		// THIS LINE SHOULD NEVER BE REACHED
		throw 1000;
	}
	weird_connections_f6.close();
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
	
	
	//////////////////////////////////////
	// IPv4
	//////////////////////////////////////
	// aggregate the host-host tabels to host tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{

		if(agg_host_host_h4[i] == NULL)
			continue;
		
		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interface_hosts_hosts_balanced = 0;
		uint64_t interface_hosts_hosts_unbalanced = 0;

		// output
		Analyser_Net_h& current_host_table_h4(*(agg_host_h4[i]));
		iter = agg_host_host_h4[i]->begin();
		end = agg_host_host_h4[i]->end();
		while(iter != end)
		{
			key.from(iter->first, 32);
			
			Analyser_Entry_Net& all = agg_host_all_h4[key];
			Analyser_Entry_Net& intef = current_host_table_h4[key];

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
		stringstream host_host_path_ss4;
		host_host_path_ss4 << stat_host_host_folder_p4.string();
		host_host_path_ss4 << "/r_" << router << "_i_" << interface;
		host_host_path_ss4 << ".csv";
		ofstream host_host_f4(
			(host_host_path_ss4.str()).c_str(),
			ios_base::app
		);
		host_host_f4 << time_s << ", ";
		host_host_f4 << interface_hosts_hosts_balanced << ", ";
		host_host_f4 << interface_hosts_hosts_unbalanced << ", ";
		host_host_f4 << endl;
		host_host_f4.close();

		agg_host_host_h4[i]->clear(); // free memory as soon as possible ...
	}; //interfaces

	// log statistics
	stringstream host_host_all_path_ss4;
	host_host_all_path_ss4 << stat_host_host_folder_p4.string();
	host_host_all_path_ss4 << "/all.csv";
	ofstream host_host_all_f4(
		(host_host_all_path_ss4.str()).c_str(),
		ios_base::app
	);
	host_host_all_f4 << time_s << ", ";
	host_host_all_f4 << all_hosts_hosts_balanced << ", ";
	host_host_all_f4 << all_hosts_hosts_unbalanced << ", ";
	host_host_all_f4 << endl;
	host_host_all_f4.close();
	
	//////////////////////////////////////
	// IPv6
	//////////////////////////////////////
	// aggregate the host-host tabels to host tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{

		if(agg_host_host_h6[i] == NULL)
			continue;
		
		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interface_hosts_hosts_balanced = 0;
		uint64_t interface_hosts_hosts_unbalanced = 0;

		// output
		Analyser_Net_h& current_host_table_h6(*(agg_host_h6[i]));
		iter = agg_host_host_h6[i]->begin();
		end = agg_host_host_h6[i]->end();
		while(iter != end)
		{
			key.from(iter->first, 128);
			
			Analyser_Entry_Net& all = agg_host_all_h6[key];
			Analyser_Entry_Net& intef = current_host_table_h6[key];

			if(all.bgp_length == -1)
			{
				// resolve it
				p.from_nb(key.addr(), 128, Prefix::FAMILY_IPV6);
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
		stringstream host_host_path_ss6;
		host_host_path_ss6 << stat_host_host_folder_p6.string();
		host_host_path_ss6 << "/r_" << router << "_i_" << interface;
		host_host_path_ss6 << ".csv";
		ofstream host_host_f6(
			(host_host_path_ss6.str()).c_str(),
			ios_base::app
		);
		host_host_f6 << time_s << ", ";
		host_host_f6 << interface_hosts_hosts_balanced << ", ";
		host_host_f6 << interface_hosts_hosts_unbalanced << ", ";
		host_host_f6 << endl;
		host_host_f6.close();

		agg_host_host_h6[i]->clear(); // free memory as soon as possible ...
	}; //interfaces

	// log statistics
	stringstream host_host_all_path_ss6;
	host_host_all_path_ss6 << stat_host_host_folder_p6.string();
	host_host_all_path_ss6 << "/all.csv";
	ofstream host_host_all_f6(
		(host_host_all_path_ss6.str()).c_str(),
		ios_base::app
	);
	host_host_all_f6 << time_s << ", ";
	host_host_all_f6 << all_hosts_hosts_balanced << ", ";
	host_host_all_f6 << all_hosts_hosts_unbalanced << ", ";
	host_host_all_f6 << endl;
	host_host_all_f6.close();
	
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


	//////////////////////////////////////
	// IPv4
	//////////////////////////////////////
	// interface tables
	// aggregate the host tabels to net/24 and bgp tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{

		if(agg_host_h4[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_hosts_not_routable = 0;
		uint64_t interf_hosts_balanced = 0;
		uint64_t interf_hosts_unbalanced = 0;
		uint64_t interf_hosts_unbalanced_but_reachable = 0;

		// output
		Analyser_Net_h& current_net_table_h4(*(agg_net_h4[i]));
		Analyser_Net_h& current_bgp_table_h4(*(agg_bgp_h4[i]));

		// log host prefixes
		stringstream host_unbalanced_path_ss4;
		host_unbalanced_path_ss4 << prefixes_host_folder_p4.string();
		host_unbalanced_path_ss4 << "/r_" << router << "_i_" << interface;
		path tmp = host_unbalanced_path_ss4.str().c_str();
		create_directory(tmp);
		host_unbalanced_path_ss4 << "/" << time_s /(24*3600) << "";
		tmp = host_unbalanced_path_ss4.str().c_str();
		create_directory(tmp);

		host_unbalanced_path_ss4 << "/"<< time_s <<".csv";
		ofstream host_unbalanced_f4(
			(host_unbalanced_path_ss4.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_host_h4[i]->begin();
		end = agg_host_h4[i]->end();
		while(iter != end)
		{

			// not routable
			if(iter->second.bgp_length == ANALYSER_BGP_AGGREGATION_NOT_ANNOUNCED)
			{
				interf_hosts_not_routable++;
				log_key_net_pair(iter->first,iter->second,buf);
				host_unbalanced_f4 << "-1, " << buf << endl;
				iter++;
				continue;
			}

			// net
			key.from(iter->first, 24);
			Analyser_Entry_Net& net_all = agg_net_all_h4[key];
			Analyser_Entry_Net& net_intef = current_net_table_h4[key];

			// bgp
			key.from(iter->first, (iter->second).bgp_length);
			Analyser_Entry_Net& bgp_all = agg_bgp_all_h4[key];
			Analyser_Entry_Net& bgp_intef = current_bgp_table_h4[key];

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
				if(agg_host_all_h4[(iter->first)].balanced > 0)
				{
					interf_hosts_unbalanced_but_reachable++;
					host_unbalanced_f4 << "1, " << buf << endl;
				}
				else
				{
					interf_hosts_unbalanced++;
					host_unbalanced_f4 << "0, " << buf << endl;
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
		host_unbalanced_f4.close();

		// log stat
		stringstream host_path_ss4;
		host_path_ss4 << stat_host_folder_p4.string();
		host_path_ss4 << "/r_" << router << "_i_" << interface;
		host_path_ss4 << ".csv";
		ofstream host_f4(
			(host_path_ss4.str()).c_str(),
			ios_base::app
		);
		host_f4 << time_s << ", ";
		host_f4 << interf_hosts_not_routable << ", ";
		host_f4 << interf_hosts_balanced << ", ";
		host_f4 << interf_hosts_unbalanced << ", ";
		host_f4 << interf_hosts_unbalanced_but_reachable << ", ";
		host_f4 << endl;
		host_f4.close();

	}; //interfaces


	// All hosts
	uint64_t all_hosts_not_routable = 0;
	uint64_t all_hosts_balanced = 0;
	uint64_t all_hosts_unbalanced = 0;

	// log unbalanced hosts
	stringstream all_host_unbalanced_path_ss4;
	all_host_unbalanced_path_ss4 << prefixes_host_folder_p4.string();
	all_host_unbalanced_path_ss4 << "/all";
	path tmp = all_host_unbalanced_path_ss4.str().c_str();
	create_directory(tmp);
	all_host_unbalanced_path_ss4 << "/" << time_s /(24*3600) << "";
	tmp = all_host_unbalanced_path_ss4.str().c_str();
	create_directory(tmp);
	all_host_unbalanced_path_ss4 << "/"<< time_s <<".csv";
	ofstream all_host_unbalanced_f4(
		(all_host_unbalanced_path_ss4.str()).c_str(),
		ios_base::trunc
	);

	iter = agg_host_all_h4.begin();
	end = agg_host_all_h4.end();
	while(iter != end)
	{
		// not routable
		if(iter->second.bgp_length == ANALYSER_BGP_AGGREGATION_NOT_ANNOUNCED)
		{
			all_hosts_not_routable++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_host_unbalanced_f4 << "-1, " << buf << endl;
		}
		else if((iter->second).balanced > 0)
		{
			all_hosts_balanced++;
		}
		else
		{
			all_hosts_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_host_unbalanced_f4 << "0, " << buf << endl;
		}
		iter++;
	}
	all_host_unbalanced_f4.close();

	// log stat
	stringstream host_all_path_ss4;
	host_all_path_ss4 << stat_host_folder_p4.string();
	host_all_path_ss4 << "/all.csv";
	ofstream host_all_f4(
		(host_all_path_ss4.str()).c_str(),
		ios_base::app
	);
	host_all_f4 << time_s << ", ";
	host_all_f4 << all_hosts_not_routable << ", ";
	host_all_f4 << all_hosts_balanced << ", ";
	host_all_f4 << all_hosts_unbalanced << ", ";
	host_all_f4 << 0 << ", ";
	host_all_f4 << endl;
	host_all_f4.close();


	//////////////////////////////////////
	// IPv6
	//////////////////////////////////////
	// interface tables
	// aggregate the host tabels to net/48 and bgp tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{

		if(agg_host_h6[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_hosts_not_routable = 0;
		uint64_t interf_hosts_balanced = 0;
		uint64_t interf_hosts_unbalanced = 0;
		uint64_t interf_hosts_unbalanced_but_reachable = 0;

		// output
		Analyser_Net_h& current_net_table_h6(*(agg_net_h6[i]));
		Analyser_Net_h& current_bgp_table_h6(*(agg_bgp_h6[i]));

		// log host prefixes
		stringstream host_unbalanced_path_ss6;
		host_unbalanced_path_ss6 << prefixes_host_folder_p6.string();
		host_unbalanced_path_ss6 << "/r_" << router << "_i_" << interface;
		path tmp = host_unbalanced_path_ss6.str().c_str();
		create_directory(tmp);
		host_unbalanced_path_ss6 << "/" << time_s /(24*3600) << "";
		tmp = host_unbalanced_path_ss6.str().c_str();
		create_directory(tmp);

		host_unbalanced_path_ss6 << "/"<< time_s <<".csv";
		ofstream host_unbalanced_f6(
			(host_unbalanced_path_ss6.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_host_h6[i]->begin();
		end = agg_host_h6[i]->end();
		while(iter != end)
		{

			// not routable
			if(iter->second.bgp_length == ANALYSER_BGP_AGGREGATION_NOT_ANNOUNCED)
			{
				interf_hosts_not_routable++;
				log_key_net_pair(iter->first,iter->second,buf);
				host_unbalanced_f6 << "-1, " << buf << endl;
				iter++;
				continue;
			}

			// net
			key.from(iter->first, 48);
			Analyser_Entry_Net& net_all = agg_net_all_h6[key];
			Analyser_Entry_Net& net_intef = current_net_table_h6[key];

			// bgp
			key.from(iter->first, (iter->second).bgp_length);
			Analyser_Entry_Net& bgp_all = agg_bgp_all_h6[key];
			Analyser_Entry_Net& bgp_intef = current_bgp_table_h6[key];

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
				if(agg_host_all_h6[(iter->first)].balanced > 0)
				{
					interf_hosts_unbalanced_but_reachable++;
					host_unbalanced_f6 << "1, " << buf << endl;
				}
				else
				{
					interf_hosts_unbalanced++;
					host_unbalanced_f6 << "0, " << buf << endl;
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
		host_unbalanced_f6.close();

		// log stat
		stringstream host_path_ss6;
		host_path_ss6 << stat_host_folder_p6.string();
		host_path_ss6 << "/r_" << router << "_i_" << interface;
		host_path_ss6 << ".csv";
		ofstream host_f6(
			(host_path_ss6.str()).c_str(),
			ios_base::app
		);
		host_f6 << time_s << ", ";
		host_f6 << interf_hosts_not_routable << ", ";
		host_f6 << interf_hosts_balanced << ", ";
		host_f6 << interf_hosts_unbalanced << ", ";
		host_f6 << interf_hosts_unbalanced_but_reachable << ", ";
		host_f6 << endl;
		host_f6.close();

	}; //interfaces


	// All hosts
	all_hosts_not_routable = 0;
	all_hosts_balanced = 0;
	all_hosts_unbalanced = 0;

	// log unbalanced hosts
	stringstream all_host_unbalanced_path_ss6;
	all_host_unbalanced_path_ss6 << prefixes_host_folder_p6.string();
	all_host_unbalanced_path_ss6 << "/all";
	tmp = all_host_unbalanced_path_ss6.str().c_str();
	create_directory(tmp);
	all_host_unbalanced_path_ss6 << "/" << time_s /(24*3600) << "";
	tmp = all_host_unbalanced_path_ss6.str().c_str();
	create_directory(tmp);
	all_host_unbalanced_path_ss6 << "/"<< time_s <<".csv";
	ofstream all_host_unbalanced_f6(
		(all_host_unbalanced_path_ss6.str()).c_str(),
		ios_base::trunc
	);

	iter = agg_host_all_h6.begin();
	end = agg_host_all_h6.end();
	while(iter != end)
	{
		// not routable
		if(iter->second.bgp_length == ANALYSER_BGP_AGGREGATION_NOT_ANNOUNCED)
		{
			all_hosts_not_routable++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_host_unbalanced_f6 << "-1, " << buf << endl;
		}
		else if((iter->second).balanced > 0)
		{
			all_hosts_balanced++;
		}
		else
		{
			all_hosts_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_host_unbalanced_f6 << "0, " << buf << endl;
		}
		iter++;
	}
	all_host_unbalanced_f6.close();

	// log stat
	stringstream host_all_path_ss6;
	host_all_path_ss6 << stat_host_folder_p6.string();
	host_all_path_ss6 << "/all.csv";
	ofstream host_all_f6(
		(host_all_path_ss6.str()).c_str(),
		ios_base::app
	);
	host_all_f6 << time_s << ", ";
	host_all_f6 << all_hosts_not_routable << ", ";
	host_all_f6 << all_hosts_balanced << ", ";
	host_all_f6 << all_hosts_unbalanced << ", ";
	host_all_f6 << 0 << ", ";
	host_all_f6 << endl;
	host_all_f6.close();

};
void Analyser::analyse_net(uint64_t time_s)
{
	Analyser_Net_h::iterator iter, end;
	Analyser_Key_Net key;
	string buf;


	//////////////////////////////////////
	// IPv4
	//////////////////////////////////////
	// interface tables
	// analyse the net/24 tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_net_h4[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_net_balanced = 0;
		uint64_t interf_net_unbalanced = 0;
		uint64_t interf_net_unbalanced_but_reachable = 0;

		// log net prefixes
		stringstream net_unbalanced_path_ss4;
		net_unbalanced_path_ss4 << prefixes_net_folder_p4.string();
		net_unbalanced_path_ss4 << "/r_" << router << "_i_" << interface;
		path tmp = net_unbalanced_path_ss4.str().c_str();
		create_directory(tmp);
		net_unbalanced_path_ss4 << "/" << time_s /(24*3600) << "";
		tmp = net_unbalanced_path_ss4.str().c_str();
		create_directory(tmp);
		net_unbalanced_path_ss4 << "/"<< time_s <<".csv";
		ofstream net_unbalanced_f4(
			(net_unbalanced_path_ss4.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_net_h4[i]->begin();
		end = agg_net_h4[i]->end();
		while(iter != end)
		{
			if((iter->second).balanced > 0)
			{ // balanced
				interf_net_balanced++;
			}
			else
			{ // unbalanced
				log_key_net_pair(iter->first,iter->second,buf);
				if(agg_net_all_h4[(iter->first)].balanced > 0)
				{
					interf_net_unbalanced_but_reachable++;
					net_unbalanced_f4 << "1, " << buf << endl;
				}
				else
				{
					interf_net_unbalanced++;
					net_unbalanced_f4 << "0, " << buf << endl;
				}
			}
			iter++;
		}
		net_unbalanced_f4.close();

		// log stat
		stringstream net_path_ss4;
		net_path_ss4 << stat_net_folder_p4.string();
		net_path_ss4 << "/r_" << router << "_i_" << interface;
		net_path_ss4 << ".csv";
		ofstream net_f4(
			(net_path_ss4.str()).c_str(),
			ios_base::app
		);
		net_f4 << time_s << ", ";
		net_f4 << interf_net_balanced << ", ";
		net_f4 << interf_net_unbalanced << ", ";
		net_f4 << interf_net_unbalanced_but_reachable << ", ";
		net_f4 << endl;
		net_f4.close();

	}; //interfaces

	// All hosts
	uint64_t all_net_balanced = 0;
	uint64_t all_net_unbalanced = 0;


	// log unbalanced net
	stringstream all_net_unbalanced_path_ss4;
	all_net_unbalanced_path_ss4 << prefixes_net_folder_p4.string();
	all_net_unbalanced_path_ss4 << "/all";
	path tmp = all_net_unbalanced_path_ss4.str().c_str();
	create_directory(tmp);
	all_net_unbalanced_path_ss4 << "/" << time_s /(24*3600) << "";
	tmp = all_net_unbalanced_path_ss4.str().c_str();
	create_directory(tmp);
	all_net_unbalanced_path_ss4 << "/"<< time_s <<".csv";
	ofstream all_net_unbalanced_f4(
		(all_net_unbalanced_path_ss4.str()).c_str(),
		ios_base::trunc
	);


	iter = agg_net_all_h4.begin();
	end = agg_net_all_h4.end();
	while(iter != end)
	{
		if((iter->second).balanced > 0)
			all_net_balanced++;
		else
		{
			all_net_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_net_unbalanced_f4 << "0, " << buf << endl;
		}
		iter++;
	}
	all_net_unbalanced_f4.close();

	// log stat
	stringstream net_all_path_ss4;
	net_all_path_ss4 << stat_net_folder_p4.string();
	net_all_path_ss4 << "/all.csv";
	ofstream net_all_f4(
		(net_all_path_ss4.str()).c_str(),
		ios_base::app
	);
	net_all_f4 << time_s << ", ";
	net_all_f4 << all_net_balanced << ", ";
	net_all_f4 << all_net_unbalanced << ", ";
	net_all_f4 << 0 << ", ";
	net_all_f4 << endl;
	net_all_f4.close();
	
	
	//////////////////////////////////////
	// IPv6
	//////////////////////////////////////
	// interface tables
	// analyse the net/48 tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_net_h6[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_net_balanced = 0;
		uint64_t interf_net_unbalanced = 0;
		uint64_t interf_net_unbalanced_but_reachable = 0;

		// log net prefixes
		stringstream net_unbalanced_path_ss6;
		net_unbalanced_path_ss6 << prefixes_net_folder_p6.string();
		net_unbalanced_path_ss6 << "/r_" << router << "_i_" << interface;
		path tmp = net_unbalanced_path_ss6.str().c_str();
		create_directory(tmp);
		net_unbalanced_path_ss6 << "/" << time_s /(24*3600) << "";
		tmp = net_unbalanced_path_ss6.str().c_str();
		create_directory(tmp);
		net_unbalanced_path_ss6 << "/"<< time_s <<".csv";
		ofstream net_unbalanced_f6(
			(net_unbalanced_path_ss6.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_net_h6[i]->begin();
		end = agg_net_h6[i]->end();
		while(iter != end)
		{
			if((iter->second).balanced > 0)
			{ // balanced
				interf_net_balanced++;
			}
			else
			{ // unbalanced
				log_key_net_pair(iter->first,iter->second,buf);
				if(agg_net_all_h6[(iter->first)].balanced > 0)
				{
					interf_net_unbalanced_but_reachable++;
					net_unbalanced_f6 << "1, " << buf << endl;
				}
				else
				{
					interf_net_unbalanced++;
					net_unbalanced_f6 << "0, " << buf << endl;
				}
			}
			iter++;
		}
		net_unbalanced_f6.close();

		// log stat
		stringstream net_path_ss6;
		net_path_ss6 << stat_net_folder_p6.string();
		net_path_ss6 << "/r_" << router << "_i_" << interface;
		net_path_ss6 << ".csv";
		ofstream net_f6(
			(net_path_ss6.str()).c_str(),
			ios_base::app
		);
		net_f6 << time_s << ", ";
		net_f6 << interf_net_balanced << ", ";
		net_f6 << interf_net_unbalanced << ", ";
		net_f6 << interf_net_unbalanced_but_reachable << ", ";
		net_f6 << endl;
		net_f6.close();

	}; //interfaces

	// All hosts
	all_net_balanced = 0;
	all_net_unbalanced = 0;


	// log unbalanced net
	stringstream all_net_unbalanced_path_ss6;
	all_net_unbalanced_path_ss6 << prefixes_net_folder_p6.string();
	all_net_unbalanced_path_ss6 << "/all";
	tmp = all_net_unbalanced_path_ss6.str().c_str();
	create_directory(tmp);
	all_net_unbalanced_path_ss6 << "/" << time_s /(24*3600) << "";
	tmp = all_net_unbalanced_path_ss6.str().c_str();
	create_directory(tmp);
	all_net_unbalanced_path_ss6 << "/"<< time_s <<".csv";
	ofstream all_net_unbalanced_f6(
		(all_net_unbalanced_path_ss6.str()).c_str(),
		ios_base::trunc
	);


	iter = agg_net_all_h6.begin();
	end = agg_net_all_h6.end();
	while(iter != end)
	{
		if((iter->second).balanced > 0)
			all_net_balanced++;
		else
		{
			all_net_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_net_unbalanced_f6 << "0, " << buf << endl;
		}
		iter++;
	}
	all_net_unbalanced_f6.close();

	// log stat
	stringstream net_all_path_ss6;
	net_all_path_ss6 << stat_net_folder_p6.string();
	net_all_path_ss6 << "/all.csv";
	ofstream net_all_f6(
		(net_all_path_ss6.str()).c_str(),
		ios_base::app
	);
	net_all_f6 << time_s << ", ";
	net_all_f6 << all_net_balanced << ", ";
	net_all_f6 << all_net_unbalanced << ", ";
	net_all_f6 << 0 << ", ";
	net_all_f6 << endl;
	net_all_f6.close();

};
void Analyser::analyse_bgp(uint64_t time_s)
{
	string buf;
	Analyser_Net_h::iterator iter, end;
	Analyser_Key_Net key;

	//////////////////////////////////////
	// IPv4
	//////////////////////////////////////
	// interface tables
	// analyse the net/24 tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_bgp_h4[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_bgp_balanced = 0;
		uint64_t interf_bgp_unbalanced = 0;
		uint64_t interf_bgp_unbalanced_but_reachable = 0;

		// log net prefixes
		stringstream bgp_unbalanced_path_ss4;
		bgp_unbalanced_path_ss4 << prefixes_bgp_folder_p4.string();
		bgp_unbalanced_path_ss4 << "/r_" << router << "_i_" << interface;
		path tmp = bgp_unbalanced_path_ss4.str().c_str();
		create_directory(tmp);
		bgp_unbalanced_path_ss4 << "/" << time_s /(24*3600) << "";
		tmp = bgp_unbalanced_path_ss4.str().c_str();
		create_directory(tmp);
		bgp_unbalanced_path_ss4 << "/"<< time_s <<".csv";
		ofstream bgp_unbalanced_f4(
			(bgp_unbalanced_path_ss4.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_bgp_h4[i]->begin();
		end = agg_bgp_h4[i]->end();
		while(iter != end)
		{
			if((iter->second).balanced > 0)
			{ // balanced
				interf_bgp_balanced++;
			}
			else
			{ // unbalanced
				log_key_net_pair(iter->first,iter->second,buf);
				if(agg_bgp_all_h4[(iter->first)].balanced > 0)
				{
					interf_bgp_unbalanced_but_reachable++;
					bgp_unbalanced_f4 << "1, " << buf << endl;
				}
				else
				{
					interf_bgp_unbalanced++;
					bgp_unbalanced_f4 << "0, " << buf << endl;
				}
			}
			iter++;
		}
		bgp_unbalanced_f4.close();

		// log data
		stringstream bgp_path_ss4;
		bgp_path_ss4 << stat_bgp_folder_p4.string();
		bgp_path_ss4 << "/r_" << router << "_i_" << interface;
		bgp_path_ss4 << ".csv";
		ofstream bgp_f4(
			(bgp_path_ss4.str()).c_str(),
			ios_base::app
		);
		bgp_f4 << time_s << ", ";
		bgp_f4 << interf_bgp_balanced << ", ";
		bgp_f4 << interf_bgp_unbalanced << ", ";
		bgp_f4 << interf_bgp_unbalanced_but_reachable << ", ";
		bgp_f4 << endl;
		bgp_f4.close();

		agg_bgp_h4[i]->clear(); // free memory as soon as possible ...
	}; //interfaces

	// All hosts
	uint64_t all_bgp_balanced = 0;
	uint64_t all_bgp_unbalanced = 0;

	// log unbalanced net
	stringstream all_bgp_unbalanced_path_ss4;
	all_bgp_unbalanced_path_ss4 << prefixes_bgp_folder_p4.string();
	all_bgp_unbalanced_path_ss4 << "/all";
	path tmp = all_bgp_unbalanced_path_ss4.str().c_str();
	create_directory(tmp);
	all_bgp_unbalanced_path_ss4 << "/" << time_s /(24*3600) << "";
	tmp = all_bgp_unbalanced_path_ss4.str().c_str();
	create_directory(tmp);
	all_bgp_unbalanced_path_ss4 << "/"<< time_s <<".csv";
	ofstream all_bgp_unbalanced_f4(
		(all_bgp_unbalanced_path_ss4.str()).c_str(),
		ios_base::trunc
	);

	iter = agg_bgp_all_h4.begin();
	end = agg_bgp_all_h4.end();
	while(iter != end)
	{
		if((iter->second).balanced > 0)
			all_bgp_balanced++;
		else
		{
			all_bgp_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_bgp_unbalanced_f4 << "0, " << buf << endl;
		}
		iter++;
	}
	all_bgp_unbalanced_f4.close();
	// log data
	stringstream bgp_all_path_ss4;
	bgp_all_path_ss4 << stat_bgp_folder_p4.string();
	bgp_all_path_ss4 << "/all.csv";
	ofstream bgp_all_f4(
		(bgp_all_path_ss4.str()).c_str(),
		ios_base::app
	);
	bgp_all_f4 << time_s << ", ";
	bgp_all_f4 << all_bgp_balanced << ", ";
	bgp_all_f4 << all_bgp_unbalanced << ", ";
	bgp_all_f4 << 0 << ", ";
	bgp_all_f4 << endl;
	bgp_all_f4.close();

	agg_bgp_all_h4.clear();
	
	//////////////////////////////////////
	// IPv6
	//////////////////////////////////////
	// interface tables
	// analyse the net/48 tables
	for(int i = 0; i < ANALYSER_MAX_ROUTER*ANALYSER_MAX_INTERFACE; i++)
	{
		if(agg_bgp_h6[i] == NULL)
			continue;

		int router =  i/ANALYSER_MAX_INTERFACE;
		int interface = i%ANALYSER_MAX_INTERFACE;

		uint64_t interf_bgp_balanced = 0;
		uint64_t interf_bgp_unbalanced = 0;
		uint64_t interf_bgp_unbalanced_but_reachable = 0;

		// log net prefixes
		stringstream bgp_unbalanced_path_ss6;
		bgp_unbalanced_path_ss6 << prefixes_bgp_folder_p6.string();
		bgp_unbalanced_path_ss6 << "/r_" << router << "_i_" << interface;
		path tmp = bgp_unbalanced_path_ss6.str().c_str();
		create_directory(tmp);
		bgp_unbalanced_path_ss6 << "/" << time_s /(24*3600) << "";
		tmp = bgp_unbalanced_path_ss6.str().c_str();
		create_directory(tmp);
		bgp_unbalanced_path_ss6 << "/"<< time_s <<".csv";
		ofstream bgp_unbalanced_f6(
			(bgp_unbalanced_path_ss6.str()).c_str(),
			ios_base::trunc
		);

		iter = agg_bgp_h6[i]->begin();
		end = agg_bgp_h6[i]->end();
		while(iter != end)
		{
			if((iter->second).balanced > 0)
			{ // balanced
				interf_bgp_balanced++;
			}
			else
			{ // unbalanced
				log_key_net_pair(iter->first,iter->second,buf);
				if(agg_bgp_all_h6[(iter->first)].balanced > 0)
				{
					interf_bgp_unbalanced_but_reachable++;
					bgp_unbalanced_f6 << "1, " << buf << endl;
				}
				else
				{
					interf_bgp_unbalanced++;
					bgp_unbalanced_f6 << "0, " << buf << endl;
				}
			}
			iter++;
		}
		bgp_unbalanced_f6.close();

		// log data
		stringstream bgp_path_ss6;
		bgp_path_ss6 << stat_bgp_folder_p6.string();
		bgp_path_ss6 << "/r_" << router << "_i_" << interface;
		bgp_path_ss6 << ".csv";
		ofstream bgp_f6(
			(bgp_path_ss6.str()).c_str(),
			ios_base::app
		);
		bgp_f6 << time_s << ", ";
		bgp_f6 << interf_bgp_balanced << ", ";
		bgp_f6 << interf_bgp_unbalanced << ", ";
		bgp_f6 << interf_bgp_unbalanced_but_reachable << ", ";
		bgp_f6 << endl;
		bgp_f6.close();

		agg_bgp_h6[i]->clear(); // free memory as soon as possible ...
	}; //interfaces

	// All hosts
	all_bgp_balanced = 0;
	all_bgp_unbalanced = 0;

	// log unbalanced net
	stringstream all_bgp_unbalanced_path_ss6;
	all_bgp_unbalanced_path_ss6 << prefixes_bgp_folder_p6.string();
	all_bgp_unbalanced_path_ss6 << "/all";
	tmp = all_bgp_unbalanced_path_ss6.str().c_str();
	create_directory(tmp);
	all_bgp_unbalanced_path_ss6 << "/" << time_s /(24*3600) << "";
	tmp = all_bgp_unbalanced_path_ss6.str().c_str();
	create_directory(tmp);
	all_bgp_unbalanced_path_ss6 << "/"<< time_s <<".csv";
	ofstream all_bgp_unbalanced_f6(
		(all_bgp_unbalanced_path_ss6.str()).c_str(),
		ios_base::trunc
	);

	iter = agg_bgp_all_h6.begin();
	end = agg_bgp_all_h6.end();
	while(iter != end)
	{
		if((iter->second).balanced > 0)
			all_bgp_balanced++;
		else
		{
			all_bgp_unbalanced++;
			log_key_net_pair(iter->first,iter->second,buf);
			all_bgp_unbalanced_f6 << "0, " << buf << endl;
		}
		iter++;
	}
	all_bgp_unbalanced_f6.close();
	// log data
	stringstream bgp_all_path_ss6;
	bgp_all_path_ss6 << stat_bgp_folder_p6.string();
	bgp_all_path_ss6 << "/all.csv";
	ofstream bgp_all_f6(
		(bgp_all_path_ss6.str()).c_str(),
		ios_base::app
	);
	bgp_all_f6 << time_s << ", ";
	bgp_all_f6 << all_bgp_balanced << ", ";
	bgp_all_f6 << all_bgp_unbalanced << ", ";
	bgp_all_f6 << 0 << ", ";
	bgp_all_f6 << endl;
	bgp_all_f6.close();

	agg_bgp_all_h6.clear();
	
	
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
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_processed4));

	// interfaces IPv4
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_invalid4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_zero4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_not_monitored4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_monitored4));

	// state IPv4
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_processed4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_balanced4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_unbalanced4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_other4));

	// weird connections IPv4
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_processed4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_no_rule4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_zero_packet4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_bpp4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_ok4));

	// signal traffic IPv4
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_processed4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_ok4));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_other4));

	// accounted IPv4
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_accounted_unbalanced4));
	
	// connections IPv6
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_processed6));

	// interfaces IPv6
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_invalid6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_zero6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_not_monitored6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_if_monitored6));

	// state IPv6
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_processed6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_balanced6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_unbalanced6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_state_other6));

	// weird connections IPv6
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_processed6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_no_rule6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_zero_packet6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_bpp6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_weird_cons_ok6));

	// signal traffic IPv6
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_processed6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_ok6));
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_signal_other6));

	// accounted IPv6
	rb_ary_push(array, ULL2NUM(analyser->stat_cons_accounted_unbalanced6));

	return(array);
};
