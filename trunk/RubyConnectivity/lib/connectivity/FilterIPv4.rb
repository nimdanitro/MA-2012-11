#------------------------------------------------------------------------------#
# The Filter IPv4 marks all IPv4 flows as invalid 
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class FilterIPv4

	def initialize(
		output,
		stat_interval_s = 300
	)
		# result path
		@result_p = output
		FileUtils::mkdir_p(@result_p)

		# statistics
		@stat_f = File.open("#{@result_p}/stat.csv", 'w')
		@stat_f.puts statistics_head_to_csv()
		statistics_reset()

		@stat_export_interval_s = stat_interval_s;
		@stat_export_next_s = 0;
	end
################################################################################
### statistics  ################################################################
################################################################################
	def statistics_head_to_csv
		s = '# '
		s += 'time_s, '
		s += 'processed, '
		s += 'filtered, '
		return(s)
	end
	def statistics_data_to_csv(export_time_s)
		s = ''
		s += "#{export_time_s}, "
		s += "#{@stat_connections_processed}, "
		s += "#{@stat_connections_filtered}, "
		return(s)
	end
	def statistics_reset()
		stat_reset__()
		@stat_connections_processed = 0
		@stat_connections_filtered = 0
	end
	def statistics_get_from_ext()
		stats_a = stat_get__()
		@time_s = stats_a[0]
		@stat_connections_processed = stats_a[1]
		@stat_connections_filtered = stats_a[2]
	end

	def statistics_export()

		statistics_get_from_ext()

		# init ?
		if @stat_export_next_s == 0
				@stat_export_next_s = (@time_s/@stat_export_interval_s)*@stat_export_interval_s
			return
		end

		while @stat_export_next_s <= @time_s
			@stat_f.puts statistics_data_to_csv(@stat_export_next_s)
			@stat_f.flush()
			statistics_reset()
			@stat_export_next_s += @stat_export_interval_s
		end

		set_stat_next_export_s__(@stat_export_next_s)
	end

################################################################################
### filter #####################################################################
################################################################################
	def filter(cons)
		filter__(cons)
	end
################################################################################
### debug  #####################################################################
################################################################################
	def to_s
		s =  "FilterIPv4 (#{Version}) \n"
		s += "Processed: #{@stat_connections_processed} cons\n"
		s += "Filtered: #{@stat_connections_filtered} cons\n"
		s += "Filtered: #{@stat_connections_filtered.to_f/@stat_connections_processed*100} %\n"
	end
end


