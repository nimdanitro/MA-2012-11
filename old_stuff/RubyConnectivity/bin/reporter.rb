#!/usr/bin/ruby1.9
#require_relative '../lib/connectivity/helper.rb'

require 'fileutils'
require 'ipaddr'
require 'logger'
require 'time'

Log = Logger.new(STDOUT)
Log.level = Logger::DEBUG

###########################
# START REPORT
###########################
def start_report(folder_report,version)
  # CREATE FOLDER FOR REPORTS
  report_folder_dir = folder_report + "/reports"
	FileUtils::mkdir_p report_folder_dir
	
	# Define the prefix folders!
	prefix_bgp_folder = folder_report + "/prefix_bgp"
	prefix_net_folder = folder_report + "/prefix_net"
	prefix_host_folder = folder_report + "/prefix_host"
	
	
  prefix_bgp_files = Dir["#{prefix_bgp_folder}/all/*/[0-9]*.csv"].sort
	prefix_bgp_files.each do |file|  	  
    report_file(file,report_folder_dir,prefix_net_folder,prefix_host_folder,version)
  end
end

###########################
# START REPORT AT EPOCH
###########################
def start_report_at_epoch(folder_report,epoch,version)
  # CREATE FOLDER FOR REPORTS
  report_folder_dir = folder_report + "/reports"
	FileUtils::mkdir_p report_folder_dir
	
	# Define the prefix folders!
	prefix_bgp_folder = folder_report + "/prefix_bgp"
	prefix_net_folder = folder_report + "/prefix_net"
	prefix_host_folder = folder_report + "/prefix_host"
	
	prefix_bgp_files = Dir["#{prefix_bgp_folder}/all/*/#{epoch}.csv"].sort
	prefix_bgp_files.each do |file|  
    report_file(file,report_folder_dir,prefix_net_folder,prefix_host_folder,version)
  end
end

###########################
# REPORT FILE
###########################
def report_file(file,report_folder_dir,prefix_net_folder,prefix_host_folder,version)
  # HASH FOR DATA OF LINES...
  hash_data = Hash.new
  # EXTRACTING THE TIME.. AS UNIX EPOCH
  epoch = File.basename(file).split('.')[-2].to_i	  

  File.open(file).each_line do |line|
    # SKIP TO NEXT IF CORRUPT LINE OR COMMENT...
		next if line =~ /#/ or line.size < 5
		data = line.split(', ')


		type = data[0].to_i
		if type == 0
			prefix =  data[1]
			unbalanced = data[3].to_i

			if hash_data[prefix] == nil
			  hash_data[prefix] = unbalanced
			else
			  # SHOULD NOT BE THE CASE THAT A PREFIX HAS MORE THAN ONE ENTRY BUT IN CASE OF SEVERAL ENTRIES JUST ADD THE SEVERITY...
				hash_data[prefix] += unbalanced
			end	  			
		end	
	end
  # SORT HASH BY VALUES...
  data_sorted = hash_data.sort {|a,b| a[1]<=>b[1]}	  
  # GET TOP 5.. but only if more than 1 internal host is affected!
  # top_five will hold the prefixes with the highest severity.. but higher severity than 1!
  top_five = Array.new
  t5_prefixes = Array.new
  # calculate the total severity sum for this epoch..
  problem_sum = 0
  i = 0
  data_sorted.reverse_each do |prefix, severity|
    # if severity is 1 or even zero skip to next!
    next if severity == 1 or severity == 0

	  if i < 5 and severity > 1
	    #whois = %x[whois #{prefix} | egrep "(netname)|(NetName)" | awk '{print $2}']
	    top_five[i] = [prefix, severity]
    end

    if severity > 1
      problem_sum = problem_sum+1
    end

    i=i+1
	end

	classification = get_classification(epoch,problem_sum,top_five,version)
	date = Time.at(epoch) 
	# OPEN REPORT FILE
	report_out = File.open(report_folder_dir +"/#{epoch}.txt", 'w')
	report_out.puts "
FACT REPORT
-----------

EPOCH: #{epoch}
DATE: #{date}
:::::::::::::::::::::::::::::::::::::::		
CONNECTIVITY PROBLEM: #{classification}
:::::::::::::::::::::::::::::::::::::::

TOP PROBLEMS:
" 
  i=1
  top_five.each do |prefix|
    report_out.puts "#{i}: PREFIX: #{prefix[0]}, SEVERITY: #{prefix[1]}"
    i=i+1
  end
  
  report_out.puts "
  
PROBLEM STRUCTURE:"
  i=1
  top_five.each do |prefix|
    report_out.puts "#{i}: PREFIX: #{prefix[0]}, SEVERITY: #{prefix[1]}"
    nets = get_net_by_prefix(prefix, prefix_net_folder + "/all/*/#{epoch}.csv")
    j=1
    nets.each do |net|
      hosts = get_hosts_by_net(net,prefix_host_folder + "/all/*/#{epoch}.csv")  
      report_out.puts "    #{i}.#{j} NET: #{net[0]}, SEVERITY: #{net[1]}"
      hosts.each do |host|
        report_out.puts "       |- HOST: #{host[0]}, SEVERITY: #{host[1]}"
      end
      j=j+1
    end
    i=i+1
    report_out.puts ""
  end
	report_out.close
end

###########################
# CLASSIFICATION
###########################
def get_classification(epoch,problem_sum,top_five,version)
  hour = Time.at(epoch).hour.to_i
  threshold = 10
  # apply a simple day/night model (8to8)... at night there must be a smaller threshold!
  if hour > 8 and hour <=20
    #DAY::::
    threshold = 40
  end
  
  score = 0
  # creating the severity sum of the top five
  topsum = 0
  top_five.each do |elem|
    topsum = topsum + elem[1]
  end
  
  top_number = top_five.length
  if top_number == 5
    score = score + 10
  elsif top_number > 1
    score = score + 2*top_number
  end
  
  if problem_sum >= 2*topsum
    # there are a lot of problems...
    score = score + 10
  end
  
  if topsum >= threshold
    score = score + 10
  end
  # IPv6 BONUS SCORE
  bonus = 1
  score = score + (version-4)*bonus
  
  if score >= 20
    classification = "VERY LIKELY"
  elsif score >= 13
    classification = "LIKELY"
  else
    classification = "UNLIKELY"
  end
  return classification
end

############################################
# GET TOP FIVE NETS (/24) OF A PREFIX
############################################
def get_net_by_prefix(prefix,net_file)
  net = Array.new
  if prefix[0].split('/')[-1].to_i == 24
    net.push([prefix[0],prefix[1]])
    return net
  end
  hash_data = Hash.new
  
  files = Dir[net_file].sort
	files.each do |file|
    ipprefix = IPAddr.new(prefix[0])
    File.open(file).each_line do |line|
      # SKIP TO NEXT IF CORRUPT LINE OR COMMENT...
  		next if line =~ /#/ or line.size < 5
  		data = line.split(', ')
  		tmp_net = IPAddr.new(data[1])
  		if ipprefix.include?(tmp_net)
  		  hash_data[data[1]] = data[3].to_i
  	  end
    end
  end
  
  i=0
  data_sorted = hash_data.sort {|a,b| -1*(a[1]<=>b[1])}	  
  data_sorted.each do |prefix, severity|
    if i < 5
	    net.push([prefix, severity])
    end
    i=i+1
  end
  return net
end

###########################
# GET HOSTS BY NET/24
###########################
def get_hosts_by_net(net,hosts_file)
  
  hosts = Array.new
  hash_data = Hash.new
  ipnet = IPAddr.new(net[0].to_s)
  
  files = Dir[hosts_file].sort
	files.each do |file|
    File.open(file).each_line do |line|
      # SKIP TO NEXT IF CORRUPT LINE OR COMMENT...
  		next if line =~ /#/ or line.size < 5
  		data = line.split(', ')
  		tmp_host = IPAddr.new(data[1])
  		if ipnet.include?(tmp_host)
  		  hash_data[data[1]] = data[3].to_i
  	  end
    end
  end
  
  i=0
  data_sorted = hash_data.sort {|a,b| -1*(a[1]<=>b[1])}	  
  data_sorted.each do |host, severity|
    if i < 5
	    hosts.push([host, severity])
    end
    i=i+1
  end
  
  return hosts
end

###########################
# PROCESS
###########################
def process(data_folder_p,arg)
	Log.debug("Process data from anaylser folder #{data_folder_p}")
	
	if arg == 0 or arg == "-4"
  	# REPORT IPv4 FOLDER
  	folder_report = data_folder_p + "/IPv4"
  	Log.debug("Process data from anaylser folder #{folder_report}")
  	start_report(folder_report,4)
	end
	if arg == 0 or arg == "-6"
  	# REPORT IPv6 FOLDER
  	folder_report = data_folder_p + "/IPv6"
  	Log.debug("Process data from anaylser folder #{folder_report}")
  	start_report(folder_report,6)
	end
end

###########################
# PROCESS EPOCH
###########################
def process_epoch(data_folder_p,epoch)
	Log.debug("Process data from anaylser folder #{data_folder_p} for epoch #{epoch}")
	
	# REPORT IPv4 FOLDER
	folder_report = data_folder_p + "/IPv4"
	Log.debug("Process data from anaylser folder #{folder_report} for epoch #{epoch}")
	start_report_at_epoch(folder_report,epoch,4)
	
	# REPORT IPv6 FOLDER
	folder_report = data_folder_p + "/IPv6"
	Log.debug("Process data from anaylser folder #{folder_report} for epoch #{epoch}")
	start_report_at_epoch(folder_report,epoch,6)
  
end

##################################################################
# MAIN: START WITH PATH TO ANALYSER DIRECTORY!!                  #
##################################################################
if (ARGV[0] == nil or	File.directory?(ARGV[0]) == false or File.directory?(ARGV[0]+"/IPv4") == false)
	puts "Usage <connectivity analyser folder> for reporting the entire analyser forlder"
	puts "Usage <connectivity analyser folder> <epoch> for reporting only one time slot at epoch!"
	exit
end

if (ARGV[1] == nil)
  process(ARGV[0],0)
elsif ARGV[1] == "-4" or ARGV[1]== "-6"
  process(ARGV[0],ARGV[1])
else
  process_epoch(ARGV[0],ARGV[1])
end