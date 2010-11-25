#------------------------------------------------------------------------------#
# NFDataParser: Parse the nfdump data into connections data strctures 
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#


require_relative 'helper.rb'
class NFDataParser

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
		s =  "NFDataParser (#{DataParser::Version}) \n"
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
	# fmt:%pr,%sa,%sp,%da,%dp,%nh
	def parse_files(input_files_b)
	  input_files_b.each do |file_p|
      f = %x(nfdump -r  #{file_p} -o "fmt:%pr,%sa,%sp,%da,%dp,%nh")
      a = f.split(/\n/,2).last
      a = a.split("Summary",2).first
      parse(a)
    end 
  end
	
	def parse(output)
      output.each_line do |line|
        nfdata = line.split(',')
        proto = nfdata[0].tr_s('"', '').strip
        srcaddr = nfdata[1].tr_s('"', '').strip
        puts "PROTO: #{proto} and #{srcaddr}"
      end
      
		end
	end
end

