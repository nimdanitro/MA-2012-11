#------------------------------------------------------------------------------#
# The Connection Matrix keeps the connection of a given time period
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class ConnectionMatrix

	Version = "00.00.01"
	VersionNbr = 000001

	def initialize(
		cm_output,
		cm_periodic_s,
		cm_cache_duration_s,
		cm_cache_max, 
		cm_cache_prune_interval_s,
		cm_analysis_delay_s,
		cm_analysis_active_duration_s
		
	)

		# result path
		@result_p = cm_output
		FileUtils::mkdir_p(@result_p)

		# flow cache
		@connection_cache_h = Hash.new
		@connection_cache_duration_s = cm_cache_duration_s
		@connection_cache_max = cm_cache_max
		@connection_cache_prune_interval_s = cm_cache_prune_interval_s # multiple of a periodic_s

		# time
		@time_s = 0 # time approximated based on the start_s of the data

		# periodic jobs 
		@periodic_job_interval_s = cm_periodic_s
		@periodic_job_next_s = 0

		# analysis
		@analysis_delay_s = cm_analysis_delay_s
		@analysis_active_duration_s = cm_analysis_active_duration_s

		# statistics
		@stat_f = File.open("#{@result_p}/stat.csv", 'w')
		@stat_f.puts statistics_head_to_csv()
		statistics_reset()
	end

################################################################################
### add         ################################################################
################################################################################
	def add_connections(cons)
		# this is handled by the c++ part
		add_connections__(cons) do |periodic_job_s|
				periodic_job()
				yield(periodic_job_s)
		end
	end
################################################################################
### statistics  ################################################################
################################################################################
	def statistics_head_to_csv
		s = '# '
		s += 'time_s, '
		s += 'connections_processed, '
		s += 'cache_size, '
		17.times do |i|
				s += "type_#{i}, "
		end
		return(s)
	end
	def statistics_data_to_csv(export_time_s)
		s = ''
		s += "#{export_time_s}, "
		s += "#{@stat_connections_processed}, "
		s += "#{@stat_cache_size}, "
		17.times do |i|
				s += "#{@stat_state_a[i]}, "
		end
		return(s)
	end
	def statistics_reset()
		statistics_reset__()
		@stat_connections_processed = 0
		@stat_cache_size = 0
		@stat_state_a = Array.new
	end
	def statistics_get_from_ext()

		stats_a = statistics_get__()

		@time_s = stats_a[0]

		@stat_connections_processed = stats_a[1]
		@stat_cache_size = stats_a[2]
		17.times do |i|
				@stat_state_a[i] = stats_a[3 + i]
		end
	end

################################################################################
### periodic jobs  #############################################################
################################################################################
	def periodic_job()

		# we need the time ...
		statistics_get_from_ext()

		# init ?
		if @periodic_job_next_s == 0
			@periodic_job_next_s = (@time_s/@periodic_job_interval_s)*@periodic_job_interval_s
			return
		end

		while @periodic_job_next_s <= @time_s

			analysis()

			statistics_get_from_ext()

			# get time_s and write statistics
			@stat_f.puts statistics_data_to_csv(@periodic_job_next_s)
			@stat_f.flush()
			statistics_reset()

			# prune outdated connections
			if @periodic_job_next_s%@connection_cache_prune_interval_s == 0
				prune_cache
			end
			@periodic_job_next_s += @periodic_job_interval_s
		end

		set_periodic_job_next_s__(@periodic_job_next_s)
	end

	def prune_cache()
			prune__(@periodic_job_next_s - @connection_cache_duration_s)
	end

################################################################################
### analysis    ################################################################
################################################################################
	def analysis
			analysis__(
				@periodic_job_next_s - @analysis_delay_s - @analysis_active_duration_s,
				@periodic_job_next_s - @analysis_delay_s
			)
	end
################################################################################
### debug       ################################################################
################################################################################
	def to_s
		s =  "ConnectionMatrix (#{ConnectionMatrix::Version}) \n"
		s += "Connection Cache\n"
		s += sprintf(" -- size %f \n", @connection_cache_h.size)
		s += "Statistics \n"
		s += sprintf("  processed %4.1f %10u  \n", @stat_processed_connections.to_f*100/@stat_processed_connections, @stat_processed_connections)
	end
end


