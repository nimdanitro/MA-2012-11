#------------------------------------------------------------------------------#
# The Filter Border marks all non border flows as invalid 
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class FilterBorder

# Defined in C++
#	TypeUnknown = 0
#	TypeInternet = 1
#	TypeCustomer = 2
#	TypeBackbone = 3

	CGF_Columns = 3

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
### configuration ##############################################################
################################################################################
	def load_border_interfaces(file_p)

		File.open(file_p).each do |line|

			# filter out comments
			if line.size() < 4 or line =~ /^#/
				next
			end

			# parse the data
			data = line.chomp!.split(', ')
			if data.size != CGF_Columns
				throw " Format Error in '#{file_p}' on line #{$.}"
			end

			# prepare data
			router = data[0].to_i
			interface = data[1].to_i
			type = data[2].to_i
			key = (router << 16) + interface
			add_interface__(router, interface, type)
		end
	end
################################################################################
### filter #####################################################################
################################################################################
	def filter(cons)
		filter__ cons
	end
################################################################################
### debug  #####################################################################
################################################################################
	def to_s
		s =  "FilterBorder (#{Version}) \n"
		s += "Processed: #{@stat_connections_processed} cons\n"
		s += "Filtered: #{@stat_connections_filtered} cons\n"
		s += "Filtered: #{@stat_connections_filtered.to_f/@stat_connections_processed*100} %\n"
	end
end


