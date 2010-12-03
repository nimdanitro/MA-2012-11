#------------------------------------------------------------------------------#
# DataParser: Parse the data stream into connections data strctures 
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class DataParser

	ConPerCons = 100000

	def initialize(result_p, stat_interval_s = 300)

		# result path
		@result_p = result_p
		FileUtils::mkdir_p(@result_p)

		# time
		@time_s = 0 # time approximated based on the start_s of the data

		# statistics
		statistics_reset()
		@stat_f = File.open("#{@result_p}/stat.csv", 'w')
		@stat_f.puts statistics_head_to_csv()

		@stat_export_interval_s = stat_interval_s;
		@stat_export_next_s = 0;

		@connections = Connections.new(ConPerCons)

	end

################################################################################
### overview    ################################################################
################################################################################
	def to_s
		s =  "DataParser (#{DataParser::Version}) \n"
	end
################################################################################
### statistics  ################################################################
################################################################################
	def statistics_head_to_csv
		s = '# '
		s += 'time_s, '
		s += 'stat_connections_processed, '
		return(s)
	end
	def statistics_data_to_csv(export_time_s)
		s = ''
		s += "#{export_time_s}, "
		s += "#{@stat_connections_processed}, "
		return(s)
	end
	def statistics_reset()
		stat_reset__()
		@stat_connections_processed = 0
	end
	def statistics_get_from_ext()

		stats_a = stat_get__()

		@time_s = stats_a[0]
		@stat_connections_processed = stats_a[1]
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
### parse data   ###############################################################
################################################################################

	## parsing data to connection blocks
	def parse_files(files_a)
		files_a.each do |file_p|
			parse_file(file_p) do |cons|
				yield(cons)
			end
		end
	end
	def parse_file(file_p)
		begin
			parse_file__(file_p, @connections) do |cons|
				yield(cons)
			end
		rescue DataParserError
				puts "Error Parsing the data file #{file_p}"
		end
	end
	## parsing nfdump data to connection blocks
	def nf_parse_files(files_a)
		files_a.each do |file_p|
			nf_parse_file(file_p) do |cons|
				yield(cons)
			end
		end
	end
  
	def nf_parse_file(file_p)
		begin 
			nf_parse_file_csv__(file_p, @connections) do |cons|
				yield(cons)
			end
		rescue DataParserError
				puts "Error Parsing the Nfdump data file #{file_p}"
		end
	end
	
end

