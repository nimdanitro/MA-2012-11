#------------------------------------------------------------------------------#
# The detector parses the output files of the analyser and detects any 
# connectivity issues.
# This detector is a first approach: a simple threshold detector with a 
# 8-to-8 95%-quartile threshold!
#------------------------------------------------------------------------------#
if (
	ARGV[0] == nil or 
	file.directory?(ARGV[0]) == false 
	or File.directory?(ARGV[0]+"/IPv4") == false
)
	puts "Usage <detected analyser folder>"
	exit
end
process_analyser(ARGV[0])


def process_analyser(folder_p)
  
  
  
end

##########################
# THRESHOLD
##########################
def get_threshold(input,percentile)
  theshold = 0
  if percentile > 100
    return(0)
  end
  if input.size >=0
    input = input.sort
    pos = input.size*percentile/100
    pos1 = pos.floor
    pos2 = pos.ceil
    
    # linear interpolation..
    delta = input[pos2]-input[pos1]
    threshold = input[pos1] + delta*(pos-pos1)
  end
  return(threshold)
end
##########################
# Parse NETFILE!
##########################
def parse_netfile(net_folder_p)
  start_s = 0
  end_s = 0
  daily = Array.new
	nightly = Array.new
	iter = 0
	
	File.open(net_folder_p + "/all.csv").each_line do |line|
		data = line.split(', ')
		time_s = data[0].to_i
		if time_s > end_s
	    end_s = time_s
    end
    if iter == 0
      start_s = time_s
    end
    if start_s > time_s
      start_s = time_s
    end
    
		t = Time.at(time_s)
		
		balanced = data[1].to_i
		unbalanced = data[2].to_i
		
		if (t.hour >= 6 and t.hour< 21)
      daily.push(unbalanced)
    else
      nightly.push(unbalanced)
    end
    
  end
  dailythreshold = get_threshold(daily,95)
  nightlythreshold = get_threshold(nightly,95)
  threshold_p = net_folder_p + "/threshold.csv"
	threshold_f = File.open(threshold_p, 'w')
	threshold_f.puts "TIME_S, THRESHOLD VALUE"
	File.open(net_folder_p + "/all.csv").each_line do |line|
	  data = line.split(', ')
		time_s = data[0].to_i
		t = Time.at(time_s)
		if (t.hour >= 6 and t.hour< 21)
		  if data[2].to_i>dailythreshold
        threshold_f.puts "#{time_s}, #{dailythreshold}, 1"
      else
        threshold_f.puts "#{time_s}, #{dailythreshold}, 0"
      end
    else
		  if data[2].to_i>nightlythreshold
        threshold_f.puts "#{time_s}, #{nightlythreshold}, 1"
      else
        threshold_f.puts "#{time_s}, #{nightlythreshold}, 0"
      end
    end
  end
end

def parse_hostfile(host_folder_p)
  start_s = 0
  end_s = 0
  daily = Array.new
	nightly = Array.new
	iter = 0
	
	File.open(host_folder_p + "/all.csv").each_line do |line|
		data = line.split(', ')
		time_s = data[0].to_i
		if time_s > end_s
	    end_s = time_s
    end
    if iter == 0
      start_s = time_s
    end
    if start_s > time_s
      start_s = time_s
    end
    
		t = Time.at(time_s)
		
		balanced = data[1].to_i
		unbalanced = data[2].to_i
		
		if (t.hour >= 6 and t.hour< 21)
      daily.push(unbalanced)
    else
      nightly.push(unbalanced)
    end
    
  end
  dailythreshold = get_threshold(daily,95)
  nightlythreshold = get_threshold(nightly,95)
  threshold_p = host_folder_p + "/threshold.csv"
	threshold_f = File.open(threshold_p, 'w')
	threshold_f.puts "TIME_S, THRESHOLD VALUE"
	File.open(host_folder_p + "/all.csv").each_line do |line|
	  data = line.split(', ')
		time_s = data[0].to_i
		t = Time.at(time_s)
		if (t.hour >= 6 and t.hour< 21)
		  if data[2].to_i>dailythreshold
        threshold_f.puts "#{time_s}, #{dailythreshold}, 1"
      else
        threshold_f.puts "#{time_s}, #{dailythreshold}, 0"
      end
    else
		  if data[2].to_i>nightlythreshold
        threshold_f.puts "#{time_s}, #{nightlythreshold}, 1"
      else
        threshold_f.puts "#{time_s}, #{nightlythreshold}, 0"
      end
    end
  end
end

def parse_bgpfile(bgp_folder_p)
  start_s = 0
  end_s = 0
  daily = Array.new
	nightly = Array.new
	iter = 0
	
	File.open(bgp_folder_p + "/all.csv").each_line do |line|
		data = line.split(', ')
		time_s = data[0].to_i
		if time_s > end_s
	    end_s = time_s
    end
    if iter == 0
      start_s = time_s
    end
    if start_s > time_s
      start_s = time_s
    end
    
		t = Time.at(time_s)
		
		balanced = data[1].to_i
		unbalanced = data[2].to_i
		
		if (t.hour >= 6 and t.hour< 21)
      daily.push(unbalanced)
    else
      nightly.push(unbalanced)
    end
    
  end
  dailythreshold = get_threshold(daily,95)
  nightlythreshold = get_threshold(nightly,95)
  threshold_p = bgp_folder_p + "/threshold.csv"
	threshold_f = File.open(threshold_p, 'w')
	threshold_f.puts "TIME_S, THRESHOLD VALUE, EXCEEDED"
	File.open(bgp_folder_p + "/all.csv").each_line do |line|
	  data = line.split(', ')
		time_s = data[0].to_i
		t = Time.at(time_s)
		if (t.hour >= 6 and t.hour< 21)
		  if data[2].to_i>dailythreshold
        threshold_f.puts "#{time_s}, #{dailythreshold}, 1"
      else
        threshold_f.puts "#{time_s}, #{dailythreshold}, 0"
      end
    else
		  if data[2].to_i>nightlythreshold
        threshold_f.puts "#{time_s}, #{nightlythreshold}, 1"
      else
        threshold_f.puts "#{time_s}, #{nightlythreshold}, 0"
      end
    end
  end
end

def generate_density_file(density_file_p, folder, intensity_lines_a)
	## prepare density plot file
	density_output = File.open(density_file_p, 'w')
	density_files = Dir["#{folder}/*/1*.csv"].sort
	density_files.each do |file|

		data_h = Hash.new
		time_s = file.split('/')[-1].split('.')[0].to_i
		File.open(file).each_line do |line|
			data = line.split(', ')
			if data[0] == "0"
				unbalanced = data[3].to_i
				(unbalanced+1).times do |i|
				
					if data_h[i] == nil
						data_h[i] = 1 
					else
						data_h[i] += 1 
					end
				end
			end
		end
		density_output.print "#{time_s}, "
		intensity_lines_a.each do |intensity|
			if data_h[intensity] == nil
				density_output.print "0, "
			else
			 density_output.print "#{data_h[intensity]}, "
			end
		end
		density_output.print "\n"

	end
	density_output.flush
	density_output.close
end
def generate_density_cdf_file(density_cdf_file_p, folder)
  # index = Number of internal hosts (popularity)
	# element = [1,2,3,] How many external hosts/net/prefixes (data samples)
	key_distribution_a = Array.new
	key_max = 100
	0.upto(key_max) do |i|
		key_distribution_a[i] = HelperCDF.new
	end
	## parse prefix sets --> build the per timeslot histogram
	density_files = Dir["#{folder}/*/1*.csv"].sort
	density_files.each do |file|
		
		# build the distribution
		hist_h = Hash.new
		time_s = file.split('/')[-1].split('.')[0].to_i
		File.open(file).each_line do |line|
			
			data = line.split(', ')
			type = data[0].to_i
			prefix = data[1].to_i
			balanced = data[2].to_i
			unbalanced = data[3].to_i

			# truncate the distirubtion with a max
			unbalanced = key_max if unbalanced > key_max 
			if type == 0
				hist_h[unbalanced] = 0 if hist_h[unbalanced] == nil
				hist_h[unbalanced] += 1
			end
		end
		# add the values to the global one
		0.upto(key_max) do |i|
		if hist_h[i] != nil
			key_distribution_a[i].push(hist_h[i])
		else
			key_distribution_a[i].push(0)
		end
	end
end

