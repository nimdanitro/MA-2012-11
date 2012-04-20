#!/usr/bin/ruby1.9

require 'fileutils'
require 'ipaddr'
require 'logger'

################################################################################
## extract mapping
################################################################################
# create the in-out mapping
# mapping_h[time_s] = "<used data of folder_p>"
def get_mapping(folder_a, file_base_p, lines_to_skip, other_file_base_p = nil)
	mapping_h = Hash.new
	stat_h =  Hash.new

	## merge
	folder_a.each_index do |i|
		folder = folder_a[i]
		file_p = nil
		if File.exists?("#{folder}/#{file_base_p}") == true
			file_p = "#{folder}/#{file_base_p}"
		elsif File.exists?("#{folder}/#{other_file_base_p}") == true
			file_p = "#{folder}/#{other_file_base_p}"
		else
			raise "No Mapping File found in #{folder}"
		end
		
		file = File.open(file_p)
		file.each_line do |line|

			if i != 0 and (line =~ /#/ or $. <= lines_to_skip )
				# comment line
			else

				data_a = line.chomp.split(", ")
				time_s = data_a[0].to_i
				count = data_a[1].to_i

				#if stat_h[time_s] == nil
				#	stat_h[time_s] = count
					mapping_h[time_s] = folder
				#elsif stat_h[time_s] < count
				#	stat_h[time_s] = count
				#	mapping_h[time_s] = folder
				#end

			end
		end # file
	end # folders
	return(mapping_h)
end

################################################################################
## merge a file
################################################################################
def merge_file(
	mapping_h, 
	folder_in_a, 
	folder_out, 
	folder_base_p, 
	file_base_p, 
	time_s_col,
	columns,
	skip_data_points = false
)

	# check if all files exits and if the files have the right size
	folder_in_a.each do |folder|
		file_p = "#{folder}/#{folder_base_p}/#{file_base_p}"
		if File.exists?(file_p) == false
			Log.warn("Missing data file: '#{file_p}' skip join")
			return;
		else
			File.open(file_p).each_line do |line|
				data_a = line.chomp.split(', ')
				if data_a.size != columns and !skip_data_points
					Log.error("Corrupted data in file (#{file_p}) on line: #{$.} :: '#{line}' abort join")
					exit 
				elsif data_a.size != columns
					Log.error("Corrupted data in file (#{file_p}) on line: #{$.} :: '#{line}'")
				end
			end
		end
	end

	## merge data
	FileUtils::mkdir_p("#{folder_out}/#{folder_base_p}")
	out_f = File.open("#{folder_out}/#{folder_base_p}/#{file_base_p}", 'w')
	merged_data_h = Hash.new
	folder_in_a.each do |folder|
		file_p = "#{folder}/#{folder_base_p}/#{file_base_p}"
		file = File.open(file_p)
		file.each_line do |line|
			if line =~ /#/ or line =~ /time/
			else
				data_a = line.chomp.split(", ")
				time_s = data_a[time_s_col].to_i
				data_a.size != columns

				next if data_a.size != columns

				if mapping_h[time_s] == nil
					raise "No mapping info for '#{file_p}' of line (#{$.}) '#{line}' "
				elsif folder == mapping_h[time_s]
						out_f.puts "#{line.chomp} #{time_s}, #{mapping_h[time_s]},"
				else

				end

			end
		end
	end
	out_f.close
end
################################################################################
## merge files
################################################################################
def merge_files(
	mapping_h, 
	folder_in_a, 
	folder_out, 
	file_base_p, 
	time_s_col,
	columns
)
	Dir["#{folder_in_a[0]}/#{file_base_p}/*"].sort.each do |file_p|
		tag = file_p.split('/')[-1]
		merge_file(mapping_h, folder_in_a, folder_out, file_base_p, tag , time_s_col, columns)
	end
end
################################################################################
## merge folders
################################################################################
def merge_folders(
	mapping_h, 
	folder_in_a, 
	folder_out,
	folder_in_base_p
)

	# get all files
	folder_in_a.each do |folder|
		Dir["#{folder}/#{folder_in_base_p}/*/*"].each do |file_p|
			data_a = file_p.split('/')
			folder_current = data_a[-2]
			file_current = data_a[-1]
			if folder_current =~ /\d+/ and file_current =~ /\d+/
				time_s = data_a[-1].split('.')[0].to_i
				if mapping_h[time_s] == nil
					raise "No mapping info for '#{file_p}' of line (#{$.}) '#{line}' "
				elsif folder == mapping_h[time_s]
					if File.directory?("#{folder_out}/#{folder_in_base_p}/#{folder_current}/") == false
						FileUtils::mkdir_p "#{folder_out}/#{folder_in_base_p}/#{folder_current}"
					end
					FileUtils::copy(file_p, "#{folder_out}/#{folder_in_base_p}/#{folder_current}/#{file_current}")   
				else
					# skip
				end
			else
				puts "Skip '#{file_p}'"
			end
		end
	end
end
def merge_folders_folders(
	mapping_h, 
	folder_in_a, 
	folder_out,
	folder_in_base_p
)
	Dir["#{folder_in_a[0]}/#{folder_in_base_p}/*"].sort.each do |folder_p|
		tag = folder_p.split('/')[-1]
		merge_folders(mapping_h, folder_in_a, folder_out, "#{folder_in_base_p}/#{tag}")
	end
end

################################################################################
## MAIN
################################################################################

Log = Logger.new(STDOUT)
Log.level = Logger::DEBUG

if ARGV[0] == nil
	puts " -- join the different data sets -- "
	puts "Usage <analyser folder>"
	exit
end

# input
input_folder = ARGV[0]

# output
output_folder = input_folder + "/merged"
FileUtils::mkdir_p(output_folder)

folder_a = []
Dir["#{input_folder}/*"].sort!.each do |folder|
	if folder.split('/')[-1] =~ /\d+/
		folder_a << folder if File.directory? folder
	else
		puts "Skip '#{folder}'"
	end
end


mapping_h = get_mapping(folder_a, "data_parser/stat.csv", 8)

################################################################################
## process monitor
################################################################################
merge_file(mapping_h, folder_a, output_folder,"process_monitor", "stat.csv", 0, 8)

################################################################################
## data parser
################################################################################
merge_file(mapping_h, folder_a, output_folder,"data_parser", "stat.csv", 0, 2)

################################################################################
## filter bank
################################################################################
# ipv4
merge_file(mapping_h, folder_a, output_folder,"filter_ipv4", "stat.csv", 0, 3)
# ipv6
merge_file(mapping_h, folder_a, output_folder,"filter_ipv6", "stat.csv", 0, 3)
# border
merge_file(mapping_h, folder_a, output_folder,"filter_border", "stat.csv", 0, 3)
# in out
merge_file(mapping_h, folder_a, output_folder,"filter_in_out", "stat.csv", 0, 7)
# blacklist
merge_file(mapping_h, folder_a, output_folder,"filter_prefix_blacklist", "stat.csv", 0, 3)

################################################################################
## connection matrix
################################################################################
merge_file(mapping_h, folder_a, output_folder, "connection_matrix", "stat.csv", 0, 20)

################################################################################
## analyser
################################################################################
merge_file(mapping_h, folder_a, output_folder, "analyser", "stat.csv", 0, 38)
#IPv4
merge_folders(mapping_h, folder_a, output_folder, "analyser/IPv4/weird_connections")
merge_files(mapping_h, folder_a, output_folder, "analyser/IPv4/host_host", 0, 3)
merge_files(mapping_h, folder_a, output_folder, "analyser/IPv4/host", 0, 5)
merge_folders_folders(mapping_h, folder_a, output_folder, "analyser/IPv4/prefix_host")
merge_files(mapping_h, folder_a, output_folder, "analyser/IPv4/net", 0, 4)
merge_folders_folders(mapping_h, folder_a, output_folder, "analyser/IPv4/prefix_net")
merge_files(mapping_h, folder_a, output_folder, "analyser/IPv4/bgp", 0, 4)
merge_folders_folders(mapping_h, folder_a, output_folder, "analyser/IPv4/prefix_bgp")
#IPv6
merge_folders(mapping_h, folder_a, output_folder, "analyser/IPv6/weird_connections")
merge_files(mapping_h, folder_a, output_folder, "analyser/IPv6/host_host", 0, 3)
merge_files(mapping_h, folder_a, output_folder, "analyser/IPv6/host", 0, 5)
merge_folders_folders(mapping_h, folder_a, output_folder, "analyser/IPv6/prefix_host")
merge_files(mapping_h, folder_a, output_folder, "analyser/IPv6/net", 0, 4)
merge_folders_folders(mapping_h, folder_a, output_folder, "analyser/IPv6/prefix_net")
merge_files(mapping_h, folder_a, output_folder, "analyser/IPv6/bgp", 0, 4)
merge_folders_folders(mapping_h, folder_a, output_folder, "analyser/IPv6/prefix_bgp")


################################################################################
## analyser application mix
################################################################################
merge_folders(mapping_h, folder_a, output_folder, "analyser_application_mix/stat")
merge_folders(mapping_h, folder_a, output_folder, "analyser_application_mix/protocols")
merge_folders(mapping_h, folder_a, output_folder, "analyser_application_mix/tcp_flows_in")
merge_folders(mapping_h, folder_a, output_folder, "analyser_application_mix/tcp_flows_out")
merge_folders(mapping_h, folder_a, output_folder, "analyser_application_mix/udp_flows_in")
merge_folders(mapping_h, folder_a, output_folder, "analyser_application_mix/udp_flows_out")


