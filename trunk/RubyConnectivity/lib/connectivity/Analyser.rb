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

		s += 'stat_cons_processed, '

		s += 'stat_cons_if_invalid, '
		s += 'stat_cons_if_zero, '
		s += 'stat_cons_if_not_monitored, '
		s += 'stat_cons_if_monitored, '

		s += 'stat_cons_state_processed, '
		s += 'stat_cons_state_balanced, '
		s += 'stat_cons_state_unbalanced, '
		s += 'stat_cons_state_other, '

		s += "stat_cons_weird_cons_processed, "
		s += "stat_cons_weird_cons_no_rule, "
		s += "stat_cons_weird_cons_zero_packet, "
		s += "stat_cons_weird_cons_bpp, "
		s += "stat_cons_weird_cons_ok, "

		s += "stat_cons_signal_processed, "
		s += "stat_cons_signal_ok, "
		s += "stat_cons_signal_other, "

		s += "stat_cons_accounted_unbalanced, "
		
		s += "stat_delay_s, "

		return(s)
	end
	def statistics_data_to_csv(export_time_s)
		s = ''
		s += "#{export_time_s}, "

		s += "#{@stat_cons_processed}, "

		s += "#{@stat_cons_if_invalid}, "
		s += "#{@stat_cons_if_zero}, "
		s += "#{@stat_cons_if_not_monitored}, "
		s += "#{@stat_cons_if_monitored}, "

		s += "#{@stat_cons_state_processed}, "
		s += "#{@stat_cons_state_balanced}, "
		s += "#{@stat_cons_state_unbalanced}, "
		s += "#{@stat_cons_state_other}, "

		s += "#{@stat_cons_weird_cons_processed}, "
		s += "#{@stat_cons_weird_cons_no_rule}, "
		s += "#{@stat_cons_weird_cons_zero_packet}, "
		s += "#{@stat_cons_weird_cons_bpp}, "
		s += "#{@stat_cons_weird_cons_ok}, "

		s += "#{@stat_cons_signal_processed}, "
		s += "#{@stat_cons_signal_ok}, "
		s += "#{@stat_cons_signal_other}, "

		s += "#{@stat_cons_accounted_unbalanced}, "

		s += "#{@stat_delay_s}, "

		return(s)
	end
	def statistics_reset()
		reset__()
		@stat_cons_processed = 0

		@stat_cons_if_invalid = 0
		@stat_cons_if_zero = 0
		@stat_cons_if_not_monitored = 0
		@stat_cons_if_monitored = 0

		@stat_cons_state_balanced = 0
		@stat_cons_state_unbalanced = 0
		@stat_cons_state_other = 0

		@stat_cons_weird_cons_processed = 0
		@stat_cons_weird_cons_no_rule = 0
		@stat_cons_weird_cons_zero_packet = 0
		@stat_cons_weird_cons_bpp = 0
		@stat_cons_weird_cons_ok = 0

		@stat_cons_signal_processed = 0
		@stat_cons_signal_ok = 0
		@stat_cons_signal_other = 0

		@stat_cons_accounted_unbalanced = 0

		@stat_delay_s = 0
	end
	def statistics_get_from_ext()
		stats_a = stat_get__()
		@stat_cons_processed = stats_a[0]

		offset = 1
		@stat_cons_if_invalid = stats_a[offset]
		@stat_cons_if_zero = stats_a[offset+1]
		@stat_cons_if_not_monitored = stats_a[offset+2]
		@stat_cons_if_monitored = stats_a[offset+3]

		offset = 5
		@stat_cons_state_processed = stats_a[offset]
		@stat_cons_state_balanced = stats_a[offset+1]
		@stat_cons_state_unbalanced = stats_a[offset+2]
		@stat_cons_state_other = stats_a[offset+3]

		offset = 9
		@stat_cons_weird_cons_processed = stats_a[offset]
		@stat_cons_weird_cons_no_rule = stats_a[offset+1]
		@stat_cons_weird_cons_zero_packet = stats_a[offset+2]
		@stat_cons_weird_cons_bpp = stats_a[offset+3]
		@stat_cons_weird_cons_ok = stats_a[offset+4]

		offset = 14
		@stat_cons_signal_processed = stats_a[offset]
		@stat_cons_signal_ok = stats_a[offset+1]
		@stat_cons_signal_other = stats_a[offset+2]

		offset = 17
		@stat_cons_accounted_unbalanced = stats_a[offset]
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


