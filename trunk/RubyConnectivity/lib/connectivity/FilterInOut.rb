#------------------------------------------------------------------------------#
# The Filter In Out marks all non in-out or out-in flows as invalid 
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class FilterInOut

	CGF_Columns = 3
	CGF_Type_Border = 1

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
		s += 'in_in, '
		s += 'in_out, '
		s += 'out_in, '
		s += 'out_out, '
		return(s)
	end
	def statistics_data_to_csv(export_time_s)
		s = ''
		s += "#{export_time_s}, "
		s += "#{@stat_connections_processed}, "
		s += "#{@stat_connections_filtered}, "
		s += "#{@stat_connections_in_in}, "
		s += "#{@stat_connections_in_out}, "
		s += "#{@stat_connections_out_in}, "
		s += "#{@stat_connections_out_out}, "
		return(s)
	end
	def statistics_reset()
		stat_reset__()
		@stat_connections_processed = 0
		@stat_connections_filtered = 0
		@stat_connections_in_in = 0
		@stat_connections_in_out = 0
		@stat_connections_out_in = 0
		@stat_connections_out_out = 0
	end
	def statistics_get_from_ext()
		stats_a = stat_get__()
		@time_s = stats_a[0]
		@stat_connections_processed = stats_a[1]
		@stat_connections_filtered = stats_a[2]
		@stat_connections_in_in = stats_a[3]
		@stat_connections_in_out = stats_a[4]
		@stat_connections_out_in = stats_a[5]
		@stat_connections_out_out = stats_a[6]
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
	def load_internal_prefixes(file_p)
		prefixes_a = Array.new
		prefixes_v6_a = Array.new
		File.open(file_p).each do |line|

			# filter out comments
			if line.size() < 3 or line =~ /^#/
				next
			end

			# parse the data
			# IPv4 Prefix
			if line.include? '.'
			  data = line.chomp!.split('/')
			  if data.size != CGF_Columns 
  				throw " Format Error in '#{file_p}' on line #{$.} :: '#{line}'"
    		end
    		# prepare data
  			addr = data[0]
  			addr_length = data[1].to_i
  			as = data[2].to_i
  			
    		prefixes_a[addr_length] = [] if prefixes_a[addr_length] == nil
  			prefixes_a[addr_length] << data
    		
			end
			# IPv6 Prefix
			if line.include? ':'
			  data6 = line.chomp!.split('/')
			  if data6.size != CGF_Columns
  				throw " Format Error in '#{file_p}' on line #{$.} :: '#{line}'"
  			end
  			# prepare data
        addr = data6[0]
  			addr_length = data6[1].to_i
  			as = data6[2].to_i
  			
  			prefixes_v6_a[addr_length] = [] if prefixes_v6_a[addr_length] == nil
  			prefixes_v6_a[addr_length] << data6
  			puts "JUHU"
		  end
      
      #DEBUG
			puts "PREFIX #{addr}/#{addr_length} AS #{as}"

			# we care only about border interfaces
			# add_prefix__("#{addr}/#{addr_length}", as)
		end
		0.upto(32) do |i|
			next if prefixes_a[i] == nil
			  puts "ADD #{prefixes_a[i].size} X.X.X.X/#{i} prefixes"
			  # we care only about border interfaces
			prefixes_a[i].each do |data|
				addr = data[0]
				addr_length = data[1].to_i
				as = data[2].to_i
				#puts "#{addr}/#{addr_length}, #{as}"
				add_prefix__("#{addr}/#{addr_length}", as)
			end
		end
		0.upto(128) do |i|
			next if prefixes_v6_a[i] == nil
			  puts "ADD #{prefixes_v6_a[i].size} X:X::X/#{i} prefixes"
			# we care only about border interfaces
			prefixes_v6_a[i].each do |data6|
				addr = data6[0]
				addr_length = data6[1].to_i
				as = data6[2].to_i
				#puts "#{addr}/#{addr_length}, #{as}"
				add_prefix__("#{addr}/#{addr_length}", as)
			end
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
		s =  "FilterInOut (#{Version}) \n"
		s += "Processed: #{@stat_connections_processed} cons\n"
		s += "Filtered: #{@stat_connections_filtered} cons\n"
		s += "Filtered: #{@stat_connections_filtered.to_f/@stat_connections_processed*100} %\n"
	end
end


