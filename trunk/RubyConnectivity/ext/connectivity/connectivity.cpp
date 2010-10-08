/**
 * File: connectivity.cpp 
 *
 * Copyright (c) 2010, Dominik Schatzmann 
 *
 * Author: Dominik Schatzmann (mail@dominikschatzmann.ch) 
 * 
 * Data structure to Analyze the Connectivity Matrix of a Network
 *
 */

#include <ruby.h>

#include "connection.h"
#include "connections.h"
static VALUE rb_eConnection;
static VALUE rb_cConnection;
static VALUE rb_eConnections;
static VALUE rb_cConnections;

#include "data_parser.h"
static VALUE rb_eDataParser;
static VALUE rb_cDataParser;

#include "extract_topology.h"
static VALUE rb_eExtractTopology;
static VALUE rb_cExtractTopology;

#include "filter_ipv6.h"
#include "filter_ipv4.h"
#include "filter_border.h"
#include "filter_in_out.h"
#include "filter_prefix_blacklist.h"
static VALUE rb_eFilterIPv6;
static VALUE rb_cFilterIPv6;
static VALUE rb_eFilterIPv4;
static VALUE rb_cFilterIPv4;
static VALUE rb_eFilterBorder;
static VALUE rb_cFilterBorder;
static VALUE rb_eFilterInOut;
static VALUE rb_cFilterInOut;
static VALUE rb_eFilterPrefixBlacklist;
static VALUE rb_cFilterPrefixBlacklist;

#include "connection_matrix_key.h"
#include "connection_matrix_entry.h"
#include "connection_matrix.h"
static VALUE rb_eConnectionMatrixKey;
static VALUE rb_cConnectionMatrixKey;
static VALUE rb_eConnectionMatrixEntry;
static VALUE rb_cConnectionMatrixEntry;
static VALUE rb_eConnectionMatrix;
static VALUE rb_cConnectionMatrix;
/*
#include "analyser_bgp.h"
static VALUE rb_eAnalyserBGP;
static VALUE rb_cAnalyserBGP;
*/
#include "analyser.h"
static VALUE rb_eAnalyser;
static VALUE rb_cAnalyser;

using namespace std;
extern "C" void Init_connectivity() 
{

//------------------------------------------------------------------------------
// Connection
//------------------------------------------------------------------------------
	rb_eConnection = rb_define_class("ConnectionError", rb_eStandardError);
	rb_cConnection = rb_define_class("Connection", rb_cObject);
	rb_define_alloc_func(rb_cConnection, rb_connection_alloc);

	rb_define_const(rb_cConnection, "Version__", rb_str_new2(CONNECTION_VERSION));
	rb_define_const(rb_cConnection, "VersionNbr__", INT2FIX(CONNECTION_VERSION_NBR));

	rb_define_const(rb_cConnection, "DirectionUnknown", INT2FIX(CONNECTION_DIRECTION_UNKNOWN));
	rb_define_const(rb_cConnection, "DirectionInIn", INT2FIX(CONNECTION_DIRECTION_IN_IN));
	rb_define_const(rb_cConnection, "DirectionInOut", INT2FIX(CONNECTION_DIRECTION_IN_OUT));
	rb_define_const(rb_cConnection, "DirectionOutIn", INT2FIX(CONNECTION_DIRECTION_OUT_IN));
	rb_define_const(rb_cConnection, "DirectionOutOut", INT2FIX(CONNECTION_DIRECTION_OUT_OUT));

	rb_define_const(rb_cConnection, "BorderUnknown",  INT2FIX(CONNECTION_BORDER_UNKNOWN));
	rb_define_const(rb_cConnection, "BorderTrue",  INT2FIX(CONNECTION_BORDER_TRUE));
	rb_define_const(rb_cConnection, "BorderFalse", INT2FIX(CONNECTION_BORDER_FALSE));

	rb_define_method(rb_cConnection, "valid", RUBY_METHOD_FUNC(rb_getter_valid), 0);
	rb_define_method(rb_cConnection, "addr_src_s", RUBY_METHOD_FUNC(rb_getter_addr_src_s), 0);
	rb_define_method(rb_cConnection, "addr_dst_s", RUBY_METHOD_FUNC(rb_getter_addr_dst_s), 0);
	rb_define_method(rb_cConnection, "addr_next_s", RUBY_METHOD_FUNC(rb_getter_addr_next_s), 0);
	rb_define_method(rb_cConnection, "port_src", RUBY_METHOD_FUNC(rb_getter_port_src), 0);
	rb_define_method(rb_cConnection, "port_dst", RUBY_METHOD_FUNC(rb_getter_port_dst), 0);
	rb_define_method(rb_cConnection, "protocol", RUBY_METHOD_FUNC(rb_getter_protocol), 0);
	rb_define_method(rb_cConnection, "start_s", RUBY_METHOD_FUNC(rb_getter_start_s), 0);
	rb_define_method(rb_cConnection, "stop_s", RUBY_METHOD_FUNC(rb_getter_stop_s), 0);
	rb_define_method(rb_cConnection, "packets", RUBY_METHOD_FUNC(rb_getter_packets), 0);
	rb_define_method(rb_cConnection, "bytes", RUBY_METHOD_FUNC(rb_getter_bytes), 0);
	rb_define_method(rb_cConnection, "router", RUBY_METHOD_FUNC(rb_getter_router), 0);
	rb_define_method(rb_cConnection, "if_in", RUBY_METHOD_FUNC(rb_getter_if_in), 0);
	rb_define_method(rb_cConnection, "if_out", RUBY_METHOD_FUNC(rb_getter_if_out), 0);
	rb_define_method(rb_cConnection, "direction", RUBY_METHOD_FUNC(rb_getter_direction), 0);
	rb_define_method(rb_cConnection, "border", RUBY_METHOD_FUNC(rb_getter_border), 0);

//------------------------------------------------------------------------------
// Connections
//------------------------------------------------------------------------------
	rb_eConnections = rb_define_class("ConnectionsError", rb_eStandardError);
	rb_cConnections = rb_define_class("Connections", rb_cObject);
	rb_define_alloc_func(rb_cConnections, rb_connections_alloc);

	rb_define_const(rb_cConnections, "Version__", rb_str_new2(CONNECTIONS_VERSION));
	rb_define_const(rb_cConnections, "VersionNbr__", INT2FIX(CONNECTIONS_VERSION_NBR));

	rb_define_method(rb_cConnections, "resize__", RUBY_METHOD_FUNC(rb_connections_resize), 1);
	rb_define_method(rb_cConnections, "capacity__", RUBY_METHOD_FUNC(rb_connections_capacity), 0);
	rb_define_method(rb_cConnections, "used__", RUBY_METHOD_FUNC(rb_connections_used), 0);

	rb_define_method(rb_cConnections, "get_first_unused__", RUBY_METHOD_FUNC(rb_connections_get_first_unused), 0);
	rb_define_method(rb_cConnections, "get_next_unused__", RUBY_METHOD_FUNC(rb_connections_get_next_unused), 0);

	rb_define_method(rb_cConnections, "reset__", RUBY_METHOD_FUNC(rb_connections_reset), 0);
	rb_define_method(rb_cConnections, "each_used__", RUBY_METHOD_FUNC(rb_connections_each_used), 0);
	rb_define_method(rb_cConnections, "each_all__", RUBY_METHOD_FUNC(rb_connections_each_all), 0);

//------------------------------------------------------------------------------
// The Data Parser
//------------------------------------------------------------------------------

	rb_eDataParser = rb_define_class("DataParserError", rb_eStandardError);
	rb_cDataParser = rb_define_class("DataParser", rb_cObject);
	rb_define_alloc_func(rb_cDataParser, rb_data_parser_alloc);

	rb_define_const(rb_cDataParser, "Version__", rb_str_new2(DATA_PARSER_VERSION));
	rb_define_const(rb_cDataParser, "VersionNbr__", INT2FIX(DATA_PARSER_VERSION_NBR));

	// statistics
	rb_define_method(rb_cDataParser, "stat_get__", RUBY_METHOD_FUNC(rb_data_parser_stat_get), 0);
	rb_define_method(rb_cDataParser, "stat_reset__", RUBY_METHOD_FUNC(rb_data_parser_stat_reset), 0);
	rb_define_method(rb_cDataParser, "set_stat_next_export_s__", RUBY_METHOD_FUNC(rb_data_parser_stat_next_export_s), 1);

	// parse data
	rb_define_method(rb_cDataParser, "parse_file__", RUBY_METHOD_FUNC(rb_data_parser_parse_file), 2);

//------------------------------------------------------------------------------
// The Extract Topology
//------------------------------------------------------------------------------
	rb_eExtractTopology = rb_define_class("ExtractTopologyError", rb_eStandardError);
	rb_cExtractTopology = rb_define_class("ExtractTopology", rb_cObject);
	rb_define_alloc_func(rb_cExtractTopology, rb_extract_topology_alloc);

	rb_define_const(rb_cExtractTopology, "Version__", rb_str_new2(EXTRACT_TOPOLOGY_VERSION));
	rb_define_const(rb_cExtractTopology, "VersionNbr__", INT2FIX(EXTRACT_TOPOLOGY_VERSION_NBR));
	rb_define_method(rb_cExtractTopology, "process__", RUBY_METHOD_FUNC(rb_extract_topology_process), 2);

//------------------------------------------------------------------------------
// The Filter IPv6
//------------------------------------------------------------------------------

	rb_eFilterIPv6 = rb_define_class("FilterIPv6Error", rb_eStandardError);
	rb_cFilterIPv6 = rb_define_class("FilterIPv6", rb_cObject);
	rb_define_alloc_func(rb_cFilterIPv6, rb_filter_ipv6_alloc);

	rb_define_const(rb_cFilterIPv6, "Version__", rb_str_new2(FILTER_IPV6_VERSION));
	rb_define_const(rb_cFilterIPv6, "VersionNbr__", INT2FIX(FILTER_IPV6_VERSION_NBR));

	// statistics
	rb_define_method(rb_cFilterIPv6, "stat_get__", RUBY_METHOD_FUNC(rb_filter_ipv6_stat_get), 0);
	rb_define_method(rb_cFilterIPv6, "stat_reset__", RUBY_METHOD_FUNC(rb_filter_ipv6_stat_reset), 0);
	rb_define_method(rb_cFilterIPv6, "set_stat_next_export_s__", RUBY_METHOD_FUNC(rb_filter_ipv6_stat_next_export_s), 1);

	// filter
	rb_define_method(rb_cFilterIPv6, "filter__", RUBY_METHOD_FUNC(rb_filter_ipv6_filter), 1);
//------------------------------------------------------------------------------
// The Filter IPv4
//------------------------------------------------------------------------------

	rb_eFilterIPv4 = rb_define_class("FilterIPv4Error", rb_eStandardError);
	rb_cFilterIPv4 = rb_define_class("FilterIPv4", rb_cObject);
	rb_define_alloc_func(rb_cFilterIPv4, rb_filter_ipv4_alloc);

	rb_define_const(rb_cFilterIPv4, "Version__", rb_str_new2(FILTER_IPV4_VERSION));
	rb_define_const(rb_cFilterIPv4, "VersionNbr__", INT2FIX(FILTER_IPV4_VERSION_NBR));

	// statistics
	rb_define_method(rb_cFilterIPv4, "stat_get__", RUBY_METHOD_FUNC(rb_filter_ipv4_stat_get), 0);
	rb_define_method(rb_cFilterIPv4, "stat_reset__", RUBY_METHOD_FUNC(rb_filter_ipv4_stat_reset), 0);
	rb_define_method(rb_cFilterIPv4, "set_stat_next_export_s__", RUBY_METHOD_FUNC(rb_filter_ipv4_stat_next_export_s), 1);

	// filter
	rb_define_method(rb_cFilterIPv4, "filter__", RUBY_METHOD_FUNC(rb_filter_ipv4_filter), 1);
//------------------------------------------------------------------------------
// The Filter Border
//------------------------------------------------------------------------------

	rb_eFilterBorder = rb_define_class("FilterBorderError", rb_eStandardError);
	rb_cFilterBorder = rb_define_class("FilterBorder", rb_cObject);
	rb_define_alloc_func(rb_cFilterBorder, rb_filter_border_alloc);

	rb_define_const(rb_cFilterBorder, "Version__", rb_str_new2(FILTER_BORDER_VERSION));
	rb_define_const(rb_cFilterBorder, "VersionNbr__", INT2FIX(FILTER_BORDER_VERSION_NBR));

	rb_define_const(rb_cFilterBorder, "TypeUnknown", INT2FIX(FILTER_BORDER_TYPE_UNKNOWN));
	rb_define_const(rb_cFilterBorder, "TypeInternet", INT2FIX(FILTER_BORDER_TYPE_INTERNET));
	rb_define_const(rb_cFilterBorder, "TypeCustomer", INT2FIX(FILTER_BORDER_TYPE_CUSTOMER));
	rb_define_const(rb_cFilterBorder, "TypeBackbone", INT2FIX(FILTER_BORDER_TYPE_BACKBONE));

	// statistics
	rb_define_method(rb_cFilterBorder, "stat_get__", RUBY_METHOD_FUNC(rb_filter_border_stat_get), 0);
	rb_define_method(rb_cFilterBorder, "stat_reset__", RUBY_METHOD_FUNC(rb_filter_border_stat_reset), 0);
	rb_define_method(rb_cFilterBorder, "set_stat_next_export_s__", RUBY_METHOD_FUNC(rb_filter_border_stat_next_export_s), 1);

	// configuration
	rb_define_method(rb_cFilterBorder, "add_interface__", RUBY_METHOD_FUNC(rb_filter_border_add_interface), 3);
	// filter
	rb_define_method(rb_cFilterBorder, "filter__", RUBY_METHOD_FUNC(rb_filter_border_filter), 1);

//------------------------------------------------------------------------------
// The Filter In Out
//------------------------------------------------------------------------------
	rb_eFilterInOut = rb_define_class("FilterInOutError", rb_eStandardError);
	rb_cFilterInOut = rb_define_class("FilterInOut", rb_cObject);
	rb_define_alloc_func(rb_cFilterInOut, rb_filter_in_out_alloc);

	rb_define_const(rb_cFilterInOut, "Version__", rb_str_new2(FILTER_IN_OUT_VERSION));
	rb_define_const(rb_cFilterInOut, "VersionNbr__", INT2FIX(FILTER_IN_OUT_VERSION_NBR));

	// statistics
	rb_define_method(rb_cFilterInOut, "stat_get__", RUBY_METHOD_FUNC(rb_filter_in_out_stat_get), 0);
	rb_define_method(rb_cFilterInOut, "stat_reset__", RUBY_METHOD_FUNC(rb_filter_in_out_stat_reset), 0);
	rb_define_method(rb_cFilterInOut, "set_stat_next_export_s__", RUBY_METHOD_FUNC(rb_filter_in_out_stat_next_export_s), 1);

	// configuration
	rb_define_method(rb_cFilterInOut, "add_prefix__", RUBY_METHOD_FUNC(rb_filter_in_out_add_prefix), 2);

	// filter
	rb_define_method(rb_cFilterInOut, "filter__", RUBY_METHOD_FUNC(rb_filter_in_out_filter), 1);

//------------------------------------------------------------------------------
// The Filter Prefix Blacklist
//------------------------------------------------------------------------------

	rb_eFilterPrefixBlacklist = rb_define_class("FilterPrefixBlacklistError", rb_eStandardError);
	rb_cFilterPrefixBlacklist = rb_define_class("FilterPrefixBlacklist", rb_cObject);
	rb_define_alloc_func(rb_cFilterPrefixBlacklist, rb_filter_prefix_blacklist_alloc);

	rb_define_const(rb_cFilterPrefixBlacklist, "Version__", rb_str_new2(FILTER_PREFIX_BLACKLIST_VERSION));
	rb_define_const(rb_cFilterPrefixBlacklist, "VersionNbr__", INT2FIX(FILTER_PREFIX_BLACKLIST_VERSION_NBR));

	// statistics
	rb_define_method(rb_cFilterPrefixBlacklist, "stat_get__", RUBY_METHOD_FUNC(rb_filter_prefix_blacklist_stat_get), 0);
	rb_define_method(rb_cFilterPrefixBlacklist, "stat_reset__", RUBY_METHOD_FUNC(rb_filter_prefix_blacklist_stat_reset), 0);
	rb_define_method(rb_cFilterPrefixBlacklist, "set_stat_next_export_s__", RUBY_METHOD_FUNC(rb_filter_prefix_blacklist_stat_next_export_s), 1);

	// configuration
	rb_define_method(rb_cFilterPrefixBlacklist, "add_prefix__", RUBY_METHOD_FUNC(rb_filter_prefix_blacklist_add_prefix), 2);

	// filter
	rb_define_method(rb_cFilterPrefixBlacklist, "filter__", RUBY_METHOD_FUNC(rb_filter_prefix_blacklist_filter), 1);

//------------------------------------------------------------------------------
// The Connection Matrix Key
//------------------------------------------------------------------------------
	rb_eConnectionMatrixKey = rb_define_class("ConnectionMatrixKeyError", rb_eStandardError);
	rb_cConnectionMatrixKey = rb_define_class("ConnectionMatrixKey", rb_cObject);
	rb_define_alloc_func(rb_cConnectionMatrixKey, rb_connection_matrix_key_alloc);

	rb_define_const(rb_cConnectionMatrixKey, "Version__", rb_str_new2(CONNECTION_MATRIX_KEY_VERSION));
	rb_define_const(rb_cConnectionMatrixKey, "VersionNbr__", INT2FIX(CONNECTION_MATRIX_KEY_VERSION_NBR));

//------------------------------------------------------------------------------
// The Connection Matrix Entry
//------------------------------------------------------------------------------
	rb_eConnectionMatrixEntry = rb_define_class("ConnectionMatrixEntryError", rb_eStandardError);
	rb_cConnectionMatrixEntry = rb_define_class("ConnectionMatrixKey", rb_cObject);
	rb_define_alloc_func(rb_cConnectionMatrixEntry, rb_connection_matrix_entry_alloc);

	rb_define_const(rb_cConnectionMatrixEntry, "Version__", rb_str_new2(CONNECTION_MATRIX_ENTRY_VERSION));
	rb_define_const(rb_cConnectionMatrixEntry, "VersionNbr__", INT2FIX(CONNECTION_MATRIX_ENTRY_VERSION_NBR));

//------------------------------------------------------------------------------
// The Connection Matrix
//------------------------------------------------------------------------------
	rb_eConnectionMatrix = rb_define_class("ConnectionMatrixError", rb_eStandardError);
	rb_cConnectionMatrix = rb_define_class("ConnectionMatrix", rb_cObject);
	rb_define_alloc_func(rb_cConnectionMatrix, rb_connection_matrix_alloc);

	rb_define_const(rb_cConnectionMatrix, "Version__", rb_str_new2(CONNECTION_MATRIX_VERSION));
	rb_define_const(rb_cConnectionMatrix, "VersionNbr__", INT2FIX(CONNECTION_MATRIX_VERSION_NBR));

	rb_define_method(rb_cConnectionMatrix, "statistics_get__", RUBY_METHOD_FUNC(rb_connection_matrix_stat_get), 0);
	rb_define_method(rb_cConnectionMatrix, "statistics_reset__", RUBY_METHOD_FUNC(rb_connection_matrix_stat_reset), 0);
	rb_define_method(rb_cConnectionMatrix, "set_periodic_job_next_s__", RUBY_METHOD_FUNC(rb_connection_matrix_periodic_job_next_s), 1);
	rb_define_method(rb_cConnectionMatrix, "add_connections__", RUBY_METHOD_FUNC(rb_connection_matrix_add_connections), 1);
	rb_define_method(rb_cConnectionMatrix, "prune__", RUBY_METHOD_FUNC(rb_connection_matrix_prune), 1);

	rb_define_method(rb_cConnectionMatrix, "analysis__", RUBY_METHOD_FUNC(rb_connection_matrix_analyze), 2);

/*
//------------------------------------------------------------------------------
// The Analyser BGP Prefixes
//------------------------------------------------------------------------------
	rb_eAnalyserBGP = rb_define_class("AnalyserBGPError", rb_eStandardError);
	rb_cAnalyserBGP = rb_define_class("AnalyserBGP", rb_cObject);
	rb_define_alloc_func(rb_cAnalyserBGP, rb_analyser_bgp_alloc);

	rb_define_const(rb_cAnalyserBGP, "Version__", rb_str_new2(ANALYSER_BGP_VERSION));
	rb_define_const(rb_cAnalyserBGP, "VersionNbr__", INT2FIX(ANALYSER_BGP_VERSION_NBR));

	rb_define_method(rb_cAnalyserBGP, "stat_get__", RUBY_METHOD_FUNC(rb_analyser_bgp_stat_get), 0);
	rb_define_method(rb_cAnalyserBGP, "stat_reset__", RUBY_METHOD_FUNC(rb_analyser_bgp_stat_reset), 0);

	rb_define_method(rb_cAnalyserBGP, "set_working_folder__", RUBY_METHOD_FUNC(rb_analyser_bgp_set_working_folder), 1);
	rb_define_method(rb_cAnalyserBGP, "set_net_sizes__", RUBY_METHOD_FUNC(rb_analyser_bgp_set_net_sizes), 2);
	rb_define_method(rb_cAnalyserBGP, "add_prefix__", RUBY_METHOD_FUNC(rb_analyser_bgp_add_prefix), 2);
	rb_define_method(rb_cAnalyserBGP, "set_router__", RUBY_METHOD_FUNC(rb_analyser_bgp_set_router), 1);
	rb_define_method(rb_cAnalyserBGP, "set_interface__", RUBY_METHOD_FUNC(rb_analyser_bgp_set_interface), 1);

	rb_define_method(rb_cAnalyserBGP, "analyse__", RUBY_METHOD_FUNC(rb_analyser_bgp_analyse), 2);

	rb_define_method(rb_cAnalyserBGP, "export_unbalanced_prefixes__", RUBY_METHOD_FUNC(rb_analyser_bgp_export_unbalanced_prefixes), 1);
	rb_define_method(rb_cAnalyserBGP, "export_density__", RUBY_METHOD_FUNC(rb_analyser_bgp_export_density), 1);
*/

//------------------------------------------------------------------------------
// The Analyser
//------------------------------------------------------------------------------
	rb_eAnalyser = rb_define_class("AnalyserError", rb_eStandardError);
	rb_cAnalyser = rb_define_class("Analyser", rb_cObject);
	rb_define_alloc_func(rb_cAnalyser, rb_analyser_alloc);

	rb_define_const(rb_cAnalyser, "Version__", rb_str_new2(ANALYSER_VERSION));
	rb_define_const(rb_cAnalyser, "VersionNbr__", INT2FIX(ANALYSER_VERSION_NBR));

	rb_define_method(rb_cAnalyser, "reset__", RUBY_METHOD_FUNC(rb_analyser_reset), 0);

	rb_define_method(rb_cAnalyser, "set_working_folder__", RUBY_METHOD_FUNC(rb_analyser_set_working_folder), 1);
	rb_define_method(rb_cAnalyser, "add_interface__", RUBY_METHOD_FUNC(rb_analyser_add_interface), 2);
	rb_define_method(rb_cAnalyser, "add_prefix__", RUBY_METHOD_FUNC(rb_analyser_add_prefix), 2);

	rb_define_method(rb_cAnalyser, "analyse__", RUBY_METHOD_FUNC(rb_analyser_analyse), 2);

	rb_define_method(rb_cAnalyser, "stat_get__", RUBY_METHOD_FUNC(rb_analyser_stat_get), 0);

};

