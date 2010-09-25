#------------------------------------------------------------------------------#
# ProcessMonitor: Monitor the process 
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

class ProcessMonitor
	def initialize(path)
		@real_time_last_s = Time.now

		# result path
		@result_p = path
		FileUtils::mkdir_p(@result_p)

		# statfile
		@stat_f = File.open("#{@result_p}/stat.csv",'w')
		@stat_f.print "packet_time_s, packet_time_mod_s, "
		@stat_f.print "memory_mb, "
		@stat_f.print "period_delta_s, "
		@stat_f.print "data_parser_delta_s, "
		@stat_f.print "filter_delta_s, "
		@stat_f.print "connection_matrix_delta_s, "
		@stat_f.print "analyser_delta_s, "
		@stat_f.print "\n"
		@stat_f.flush
	end

	def period_start
			@period_start_s = Time.now
	end
	def period_stop(packet_time_s)
			@period_stop_s = Time.now
			@period_delta_s = @period_stop_s  - @period_start_s
			dump_stats(packet_time_s)
	end

	def data_parser_start
			@data_parser_start_s = Time.now
	end
	def data_parser_stop
			@data_parser_stop_s = Time.now
			@data_parser_delta_s += @data_parser_stop_s  - @data_parser_start_s
	end
	def filter_start
			@filter_start_s = Time.now
	end
	def filter_stop
			@filter_stop_s = Time.now
			@filter_delta_s += @filter_stop_s  - @filter_start_s
	end

	def connection_matrix_start
			@connection_matrix_start_s = Time.now
	end
	def connection_matrix_stop
			@connection_matrix_stop_s = Time.now
			@connection_matrix_delta_s += @connection_matrix_stop_s  - @connection_matrix_start_s
	end

	def analyser_start
			@analyser_start_s = Time.now
	end
	def analyser_stop
			@analyser_stop_s = Time.now
			@analyser_delta_s += @analyser_stop_s  - @analyser_start_s
	end
	def get_memory
			@memory_mb = `ps -o rss= -p #{Process.pid}`.to_i/1024
	end
	def reset
		@memory_mb = 0
		@period_delta_s = 0
		@data_parser_delta_s = 0
		@filter_delta_s = 0
		@connection_matrix_delta_s = 0
		@analyser_delta_s = 0
	end
	def	dump_stats(packet_time_s)
		get_memory
		@stat_f.print "#{packet_time_s}, #{packet_time_s%(24*3600)}, "
		@stat_f.print "#{@memory_mb}, "
		@stat_f.print "#{@period_delta_s}, "
		@stat_f.print "#{@data_parser_delta_s}, "
		@stat_f.print "#{@filter_delta_s}, "
		@stat_f.print "#{@connection_matrix_delta_s}, "
		@stat_f.print "#{@analyser_delta_s}, "
		@stat_f.print "\n"
		@stat_f.flush
		reset()
	end
end


