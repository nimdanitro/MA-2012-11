#!/usr/bin/ruby1.9
require_relative '../lib/connectivity/helper.rb'

if ARGV[0] == nil or  ARGV[1] == nil or ARGV[2] == nil
	puts "Usage <config folder> <input folder> <output folder>"
	exit
end

# config 
config_folder = ARGV[0]
puts "Config Folder '#{config_folder}'"
# input
input_folder = ARGV[1]
input_files_a = Dir["#{input_folder}/dc_*_*PBD2*"].sort!

puts "Input Files '#{input_files_a*','}'"

# output
output_folder = ARGV[2]
FileUtils::mkdir_p(output_folder)

### CONFIGURATION  #############################################################

## DATA PARSER
data_parser = DataParser.new(output_folder + '/data_parser', 300)

## FILTER BANK
# filter all ipv6 traffic
filter_ipv6 = FilterIPv6.new(output_folder + '/filter_ipv6', 300)
# filter all ipv4 traffic
filter_ipv4 = FilterIPv4.new(output_folder + '/filter_ipv4', 300)
# filter all non border traffic
filter_border =  FilterBorder.new(output_folder + '/filter_border', 300)
filter_border.load_border_interfaces("#{config_folder}/filter_border/router_interfaces.txt")
# filter all in-in out-out traffic
filter_in_out =  FilterInOut.new(output_folder + '/filter_in_out', 300)
filter_in_out.load_internal_prefixes("#{config_folder}/filter_in_out/switch_prefixes.txt")
# filter blacklisted prefixes (single hosts, address ranges, full ases ...
filter_prefix_blacklist =  FilterPrefixBlacklist.new(output_folder + '/filter_prefix_blacklist', 300)
filter_prefix_blacklist.load_prefix_folder("#{config_folder}/filter_prefix_blacklist")

## CONNECTION MATRIX
connection_matrix_output = output_folder + '/connection_matrix'
connection_matrix_perodic_s = 300
connection_matrix_cache_duration_s = 900
connection_matrix_cache_max = 1000000000
connection_matrix_cache_prune_interval_s = 300
connection_matrix_analysis_delay_s = 360
connection_matrix_analysis_active_duration_s = 300

connection_matrix = ConnectionMatrix.new(
	connection_matrix_output,
	connection_matrix_perodic_s,
	connection_matrix_cache_duration_s,
	connection_matrix_cache_max,
	connection_matrix_cache_prune_interval_s,
	connection_matrix_analysis_delay_s,
	connection_matrix_analysis_active_duration_s
)

################################################################################
## ANALYSER ###################################################################
################################################################################
analyser = Analyser.new(output_folder + '/analyser/IPv4')
analyser.load_prefixes("#{config_folder}/analyser/prefixes.txt")
analyser.add_interface(1, 2)  # Equinix Exchange Zurich
analyser.add_interface(1, 6)  # AS1299  Telia
analyser.add_interface(2, 1)  # AS20965  GEANT2
analyser.add_interface(2, 2)  # CERN
analyser.add_interface(2, 11) # Cern Internet Exchange Point (CIXP)
analyser.add_interface(2, 15) # AS3549  GBLX
analyser.add_interface(2, 16) # AS3549  GBLX
analyser.add_interface(3, 25) # SwissIX
analyser.add_interface(4, 76) # AS20965  GEANT2
analyser.add_interface(4, 81) # AMS-IX
analyser.add_interface(4, 86) # AS3303 Swisscom (CERN, Meyrin GE)
analyser.add_interface(5, 5)  # AS3303 Swisscom (Equinix Zurich)
analyser.add_interface(5, 36) # SwissIX
analyser.add_interface(6, 6)  # BelWue
################################################################################
## Process Monitor #############################################################
################################################################################
process_monitor = ProcessMonitor.new(output_folder + '/process_monitor')

################################################################################
## DO SOME WORK ################################################################
################################################################################
global_time_s = 0
process_monitor.reset
process_monitor.period_start
process_monitor.data_parser_start
data_parser.parse_files(input_files_a) do |cons|
	process_monitor.data_parser_stop
	# Filter bank
	process_monitor.filter_start
	#filter_ipv6.filter cons
	filter_ipv4.filter cons
	filter_border.filter cons
	filter_in_out.filter cons
	filter_prefix_blacklist.filter cons
	process_monitor.filter_stop

	# Connection Matrix
	process_monitor.connection_matrix_start	
  connection_matrix.add_connections(cons) do |time_s|
		process_monitor.connection_matrix_stop
		next if time_s == 0 # skip the zero interval
		puts "#{time_s} :: Analyse the Connection Matrix"
		STDOUT.flush()
		process_monitor.analyser_start
		analyser.analyse(connection_matrix, time_s)
		process_monitor.analyser_stop

		# process monitor
		process_monitor.period_stop(time_s)
		process_monitor.period_start

		process_monitor.connection_matrix_start
	end
	process_monitor.connection_matrix_stop
	process_monitor.data_parser_start
end
process_monitor.data_parser_stop
