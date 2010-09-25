#------------------------------------------------------------------------------#
# The Analyser BGP analyses the connection matrix for unbalanced Prefixes 
# including the possibility to aggregate the unbalanced connections CIDR
# network prefixes.
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#
require_relative 'helper.rb'
class AnalyserBGP

	CGF_Columns = 3
	def initialize(output)

		# result path
		@result_p = output
		FileUtils::mkdir_p(@result_p)
		set_working_folder__(@result_p)

		# statistics
		@stat_f = File.open("#{@result_p}/stat.csv", 'w')
		@stat_f.puts statistics_head_to_csv()
		statistics_reset()

		# unbalanced prefixes
		@prefixes_p = "#{@result_p}/prefixes"
		FileUtils::mkdir_p(@prefixes_p)

		# density
		@density_p = "#{@result_p}/density"
		FileUtils::mkdir_p(@density_p)

	end
################################################################################
### statistics  ################################################################
################################################################################
	def statistics_head_to_csv
		s = '# '
		s += 'time_s, '

		s += '5tps_processed, '
		s += '5tps_balanced, '
		s += '5tps_unbalanced, '
		s += '5tps_unbalanced_weird_connection, '
		s += '5tps_unbalanced_not_signal, '

		s += 'net_net_items, '
		s += 'net_net_balanced, '
		s += 'net_net_unbalanced, '

		s += 'net_items, '
		s += 'net_balanced, '
		s += 'net_unbalanced, '
		return(s)
	end
	def statistics_data_to_csv(export_time_s)
		s = ''
		s += "#{export_time_s}, "
		s += "#{@stat_5tps_processed}, "
		s += "#{@stat_5tps_balanced}, "
		s += "#{@stat_5tps_unbalanced}, "
		s += "#{@stat_5tps_unbalanced_weird_connection}, "
		s += "#{@stat_5tps_unbalanced_not_signal}, "

		s += "#{@stat_net_net_items}, "
		s += "#{@stat_net_net_balanced}, "
		s += "#{@stat_net_net_unbalanced}, "

		s += "#{@stat_net_items}, "
		s += "#{@stat_net_balanced}, "
		s += "#{@stat_net_unbalanced}, "
		return(s)
	end
	def statistics_reset()
		stat_reset__()
		@stat_5tps_processed = 0
		@stat_5tps_balanced = 0
		@stat_5tps_unbalanced = 0
		@stat_5tps_unbalanced_weird_connection = 0
		@stat_5tps_unbalanced_not_signal = 0

		@stat_net_net_items = 0
		@stat_net_net_balanced = 0
		@stat_net_net_unbalanced = 0

		@stat_net_items = 0
		@stat_net_balanced = 0
		@stat_net_unbalanced = 0
	end
	def statistics_get_from_ext()
		stats_a = stat_get__()
		@stat_5tps_processed = stats_a[0]
		@stat_5tps_balanced = stats_a[1]
		@stat_5tps_unbalanced = stats_a[2]
		@stat_5tps_unbalanced_weird_connection = stats_a[3]
		@stat_5tps_unbalanced_not_signal = stats_a[4]

		@stat_net_net_items = stats_a[5]
		@stat_net_net_balanced = stats_a[6]
		@stat_net_net_unbalanced = stats_a[7]

		@stat_net_items = stats_a[8]
		@stat_net_balanced = stats_a[9]
		@stat_net_unbalanced = stats_a[10]
	end
	def statistics_export(export_time_s)
		@stat_f.puts statistics_data_to_csv(export_time_s)
		@stat_f.flush()
	end
################################################################################
### configuration ##############################################################
################################################################################
	def set_net_sizes(net_in, net_out)
		# a = 0 ==> BGP
		# a = [1-32] ==> X.X.X.X/a
		set_net_sizes__(net_in, net_out)
	end
	def load_prefixes(file_p)

		File.open(file_p).each do |line|

			# filter out comments
			if line.size() < 4 or line =~ /^#/
				next
			end

			# parse the data
			data = line.chomp!.split('/')
			if data.size != CGF_Columns
				throw " Format Error in '#{file_p}' on line #{$.} :: '#{line}'"
			end

			# prepare data
			addr = data[0]
			addr_length = data[1].to_i

			# we care only about border interfaces
			add_prefix__("#{addr}/#{addr_length}", addr_length)
		end
	end
	def set_router(router)
		set_router__(router)
	end
	def set_interface(interface)
		set_interface__(interface)
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
		analyse__(cons_matrix, time_s)

		# dump some simple statistics
		statistics_get_from_ext()
		statistics_export(time_s)

		# export the unbalanced prefixes
		export_unbalanced_prefixes(time_s)

		# export the density 
		export_density(time_s)

		statistics_reset()
	end
################################################################################
### debug  #####################################################################
################################################################################
	def to_s
		s =  "AnalyserBGP (#{Version}) \n"
		s += "Processed 5tps: #{@stat_5tps_processed} 5tps\n"
	end
end


