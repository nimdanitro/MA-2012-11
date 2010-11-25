#!/usr/bin/ruby1.9
require_relative '../lib/connectivity/helper.rb'


if ARGV[0] == nil or ARGV[1] == nil
	puts "Usage <config folder> <config folder>"
	exit
end
output_folder = ARGV[1]
data_parser = DataParser.new(output_folder + '/data_parser', 300)

# input ::: NFCAP FILES...
input_folder = ARGV[0]  
input_files_b = Array.new
 


# time
init_time = Time.now
init_time = init_time.to_i
iter_time = 0
puts "INIT at: '#{init_time}'"

loop do
  input_files_a = Dir["#{input_folder}/*"].sort!
  input_files_a.each do |file_p|
    cur_m_time = File.new(file_p).mtime
    if(cur_m_time.to_i > iter_time)
      puts "new input file: '#{file_p}'"
      input_files_b.push(file_p)
    end 
  end
  iter_time = Time.now.to_i
  puts "Time now: '#{iter_time}'"
  #puts "#{input_files_b}"
  
  unless input_files_b.empty?
    data_parser.nf_parse_files(input_files_b)
  end
  input_files_b.clear
  sleep(10)
  
  
end
