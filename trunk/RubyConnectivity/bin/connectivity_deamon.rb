#!/usr/bin/ruby1.9


if ARGV[0] == nil 
	puts "Usage <config folder>"
	exit
end

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
      puts "New File: '#{file_p}'"
      input_files_b.push(file_p)
    end 
  end
  iter_time = Time.now.to_i
  puts "Time now: '#{iter_time}'"
  temp = Array.new

  input_files_b.each do |file_p|
    f = open("|nfdump -r  #{file_p} -o "'fmt":"%pr,%sa,%sp,%da,%dp,%nh'"")
	f.each { |line| 
	if f.lineno==1
		next
	end
	if f.lineno==-2
		break
	end
	
	temp.push(line)
	}
	puts "#{temp}"
  end
  
  input_files_b.clear
  sleep(10)
  
  
end
