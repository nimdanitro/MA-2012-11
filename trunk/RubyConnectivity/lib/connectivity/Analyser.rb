#------------------------------------------------------------------------------#
# The Analyser analyses the connection matrix for unbalanced Prefixes 
# including the possibility to aggregate the unbalanced connections CIDR
# network prefixes.
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#
require_relative 'helper.rb'
class Analyser

	CGF_Columns = 2
	def initialize(output)

		# result path
		@result_p = output
		FileUtils::mkdir_p(@result_p)
		set_working_folder__(@result_p)

		# statistics
		@stat_f = File.open("#{@result_p}/stat.csv", 'w')
		@stat_f.puts statistics_head_to_csv()
		statistics_reset()

	end
################################################################################
### statistics  ################################################################
################################################################################
	def statistics_head_to_csv
		s = '# '
		s += 'time_s, '

		s += 'stat_cons_processed4, '

		s += 'stat_cons_if_invalid4, '
		s += 'stat_cons_if_zero4, '
		s += 'stat_cons_if_not_monitored4, '
		s += 'stat_cons_if_monitored4, '

		s += 'stat_cons_state_processed4, '
		s += 'stat_cons_state_balanced4, '
		s += 'stat_cons_state_unbalanced4, '
		s += 'stat_cons_state_other4, '

		s += "stat_cons_weird_cons_processed4, "
		s += "stat_cons_weird_cons_no_rule4, "
		s += "stat_cons_weird_cons_zero_packet4, "
		s += "stat_cons_weird_cons_bpp4, "
		s += "stat_cons_weird_cons_ok4, "

		s += "stat_cons_signal_processed4, "
		s += "stat_cons_signal_ok4, "
		s += "stat_cons_signal_other4, "

		s += "stat_cons_accounted_unbalanced4, "
		
		s += 'stat_cons_processed6, '

		s += 'stat_cons_if_invalid6, '
		s += 'stat_cons_if_zero6, '
		s += 'stat_cons_if_not_monitored6, '
		s += 'stat_cons_if_monitored6, '
		
		s += 'stat_cons_state_processed6, '
		s += 'stat_cons_state_balanced6, '
		s += 'stat_cons_state_unbalanced6, '
		s += 'stat_cons_state_other6, '

		s += "stat_cons_weird_cons_processed6, "
		s += "stat_cons_weird_cons_no_rule6, "
		s += "stat_cons_weird_cons_zero_packet6, "
		s += "stat_cons_weird_cons_bpp6, "
		s += "stat_cons_weird_cons_ok6, "

		s += "stat_cons_signal_processed6, "
		s += "stat_cons_signal_ok6, "
		s += "stat_cons_signal_other6, "

		s += "stat_cons_accounted_unbalanced6, "
		
		s += "stat_delay_s, "

		return(s)
	end
	def statistics_data_to_csv(export_time_s)
		s = ''
		s += "#{export_time_s}, "

		s += "#{@stat_cons_processed4}, "

		s += "#{@stat_cons_if_invalid4}, "
		s += "#{@stat_cons_if_zero4}, "
		s += "#{@stat_cons_if_not_monitored4}, "
		s += "#{@stat_cons_if_monitored4}, "

		s += "#{@stat_cons_state_processed4}, "
		s += "#{@stat_cons_state_balanced4}, "
		s += "#{@stat_cons_state_unbalanced4}, "
		s += "#{@stat_cons_state_other4}, "

		s += "#{@stat_cons_weird_cons_processed4}, "
		s += "#{@stat_cons_weird_cons_no_rule4}, "
		s += "#{@stat_cons_weird_cons_zero_packet4}, "
		s += "#{@stat_cons_weird_cons_bpp4}, "
		s += "#{@stat_cons_weird_cons_ok4}, "

		s += "#{@stat_cons_signal_processed4}, "
		s += "#{@stat_cons_signal_ok4}, "
		s += "#{@stat_cons_signal_other4}, "

		s += "#{@stat_cons_accounted_unbalanced4}, "
		
		s += "#{@stat_cons_processed6}, "

		s += "#{@stat_cons_if_invalid6}, "
		s += "#{@stat_cons_if_zero6}, "
		s += "#{@stat_cons_if_not_monitored6}, "
		s += "#{@stat_cons_if_monitored6}, "

		s += "#{@stat_cons_state_processed6}, "
		s += "#{@stat_cons_state_balanced6}, "
		s += "#{@stat_cons_state_unbalanced6}, "
		s += "#{@stat_cons_state_other6}, "

		s += "#{@stat_cons_weird_cons_processed6}, "
		s += "#{@stat_cons_weird_cons_no_rule6}, "
		s += "#{@stat_cons_weird_cons_zero_packet6}, "
		s += "#{@stat_cons_weird_cons_bpp6}, "
		s += "#{@stat_cons_weird_cons_ok6}, "

		s += "#{@stat_cons_signal_processed6}, "
		s += "#{@stat_cons_signal_ok6}, "
		s += "#{@stat_cons_signal_other6}, "

		s += "#{@stat_cons_accounted_unbalanced6}, "

		s += "#{@stat_delay_s}, "

		return(s)
	end
	def statistics_reset()
		reset__()
		@stat_cons_processed4 = 0

		@stat_cons_if_invalid4 = 0
		@stat_cons_if_zero4 = 0
		@stat_cons_if_not_monitored4 = 0
		@stat_cons_if_monitored4 = 0

		@stat_cons_state_balanced4 = 0
		@stat_cons_state_unbalanced4 = 0
		@stat_cons_state_other4 = 0

		@stat_cons_weird_cons_processed4 = 0
		@stat_cons_weird_cons_no_rule4 = 0
		@stat_cons_weird_cons_zero_packet4 = 0
		@stat_cons_weird_cons_bpp4 = 0
		@stat_cons_weird_cons_ok4 = 0

		@stat_cons_signal_processed4 = 0
		@stat_cons_signal_ok4 = 0
		@stat_cons_signal_other4 = 0

		@stat_cons_accounted_unbalanced4 = 0
		
		@stat_cons_processed6 = 0

		@stat_cons_if_invalid6 = 0
		@stat_cons_if_zero6 = 0
		@stat_cons_if_not_monitored6 = 0
		@stat_cons_if_monitored6 = 0

		@stat_cons_state_balanced6 = 0
		@stat_cons_state_unbalanced6 = 0
		@stat_cons_state_other6 = 0

		@stat_cons_weird_cons_processed6 = 0
		@stat_cons_weird_cons_no_rule6 = 0
		@stat_cons_weird_cons_zero_packet6 = 0
		@stat_cons_weird_cons_bpp6 = 0
		@stat_cons_weird_cons_ok6 = 0

		@stat_cons_signal_processed6 = 0
		@stat_cons_signal_ok6 = 0
		@stat_cons_signal_other6 = 0

		@stat_cons_accounted_unbalanced6 = 0

		@stat_delay_s = 0
	end
	def statistics_get_from_ext()
		stats_a = stat_get__()
		@stat_cons_processed4 = stats_a[0]

		offset = 1
		@stat_cons_if_invalid4 = stats_a[offset]
		@stat_cons_if_zero4 = stats_a[offset+1]
		@stat_cons_if_not_monitored4 = stats_a[offset+2]
		@stat_cons_if_monitored4 = stats_a[offset+3]

		offset = 5
		@stat_cons_state_processed4 = stats_a[offset]
		@stat_cons_state_balanced4 = stats_a[offset+1]
		@stat_cons_state_unbalanced4 = stats_a[offset+2]
		@stat_cons_state_other4 = stats_a[offset+3]

		offset = 9
		@stat_cons_weird_cons_processed4 = stats_a[offset]
		@stat_cons_weird_cons_no_rule4 = stats_a[offset+1]
		@stat_cons_weird_cons_zero_packet4 = stats_a[offset+2]
		@stat_cons_weird_cons_bpp4 = stats_a[offset+3]
		@stat_cons_weird_cons_ok4 = stats_a[offset+4]

		offset = 14
		@stat_cons_signal_processed4 = stats_a[offset]
		@stat_cons_signal_ok4 = stats_a[offset+1]
		@stat_cons_signal_other4 = stats_a[offset+2]

		offset = 17
		@stat_cons_accounted_unbalanced4 = stats_a[offset]
		
		@stat_cons_processed6 = stats_a[18]

		offset = 19
		@stat_cons_if_invalid6 = stats_a[offset]
		@stat_cons_if_zero6 = stats_a[offset+1]
		@stat_cons_if_not_monitored6 = stats_a[offset+2]
		@stat_cons_if_monitored6 = stats_a[offset+3]

		offset = 23
		@stat_cons_state_processed6 = stats_a[offset]
		@stat_cons_state_balanced6 = stats_a[offset+1]
		@stat_cons_state_unbalanced6 = stats_a[offset+2]
		@stat_cons_state_other6 = stats_a[offset+3]

		offset = 27
		@stat_cons_weird_cons_processed6 = stats_a[offset]
		@stat_cons_weird_cons_no_rule6 = stats_a[offset+1]
		@stat_cons_weird_cons_zero_packet6 = stats_a[offset+2]
		@stat_cons_weird_cons_bpp6 = stats_a[offset+3]
		@stat_cons_weird_cons_ok6 = stats_a[offset+4]

		offset = 32
		@stat_cons_signal_processed6 = stats_a[offset]
		@stat_cons_signal_ok6 = stats_a[offset+1]
		@stat_cons_signal_other6 = stats_a[offset+2]

		offset = 35
		@stat_cons_accounted_unbalanced6 = stats_a[offset]
		
	end
	def statistics_export(export_time_s)
		@stat_f.puts statistics_data_to_csv(export_time_s)
		@stat_f.flush()
	end
################################################################################
### configuration ##############################################################
################################################################################
	def add_interface(router, interface)
		add_interface__(router, interface)
	end
	def load_prefixes(file_p)

		File.open(file_p).each do |line|

			# filter out comments
			if line.size() < 4 or line =~ /^#/
				next
			end

			# parse the data
			data = line.strip.split('/')
			#if data.size != CGF_Columns
			#	throw " Format Error in '#{file_p}' on line #{$.} :: '#{line}'"
			#qend

			# prepare data
			#puts line
			addr = data[0]
			addr_length = data[1].to_i
			#puts "#{addr}/#{addr_length} => #{addr_length}"
			#STDOUT.flush()
			# we care only about border interfaces
			add_prefix__("#{addr}/#{addr_length}", addr_length)
		end
	end
################################################################################
### analyse ####################################################################
################################################################################
	def export_unbalanced_prefixes(time_s)
		folder = "#{@prefixes_p}/#{time_s/(24*3600)}/"
		FileUtils::mkdir_p(folder)
		file_p = "#{folder}/#{time_s}.csv"
		export_unbalanced_prefixes__(file_p)
	end
	def export_density(time_s)
		folder = "#{@density_p}/#{time_s/(24*3600)}/"
		FileUtils::mkdir_p(folder)
		file_p = "#{folder}/#{time_s}.csv"
		export_density__(file_p)
	end
	def analyse(cons_matrix, time_s)

		# analyse the connection matrix
		start_s = Time.now
		analyse__(cons_matrix, time_s)
		@stat_delay_s = Time.now - start_s
	
		# dump some simple statistics
		statistics_get_from_ext()
		statistics_export(time_s)
		statistics_reset()
	end
################################################################################
### debug  #####################################################################
################################################################################
	def to_s
		s =  "Analyser (#{Version}) \n"
	end
end


