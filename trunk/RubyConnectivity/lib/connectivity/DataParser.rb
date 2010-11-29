#------------------------------------------------------------------------------#
# DataParser: Parse the data stream into connections data strctures 
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

require 'time'
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
    # OLD!!!
    # files_a.each do |file_p|
    #   # ON CLUSTER IS NOT THE PROPER VERSION OF NFDUMP!!
    #   #f = %x(nfdump -r  #{file_p} -o csv)
    #       #so use self-compiled local nfdump!
    #       f = %x(/home/asdaniel/nfdump/bin/nfdump -r  #{file_p} -o csv -6)
    #       a = f.split(/\n/,2).last
    #       a = a.split("Summary",2).first
    #       nf_parse(a)
    #     end 
  end
  
  def nf_parse_file(file_p)
    begin 
      nf_parse_file_csv__(file_p, @connections) do |cons|
          yield(cons)
      end
    rescue
        puts "Error Parsing the Nfdump data file #{file_p}"
    end
  end
	
	# def nf_parse(output)
	#       output.each_line do |line|
	#         nfdata = line.split(',')
	#         protocol = nfdata[7].tr_s('"', '').strip
	#         src_addr = nfdata[3].tr_s('"', '').strip
	#         src_port = nfdata[5].tr_s('"', '').strip
	#         dst_addr = nfdata[4].tr_s('"', '').strip
	#         dst_port = nfdata[6].tr_s('"', '').strip
	#         #puts "SRC: #{src_addr} / #{src_port} --> DEST: #{dst_addr} / #{dst_port} with PROTO #{protocol}"
	#         nh = nfdata[23].tr_s('"', '').strip
	#         #puts "NH: #{nh}"
	#  
	#         in_interface = nfdata[15].tr_s('"', '').strip
	#         out_interface = nfdata[16].tr_s('"', '').strip
	#         #puts "INTERFACES: IN #{in_interface} / OUT #{out_interface}"
	# 
	#         packets = nfdata[11].tr_s('"', '').strip.to_i + nfdata[13].tr_s('"', '').strip.to_i
	#         #puts "PACKETS: #{packets}"
	# 
	#         bytes = nfdata[12].tr_s('"', '').strip.to_i + nfdata[14].tr_s('"', '').strip.to_i
	#         #puts "BYTES: #{bytes}"
	# 
	#         start_time = nfdata[0].tr_s('"', '').strip
	#         time_s = Time.parse(start_time).to_i
	#         #puts "TIME_S: #{time_s}"
	#         
	#         end_time = nfdata[1].tr_s('"', '').strip
	#         time_e = Time.parse(end_time).to_i
	#         
	#         begin
	#        nf_parse__(protocol, src_addr, src_port, dst_addr, dst_port, time_s, time_e, nh , in_interface, out_interface, packets, bytes) do |cons|
	#          yield(cons)
	#        end
	#      rescue DataParserError
	#        puts "Error Parsing the nfdump data line #{line}"
	#      end
	#       
	#       end
	#       
	#   end
	
end

