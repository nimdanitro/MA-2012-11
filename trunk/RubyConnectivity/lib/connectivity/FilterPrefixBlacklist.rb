#------------------------------------------------------------------------------#
# The Filter Prefix Blacklist marks specific IP/IP Ranges as invalid 
#------------------------------------------------------------------------------#
# Copyright (c) 2010, Dominik Schatzmann
# Author: Dominik Schatzmann  (mail@dominikschatzmann.ch)
# All rights reserved
#------------------------------------------------------------------------------#

require_relative 'helper.rb'
class FilterPrefixBlacklist
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
	def load_prefix_folder(folder_p)
		prefixes_a = Array.new

		files_a = Dir["#{folder_p}/bl_*.txt"].sort
		files_a.each do |file_p|
			puts "Load ... #{file_p}"
			load_prefixes_file(file_p, prefixes_a)
		end
		0.upto(32) do |i|
			next if prefixes_a[i] == nil
			puts "ADD #{prefixes_a[i].size} X.X.X.X/#{i} prefixes"
			# we care only about border interfaces
			prefixes_a[i].each do |prefix|
				add_prefix__(prefix, 1)
			end
		end
	end
	def load_prefixes_file(file_p, prefixes_a)
		File.open(file_p).each do |line|

			# filter out comments
			if line.size() < 4 or line =~ /^#/
				next
			end

			# parse the data
			data = line.chomp!.split(',')
			if data.size < 2
				throw " Format Error in '#{file_p}' on line #{$.} :: '#{line}'"
			end

			# prepare data
			prefix = data[0]
			reason = data[1].strip
			prefix_length = prefix.split('/')[1].to_i

			prefixes_a[prefix_length] = [] if prefixes_a[prefix_length] == nil
			prefixes_a[prefix_length] << prefix
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
		s =  "FilterPrefixBlacklist (#{Version}) \n"
		s += "Processed: #{@stat_connections_processed} cons\n"
		s += "Filtered: #{@stat_connections_filtered} cons\n"
		s += "Filtered: #{@stat_connections_filtered.to_f/@stat_connections_processed*100} %\n"
	end
end


