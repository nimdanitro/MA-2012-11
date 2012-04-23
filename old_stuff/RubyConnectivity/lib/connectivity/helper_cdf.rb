class HelperCDF

	Version = "00.00.01"
	VersionNbr = 000001

	def initialize
		@h = Hash.new
		@a = nil
		clear
	end
	def set_label(label)
		@label = label
	end
	def label
		return(@label)
	end
	def clear
		@h.clear()
		@elements = 0
		@a = nil
		@label = ""
		@gnuplot_x_range = ''
		@gnuplot_y_range = 'set yrange [0.001:1]'
		@gnuplot_ccdf_x_range = 'set xrange [1:100000000]'
		@gnuplot_ccdf_y_range = 'set yrange [0.001:1]'
	end
################################################################################
	def set_gnuplot_xrange(x_range)
		@gnuplot_x_range = x_range
	end
	def get_gnuplot_xrange()
		return @gnuplot_x_range
	end
	def set_gnuplot_yrange(y_range)
		@gnuplot_y_range = y_range
	end
	def get_gnuplot_yrange()
		return @gnuplot_y_range
	end

	def set_gnuplot_ccdf_xrange(x_range)
		@gnuplot_ccdf_x_range = x_range
	end
	def get_gnuplot_ccdf_xrange()
		return @gnuplot_ccdf_x_range
	end
	def set_gnuplot_ccdf_yrange(y_range)
		@gnuplot_ccdf_y_range = y_range
	end
	def get_gnuplot_ccdf_yrange()
		return @gnuplot_ccdf_y_range
	end

	def push(key, value = 1)

		if @h[key] == nil
			@h[key] = value
		else
			@h[key] += value
		end
		@elements += 1
		@a = nil
	end
	def to_file(path)
		# <key>, count(<key>), p(<key>), count(min+, <key>), cdf, ccdf) 
		@a = @h.sort()
		cdf = 0
		out = File.open(path, 'w')
		out.puts "# HelperCDF (#{Version}): key, count, p, sum, cdf, ccdf"
		@a.each do |el|
			key = el[0]
			value = el[1]
			cdf += value
			out.puts "#{key}, #{value}, #{value.to_f/@elements}, #{cdf}, #{cdf.to_f/@elements}, #{1 - cdf.to_f/@elements}"
		end
		out.flush()
		out.close()
	end
	def from_file(path)
		clear()
		line_a = nil
		File.open(path).each_line do |line|
			line_a = line.split(', ')
			if line.size < 4 or line[0] == '#' or line_a.size != 6
				#puts "comment/invalide line '#{line}'"
				next
			end
			@h[line_a[0].to_i] = line_a[1].to_i
			@elements += line_a[1].to_i
		end
		
	end

	def get_v_from_q(q)
		return 0 if @h.size == 0
		@a = @h.sort() if @a==nil
		last_key = 0
		last_cdf = 0

		sum = 0.0
		cdf = 0
		@a.each do |key, count|
			sum += count
			cdf = sum/@elements
			if(cdf  == q)
				return(key)
			elsif(cdf > q)
				avg_key =  last_key + (key - last_key).to_f/(cdf - last_cdf)*(q-last_cdf)
				return(avg_key)
			else
				last_key = key
				last_cdf = cdf
			end
		end
		return(last_key)
	end

	def get_q_from_v(v)
		@a = @h.sort() if @a==nil
		last_key = 0
		last_cdf = 0
		sum = 0.0
		cdf = 0
		@a.each do |key, count|
			sum += count
			cdf = sum/@elements
			if(key == v)
				return(cdf)
			elsif(key > v)
				avg_cdf = last_cdf + (cdf - last_cdf).to_f/(key - last_key)*(v-last_key)
				return(avg_cdf)
			else
				last_key = key
				last_cdf = cdf
			end
		end
		return(last_cdf)
	end


	def self.to_gnuplot(path, files_p, using_t = "u 1:5", head_t = "set terminal postscript eps enhanced color")
		gp = File.open("#{path}.gp", 'w')
		gp.puts "#{head_t}"
		gp.puts "set output '#{path}'"
		gp.print "plot "
		files_p.each_index do |index|
			gp.print " '#{files_p[index]}' "
			if using_t.class == Array
				gp.print "  #{using_t[index]} " 
			else
				gp.print "  #{using_t} " 
			end
			gp.print " , " if index < (files_p.size() -1)
		end
		gp.flush()
		gp.close()
		`gnuplot #{"#{path}.gp"}`
	end

	def self.cdfs_to_gnuplot_a(cdfs_a, file_p, header = "")
		folder_p = File.dirname(file_p)
		plot_type = File.basename(file_p).split('.')[-1]

		gp_p = "#{file_p}.gp"
		gp_d = "#{file_p}.data"

		# generate data file
		gpd_f = File.open(gp_d, 'w')
		0.upto(100) do |i|
			q = i/100.0
			gpd_f.print "#{q}, "
			cdfs_a.each do |cdf|
				gpd_f.print "#{cdf.get_v_from_q(q)}, "
			end
			gpd_f.print "\n"
		end
		gpd_f.flush
		gpd_f.close


		# generate plot
		gp_f = File.open(gp_p, 'w')
		if plot_type == 'eps'
			gp_f.puts "set terminal postscript eps enhanced color"
		elsif plot_type == 'svg'
			gp_f.puts "set terminal svg enhanced"
		else
			throw "Unknon Type Plot Type: '#{plot_type}'"
		end
		gp_f.puts "set out '#{file_p}'"

		gp_f.puts header if header.size > 0

		gp_f.print " plot "
		cdfs_a.each_index do |i|
			gp_f.print "'#{gp_d}' u #{i+2}:1 w l t '#{cdfs_a[i].label}', " if i != cdfs_a.size - 1
			gp_f.print "'#{gp_d}' u #{i+2}:1 w l t '#{cdfs_a[i].label}'" if i == cdfs_a.size - 1
		end
		gp_f.flush
		gp_f.close

		system("gnuplot #{gp_p}")
	end
	def self.ccdfs_to_gnuplot_a(cdfs_a, file_p, header = "")
		folder_p = File.dirname(file_p)
		plot_type = File.basename(file_p).split('.')[-1]

		gp_p = "#{file_p}.gp"
		gp_d = "#{file_p}.data"

		# generate data file
		gpd_f = File.open(gp_d, 'w')
		0.upto(90) do |i|
			q = i/100.0
			gpd_f.print "#{1-q}, "
			cdfs_a.each do |cdf|
				gpd_f.print "#{cdf.get_v_from_q(q)}, "
			end
			gpd_f.print "\n"
		end
		0.upto(99) do |i|
			q = 0.9 + i/1000.0
			gpd_f.print "#{1-q}, "
			cdfs_a.each do |cdf|
				gpd_f.print "#{cdf.get_v_from_q(q)}, "
			end
			gpd_f.print "\n"
		end

		gpd_f.flush
		gpd_f.close


		# generate plot
		gp_f = File.open(gp_p, 'w')
		if plot_type == 'eps'
			gp_f.puts "set terminal postscript eps enhanced color"
		elsif plot_type == 'svg'
			gp_f.puts "set terminal svg enhanced"
		else
			throw "Unknon Type Plot Type: '#{plot_type}'"
		end
		gp_f.puts "set out '#{file_p}'"

		gp_f.puts header if header.size > 0

		gp_f.print " plot "
		cdfs_a.each_index do |i|
			gp_f.print "'#{gp_d}' u #{i+2}:1 w l t '#{cdfs_a[i].label}', " if i != cdfs_a.size - 1
			gp_f.print "'#{gp_d}' u #{i+2}:1 w l t '#{cdfs_a[i].label}'" if i == cdfs_a.size - 1
		end
		gp_f.flush
		gp_f.close

		system("gnuplot #{gp_p}")
	end

	def to_plot2(file_p, header = '')

		folder_p = File.dirname(file_p)
		plot_type = File.basename(file_p).split('.')[-1]

		gp_p = "#{file_p}.gp"
		gp_d = "#{file_p}.data"

		# generate data file
		gpd_f = File.open(gp_d, 'w')
		0.upto(100) do |i|
			q = i/100.0
			gpd_f.print "#{q}, "
			gpd_f.print "#{get_v_from_q(q)}, "
			gpd_f.print "\n"
		end
		gpd_f.flush
		gpd_f.close


		# generate plot
		gp_f = File.open(gp_p, 'w')
		if plot_type == 'eps'
			gp_f.puts "set terminal postscript eps enhanced color"
		elsif plot_type == 'svg'
			gp_f.puts "set terminal svg enhanced"
		else
			throw "Unknon Type Plot Type: '#{plot_type}'"
		end
		gp_f.puts "set out '#{file_p}'"
		gp_f.puts @gnuplot_x_range if @gnuplot_x_range.size > 0
		gp_f.puts @gnuplot_y_range if @gnuplot_y_range.size > 0
		gp_f.puts header if header.size > 0

		gp_f.print " plot "
		gp_f.print "'#{gp_d}' u 2:1 w l t '#{@label}'"
		gp_f.flush
		gp_f.close

		system("gnuplot #{gp_p}")
	end

	def to_plot2_ccdf(file_p, header = '')

		folder_p = File.dirname(file_p)
		plot_type = File.basename(file_p).split('.')[-1]

		gp_p = "#{file_p}.gp"
		gp_d = "#{file_p}.data"

		# generate data file (log sampling)
		gpd_f = File.open(gp_d, 'w')
		0.upto(90) do |i|
			q = i/100.0
			gpd_f.print "#{1-q}, "
			gpd_f.print "#{get_v_from_q(q)}"	
			gpd_f.print "\n"
		end
		0.upto(99) do |i|
			q = 0.9 + i/1000.0
			gpd_f.print "#{1-q}, "
			gpd_f.print "#{get_v_from_q(q)} "
			gpd_f.print "\n"
		end
		gpd_f.flush
		gpd_f.close


		# generate plot
		gp_f = File.open(gp_p, 'w')
		if plot_type == 'eps'
			gp_f.puts "set terminal postscript eps enhanced color"
		elsif plot_type == 'svg'
			gp_f.puts "set terminal svg enhanced"
		else
			throw "Unknon Type Plot Type: '#{plot_type}'"
		end
		gp_f.puts "set logscale x"
		gp_f.puts "set logscale y"
		gp_f.puts "set out '#{file_p}'"
		gp_f.puts @gnuplot_ccdf_x_range if @gnuplot_ccdf_x_range.size > 0
		gp_f.puts @gnuplot_ccdf_y_range if @gnuplot_ccdf_y_range.size > 0
		gp_f.puts header if header.size > 0

		gp_f.print " plot "
		gp_f.print "'#{gp_d}' u 2:1 w l t '#{@label}'"
		gp_f.flush
		gp_f.close

		system("gnuplot #{gp_p}")
	end

end

