#!/usr/bin/ruby1.9
require_relative '../lib/connectivity/helper.rb'


################################################################################
######## summary ###############################################################
################################################################################
def process_summary(data_folder_p)
		folder_plot_p = data_folder_p + "/summary"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Summary")

		gnuplot_p = folder_plot_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'

set ytics nomirror
set ylabel 'flows'
set terminal postscript eps enhanced color
set output '#{folder_plot_p}/filterbank.eps'
plot \
	'#{data_folder_p}/data_parser/stat.csv' u 1:2 w l  t 'filter bank in', \
	'#{data_folder_p}/filter_ipv6/stat.csv' u 1:($2-$3) w l  t 'filter ipv6', \
	'#{data_folder_p}/filter_border/stat.csv' u 1:($2-$3) w l  t 'filter border', \
	'#{data_folder_p}/filter_in_out/stat.csv' u 1:($2-$3) w l  t 'filter in out', \
	'#{data_folder_p}/filter_prefix_blacklist/stat.csv' u 1:($2-$3) w l  t 'filter blacklist'


"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
end

################################################################################
######## Process Monitor #######################################################
################################################################################
def process_process_monitor(data_folder_p)

		folder_p = data_folder_p + "/process_monitor"
		folder_plot_p = folder_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Process Monitor '#{folder_p}'")

		gnuplot_p = folder_plot_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

set terminal postscript eps enhanced color
set output '#{folder_plot_p}/process_monitor.eps'
set ylabel 'duration [s]'
plot \
	'#{folder_p}/stat.csv' u 1:($5) w l t 'data parser',\
	'#{folder_p}/stat.csv' u 1:($5+$6) w l t 'fiter bank',\
	'#{folder_p}/stat.csv' u 1:($5+$6+$7) w l t 'connection matrix',\
	'#{folder_p}/stat.csv' u 1:($5+$6+$7+$8) w l t 'analyser',\
	'#{folder_p}/stat.csv' u 1:($4) w l t 'ruby'

set output '#{folder_plot_p}/process_memory.eps'
set ylabel 'memory [MB]'
plot \
	'#{folder_p}/stat.csv' u 1:3 w l t 'memory'
"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
end

################################################################################
######## Data Parser ###########################################################
################################################################################
def process_data_parser(data_folder_p)

		folder_p = data_folder_p + "/data_parser"
		folder_plot_p = folder_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Data Parser '#{folder_p}'")

		gnuplot_p = folder_plot_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror
set ylabel 'flows per 300 s'
set terminal postscript eps enhanced color
set output '#{folder_plot_p}/data_parser.eps'
plot \
	'#{folder_p}/stat.csv' u 1:2 w l t 'flows'
"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
end
################################################################################
######## FILTER BANK ###########################################################
################################################################################
def process_filter_ipv6(data_folder_p)

		folder_p = data_folder_p + "/filter_ipv6"
		folder_plot_p = folder_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Filter IPv6 '#{folder_p}'")

		gnuplot_p = folder_plot_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set size 0.5,0.5
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror
set ylabel 'flows'
set terminal postscript eps enhanced color
set output '#{folder_plot_p}/filer_ipv6.eps'
plot \
	'#{folder_p}/stat.csv' u 1:2 w l t 'all',\
	'#{folder_p}/stat.csv' u 1:3 w l t 'filtered'

set output '#{folder_plot_p}/filer_ipv6_rel.eps'
set ylabel 'filter rate %'
plot \
	'#{folder_p}/stat.csv' u 1:($3*100/$2) w l t 'filtered'\
"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`

end
def process_filter_border(data_folder_p)

		folder_p = data_folder_p + "/filter_border"
		folder_plot_p = folder_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Filter Border '#{folder_p}'")

		gnuplot_p = folder_plot_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set size 0.5,0.5
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror
set ylabel 'flows'
set terminal postscript eps enhanced color
set output '#{folder_plot_p}/filer_border.eps'
plot \
	'#{folder_p}/stat.csv' u 1:2 w l t 'all',\
	'#{folder_p}/stat.csv' u 1:3 w l t 'filtered'

set output '#{folder_plot_p}/filer_border_rel.eps'
set ylabel 'filter rate %'
plot \
	'#{folder_p}/stat.csv' u 1:($3*100/$2) w l t 'filtered'\
"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
end
def process_filter_in_out(data_folder_p)

		folder_p = data_folder_p + "/filter_in_out"
		folder_plot_p = folder_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Filter In Out '#{folder_p}'")

		gnuplot_p = folder_plot_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set size 0.5,0.5
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror
set ylabel 'flows'
set terminal postscript eps enhanced color
set output '#{folder_plot_p}/filer_in_out.eps'
plot \
	'#{folder_p}/stat.csv' u 1:2 w l t 'all',\
	'#{folder_p}/stat.csv' u 1:3 w l t 'filtered'

set output '#{folder_plot_p}/filer_in_out_rel.eps'
set ylabel 'filter rate %'
plot \
	'#{folder_p}/stat.csv' u 1:($3*100/$2) w l t 'filtered'\

"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
end
def process_filter_blacklist(data_folder_p)
		folder_p = data_folder_p + "/filter_prefix_blacklist"
		folder_plot_p = folder_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Filter Blacklist '#{folder_p}'")

		gnuplot_p = folder_plot_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set size 0.5,0.5
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror
set ylabel 'flows'
set terminal postscript eps enhanced color
set output '#{folder_plot_p}/filer_blacklist.eps'
plot \
	'#{folder_p}/stat.csv' u 1:2 w l t 'all',\
	'#{folder_p}/stat.csv' u 1:3 w l t 'filtered'

set output '#{folder_plot_p}/filer_blacklist_rel.eps'
set ylabel 'filter rate %'
plot \
	'#{folder_p}/stat.csv' u 1:($3*100/$2) w l t 'filtered'\

"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
end

################################################################################
######## Connection Matrix #####################################################
################################################################################
def process_connection_matrix(data_folder_p)
		folder_p = data_folder_p + "/connection_matrix"
		folder_plot_p = folder_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Connection Matrix '#{folder_p}'")

		gnuplot_p = folder_plot_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror
set ylabel 'Connections'
set terminal postscript eps enhanced color
set output '#{folder_plot_p}/connection_matrix_cache_size.eps'
plot \
	'#{folder_p}/stat.csv' u 1:2 w l t 'cache size'


set output '#{folder_plot_p}/connection_matrix_type_001.eps'
plot \
	'#{folder_p}/stat.csv' u ($1):($3) w filledcurve x1 fs pattern 0 lc 5 t 'other', \
	'' u ($1):($5 + $6 + $9 + $8+ $12) w filledcurve x1 fs pattern 0 lc 2 t 'balance (1,2,5)', \
	'' u ($1):($8 + $12) w filledcurve x1 fs pattern 0 lc 1  t 'unbalance (4,8)'\



set output '#{folder_plot_p}/connection_matrix_type_002_a.eps'
set logscale y
plot \
	'#{folder_p}/stat.csv' u 1:3 w l t 'cache size', \
	'' u 1:($5+0.01) w l t '1',\
	'' u 1:($6+0.01) w l t '2',\
	'' u 1:($7+0.01) w l t '3',\
	'' u 1:($8+0.01) w l t '4',\
	'' u 1:($9+0.01) w l t '5',\
	'' u 1:($10+0.01) w l t '6',\
	'' u 1:($11+0.01) w l t '7',\
	'' u 1:($12+0.01) w l t '8'\

set output '#{folder_plot_p}/connection_matrix_type_002_b.eps'
set logscale y
plot \
	'#{folder_p}/stat.csv' u 1:3 w l t 'cache size', \
	'' u 1:($13+0.01) w l t '9',\
	'' u 1:($14+0.01) w l t '10',\
	'' u 1:($15+0.01) w l t '11',\
	'' u 1:($16+0.01) w l t '12',\
	'' u 1:($17+0.01) w l t '13',\
	'' u 1:($18+0.01) w l t '14',\
	'' u 1:($19+0.01) w l t '15',\
	'' u 1:($20+0.01) w l t '16'


"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
end



################################################################################
######## Analyser ##############################################################
################################################################################
def process_analyser(data_folder_p)

		folder_p = data_folder_p + "/analyser"
		folder_plot_p = folder_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		Log.debug("Process Analyser '#{folder_p}'")


### stat.csv IPv4 ###################################################################
		Log.debug("Process Analyser STAT.CSV for IPv4 '#{folder_p}'")
		gnuplot_p = folder_plot_p + "/plot_stat_ipv4.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

set terminal postscript eps enhanced color

### connections  ###############################################################
set output '#{folder_plot_p}/monitored_ipv4.eps'
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($3+$4+$5+$6) w filledcurve x1 fs pattern 0 lc 4 t 'if invalid', \
	'#{folder_p}/stat.csv' u 1:($4+$5+$6) w filledcurve x1 fs pattern 0 lc 3 t 'if not set', \
	'#{folder_p}/stat.csv' u 1:($5+$6) w filledcurve x1 fs pattern 0 lc 1 t 'if not monitored', \
	'#{folder_p}/stat.csv' u 1:($6) w filledcurve x1 fs pattern 0 lc 2 t 'monitored',\
	'#{folder_p}/stat.csv' u 1:2 w l lt 17  t 'ALL' \

set output '#{folder_plot_p}/state_ipv4.eps'
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($8 +$9 +$10) w filledcurve x1 fs pattern 0 lc 3 t 'other', \
	'#{folder_p}/stat.csv' u 1:($8 +$9) w filledcurve x1 fs pattern 0 lc 1 t 'unbalanced',\
	'#{folder_p}/stat.csv' u 1:($8) w filledcurve x1 fs pattern 0 lc 2 t 'balanced',\
	'#{folder_p}/stat.csv' u 1:7 w l lt 17 t 'ALL' \



set output '#{folder_plot_p}/weird_connections_ipv4.eps'
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($12 + $13 +$14 +$15) w filledcurve x1 fs pattern 0 lc 5 t 'no rule', \
	'#{folder_p}/stat.csv' u 1:($13 +$14 +$15) w filledcurve x1 fs pattern 0 lc 3 t 'weird zero', \
	'#{folder_p}/stat.csv' u 1:($14 +$15) w filledcurve x1 fs pattern 0 lc 1 t 'weird',\
	'#{folder_p}/stat.csv' u 1:($15) w filledcurve x1 fs pattern 0 lc 2 t 'ok', \
	'#{folder_p}/stat.csv' u 1:11 w l lt 17  t 'ALL' \

set output '#{folder_plot_p}/signal_ipv4.eps'
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($18+$17) w filledcurve x1 fs pattern 0 lc 3 t 'other', \
	'#{folder_p}/stat.csv' u 1:($17) w filledcurve x1 fs pattern 0 lc 1 t 'signal',\
	'#{folder_p}/stat.csv' u 1:16 w l lt 17  t 'ALL' \


set output '#{folder_plot_p}/overview_ipv4.eps'
set logscale y
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($1 + 0.1) w filledcurve x1 fs pattern 0 lc 3 t 'ALL', \
	'#{folder_p}/stat.csv' u 1:($8 + $17 + 0.1) w filledcurve x1 fs pattern 0 lc 2 t 'balanced', \
	'#{folder_p}/stat.csv' u 1:($17 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'unbalanced accounted signal'

"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
### stat.csv IPv6 ###################################################################  
		Log.debug("Process Analyser STAT.CSV for IPv6 '#{folder_p}'")
		gnuplot_p = folder_plot_p + "/plot_stat_ipv6.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

set terminal postscript eps enhanced color

### connections  ###############################################################
set output '#{folder_plot_p}/monitored_ipv6.eps'
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($21+$22+$23+$24) w filledcurve x1 fs pattern 0 lc 4 t 'if invalid', \
	'#{folder_p}/stat.csv' u 1:($22+$23+$24) w filledcurve x1 fs pattern 0 lc 3 t 'if not set', \
	'#{folder_p}/stat.csv' u 1:($23+$24) w filledcurve x1 fs pattern 0 lc 1 t 'if not monitored', \
	'#{folder_p}/stat.csv' u 1:($24) w filledcurve x1 fs pattern 0 lc 2 t 'monitored',\
	'#{folder_p}/stat.csv' u 1:20 w l lt 17  t 'ALL' \

set output '#{folder_plot_p}/state_ipv6.eps'
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($26 +$27 +$28) w filledcurve x1 fs pattern 0 lc 3 t 'other', \
	'#{folder_p}/stat.csv' u 1:($26 +$27) w filledcurve x1 fs pattern 0 lc 1 t 'unbalanced',\
	'#{folder_p}/stat.csv' u 1:($26) w filledcurve x1 fs pattern 0 lc 2 t 'balanced',\
	'#{folder_p}/stat.csv' u 1:25 w l lt 17 t 'ALL' \



set output '#{folder_plot_p}/weird_connections_ipv6.eps'
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($30 + $31 +$32 +$33) w filledcurve x1 fs pattern 0 lc 5 t 'no rule', \
	'#{folder_p}/stat.csv' u 1:($31 +$32 +$33) w filledcurve x1 fs pattern 0 lc 3 t 'weird zero', \
	'#{folder_p}/stat.csv' u 1:($32 +$33) w filledcurve x1 fs pattern 0 lc 1 t 'weird',\
	'#{folder_p}/stat.csv' u 1:($33) w filledcurve x1 fs pattern 0 lc 2 t 'ok', \
	'#{folder_p}/stat.csv' u 1:29 w l lt 17  t 'ALL' \

set output '#{folder_plot_p}/signal_ipv6.eps'
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($36+$35) w filledcurve x1 fs pattern 0 lc 3 t 'other', \
	'#{folder_p}/stat.csv' u 1:($35) w filledcurve x1 fs pattern 0 lc 1 t 'signal',\
	'#{folder_p}/stat.csv' u 1:34 w l lt 17  t 'ALL' \


set output '#{folder_plot_p}/overview_ipv6.eps'
set logscale y
set ylabel 'cons'
plot \
	'#{folder_p}/stat.csv' u 1:($20 + 0.1) w filledcurve x1 fs pattern 0 lc 3 t 'ALL', \
	'#{folder_p}/stat.csv' u 1:($26 + $37 + 0.1) w filledcurve x1 fs pattern 0 lc 2 t 'balanced', \
	'#{folder_p}/stat.csv' u 1:($37 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'unbalanced accounted signal'

"
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`

### host_host IPv4 ##################################################################
		Log.debug("Process Analyser host_host IPv4'#{folder_p}'")
		folder_host_host_p = data_folder_p + "/analyser/IPv4/host_host"
		folder_plot_p = folder_host_host_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		gnuplot_p = folder_host_host_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')

		Dir["#{folder_host_host_p}/*.csv"].each do |file|
		tag = File.basename(file).split('.')[-2]


		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

set terminal postscript eps enhanced color
set output '#{folder_plot_p}/host_host_ipv4_#{tag}.eps'
set logscale y
set ylabel 'host host keys'
plot \
	'#{file}' u 1:($3 + $2 + 0.11) w filledcurve x1 fs pattern 0 lc 2 t 'Reachable', \
	'#{file}' u 1:($3 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'Not Reachable'

"
	end
	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`

### host IPv4 ##################################################################
		Log.debug("Process Analyser host IPv4 '#{folder_p}'")
		folder_host_p = data_folder_p + "/analyser/IPv4/host"
		folder_plot_p = folder_host_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

    # Generating Threshold file..
		#parse_hostfile(folder_host_p)
		
		gnuplot_p = folder_host_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')

		Dir["#{folder_host_p}/*.csv"].each do |file|
		tag = File.basename(file).split('.')[-2]


		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

set terminal postscript eps enhanced color
set output '#{folder_plot_p}/host_ipv4_#{tag}.eps'
set logscale y
set ylabel 'host keys'

#		host_f << interf_hosts_not_routable << ", ";
#		host_f << interf_hosts_balanced << ", ";
#		host_f << interf_hosts_unbalanced << ", ";
#		host_f << interf_hosts_unbalanced_but_reachable << ", ";

plot \
	'#{file}' u 1:($2+$3+$4+$5 + 0.11) w filledcurve x1 fs pattern 0 lc 2 t 'reachable', \
	'#{file}' u 1:($2+$4+$5 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'not reachable', \
	'#{file}' u 1:($2+$5 + 0.1) w filledcurve x1 fs pattern 0 lc 8 t 'not reachable (over this)', \
	'#{file}' u 1:($2 + 0.1) w filledcurve x1 fs pattern 0 lc 4 t 'not routable'\

"
	end
  # # Generating Threshold file..
  # parse_hostfile(folder_host_p)
  # threshold_p = folder_host_p + "/threshold.csv"
  # allcsv_p = folder_host_p + "/all.csv"
  # gnuplot_f.puts "
  # set timefmt '%s'
  #   set xdata time
  #   set xlabel 'time'
  #   set ytics nomirror
  # 
  #   set terminal postscript eps enhanced color
  #   set output '#{folder_plot_p}/host_threshold.eps'
  #   set ylabel 'host unbalanced'
  # 
  # plot  '#{threshold_p}' using 1:2 w filledcurves x1 fs pattern 0 lc 3 t 'threshold', \
  #       '#{allcsv_p}' using 1:3 w filledcurves x1 fs pattern 0 lc 2 t 'unbalanced'
  # "
  # 
  # gnuplot_f.flush
  # gnuplot_f.close
  # `gnuplot #{gnuplot_p}`

### net IPv4##################################################################
		Log.debug("Process Analyser net IPv4 '#{folder_p}'")
		folder_net_p = data_folder_p + "/analyser/IPv4/net"
		folder_plot_p = folder_net_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		gnuplot_p = folder_net_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')

		Dir["#{folder_net_p}/*.csv"].each do |file|
		tag = File.basename(file).split('.')[-2]


		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

set terminal postscript eps enhanced color
set output '#{folder_plot_p}/net_ipv4_#{tag}.eps'
set logscale y
set ylabel 'net keys'

# net_f << time_s << ", ";
#	net_f << interf_net_balanced << ", ";
#	net_f << interf_net_unbalanced << ", ";
#	net_f << interf_net_unbalanced_but_reachable << ", ";

plot \
	'#{file}' u 1:($2+$3+$4 + 0.11) w filledcurve x1 fs pattern 0 lc 2 t 'reachable', \
	'#{file}' u 1:($3+$4 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'not reachable', \
	'#{file}' u 1:($4 + 0.1) w filledcurve x1 fs pattern 0 lc 8 t 'not reachable (over this)'

"
	end
	
  # # Generating Threshold file..
  # parse_netfile(folder_net_p)
  # threshold_p = folder_net_p + "/threshold.csv"
  # allcsv_p = folder_net_p + "/all.csv"
  # gnuplot_f.puts "
  # set timefmt '%s'
  #   set xdata time
  #   set xlabel 'time'
  #   set ytics nomirror
  # 
  #   set terminal postscript eps enhanced color
  #   set output '#{folder_plot_p}/net_threshold.eps'
  #   set ylabel 'net unbalanced'
  # 
  # plot  '#{threshold_p}' using 1:2 w filledcurves x1 fs pattern 0 lc 3 t 'threshold', \
  #       '#{allcsv_p}' using 1:3 w filledcurves x1 fs pattern 0 lc 2 t 'unbalanced'
  # "
  # gnuplot_f.flush
  # gnuplot_f.close
  # `gnuplot #{gnuplot_p}`

### bgp IPv4 ##################################################################
		Log.debug("Process Analyser bgp IPv4 '#{folder_p}'")
		folder_bgp_p = data_folder_p + "/analyser/IPv4/bgp"
		folder_plot_p = folder_bgp_p + "/plot"
		FileUtils::mkdir_p folder_plot_p

		gnuplot_p = folder_bgp_p + "/plot.gp"
		gnuplot_f = File.open(gnuplot_p, 'w')

		Dir["#{folder_bgp_p}/*.csv"].each do |file|
		tag = File.basename(file).split('.')[-2]


		gnuplot_f.puts "
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

set terminal postscript eps enhanced color
set output '#{folder_plot_p}/bgp_ipv4_#{tag}.eps'
set logscale y
set ylabel 'bgp keys'

# bgp_f << time_s << ", ";
#	bgp_f << interf_bgp_balanced << ", ";
#	bgp_f << interf_bgp_unbalanced << ", ";
#	bgp_f << interf_bgp_unbalanced_but_reachable << ", ";

plot \
	'#{file}' u 1:($2+$3+$4 + 0.11) w filledcurve x1 fs pattern 0 lc 2 t 'reachable', \
	'#{file}' u 1:($3+$4 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'not reachable', \
	'#{file}' u 1:($4 + 0.1) w filledcurve x1 fs pattern 0 lc 8 t 'not reachable (over this)'

"
	end
  # # Generating Threshold file..
  # parse_bgpfile(folder_bgp_p)
  # threshold_p = folder_bgp_p + "/threshold.csv"
  # allcsv_p = folder_bgp_p + "/all.csv"
  # gnuplot_f.puts "
  # set timefmt '%s'
  #   set xdata time
  #   set xlabel 'time'
  #   set ytics nomirror
  # 
  #   set terminal postscript eps enhanced color
  #   set output '#{folder_plot_p}/bgp_threshold.eps'
  #   set ylabel 'bgp keys'
  # 
  # plot  '#{threshold_p}' using 1:2 w filledcurves x1 fs pattern 0 lc 3 t 'threshold', \
  #       '#{allcsv_p}' using 1:3 w filledcurves x1 fs pattern 0 lc 2 t 'unbalanced'
  # 
  # "
  # gnuplot_f.flush
  # gnuplot_f.close
  # `gnuplot #{gnuplot_p}`

  ### host_host IPv6 ##################################################################
  		Log.debug("Process Analyser host_host IPv6 '#{folder_p}'")
  		folder_host_host_p = data_folder_p + "/analyser/IPv6/host_host"
  		folder_plot_p = folder_host_host_p + "/plot"
  		FileUtils::mkdir_p folder_plot_p

  		gnuplot_p = folder_host_host_p + "/plot.gp"
  		gnuplot_f = File.open(gnuplot_p, 'w')

  		Dir["#{folder_host_host_p}/*.csv"].each do |file|
  		tag = File.basename(file).split('.')[-2]


  		gnuplot_f.puts "
  set timefmt '%s'
  set xdata time
  set xlabel 'time'
  set ytics nomirror

  set terminal postscript eps enhanced color
  set output '#{folder_plot_p}/host_host_ipv6_#{tag}.eps'
  set logscale y
  set ylabel 'host host keys'
  plot \
  	'#{file}' u 1:($3 + $2 + 0.11) w filledcurve x1 fs pattern 0 lc 2 t 'Reachable', \
  	'#{file}' u 1:($3 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'Not Reachable'

  "
  	end
  	gnuplot_f.flush
  	gnuplot_f.close
  	`gnuplot #{gnuplot_p}`

  ### host IPv6 ##################################################################
  		Log.debug("Process Analyser host IPv6 '#{folder_p}'")
  		folder_host_p = data_folder_p + "/analyser/IPv6/host"
  		folder_plot_p = folder_host_p + "/plot"
  		FileUtils::mkdir_p folder_plot_p
  		
  		# Generating Threshold file..
  		#parse_hostfile(folder_host_p)

  		gnuplot_p = folder_host_p + "/plot.gp"
  		gnuplot_f = File.open(gnuplot_p, 'w')

  		Dir["#{folder_host_p}/*.csv"].each do |file|
  		tag = File.basename(file).split('.')[-2]


  		gnuplot_f.puts "
  set timefmt '%s'
  set xdata time
  set xlabel 'time'
  set ytics nomirror

  set terminal postscript eps enhanced color
  set output '#{folder_plot_p}/host_ipv6_#{tag}.eps'
  set logscale y
  set ylabel 'host keys'

  #		host_f << interf_hosts_not_routable << ", ";
  #		host_f << interf_hosts_balanced << ", ";
  #		host_f << interf_hosts_unbalanced << ", ";
  #		host_f << interf_hosts_unbalanced_but_reachable << ", ";

  plot \
  	'#{file}' u 1:($2+$3+$4+$5 + 0.11) w filledcurve x1 fs pattern 0 lc 2 t 'reachable', \
  	'#{file}' u 1:($2+$4+$5 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'not reachable', \
  	'#{file}' u 1:($2+$5 + 0.1) w filledcurve x1 fs pattern 0 lc 8 t 'not reachable (over this)', \
  	'#{file}' u 1:($2 + 0.1) w filledcurve x1 fs pattern 0 lc 4 t 'not routable'\

  "
  	end
  	gnuplot_f.flush
  	gnuplot_f.close
  	`gnuplot #{gnuplot_p}`

  ### net IPv6##################################################################
  		Log.debug("Process Analyser net IPv6 '#{folder_p}'")
  		folder_net_p = data_folder_p + "/analyser/IPv6/net"
  		folder_plot_p = folder_net_p + "/plot"
  		FileUtils::mkdir_p folder_plot_p
      
      # Generating Threshold file..
  		#parse_netfile(folder_net_p)
        
  		gnuplot_p = folder_net_p + "/plot.gp"
  		gnuplot_f = File.open(gnuplot_p, 'w')

  		Dir["#{folder_net_p}/*.csv"].each do |file|
  		tag = File.basename(file).split('.')[-2]


  		gnuplot_f.puts "
  set timefmt '%s'
  set xdata time
  set xlabel 'time'
  set ytics nomirror

  set terminal postscript eps enhanced color
  set output '#{folder_plot_p}/net_ipv6_#{tag}.eps'
  set logscale y
  set ylabel 'net keys'

  # net_f << time_s << ", ";
  #	net_f << interf_net_balanced << ", ";
  #	net_f << interf_net_unbalanced << ", ";
  #	net_f << interf_net_unbalanced_but_reachable << ", ";

  plot \
  	'#{file}' u 1:($2+$3+$4 + 0.11) w filledcurve x1 fs pattern 0 lc 2 t 'reachable', \
  	'#{file}' u 1:($3+$4 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'not reachable', \
  	'#{file}' u 1:($4 + 0.1) w filledcurve x1 fs pattern 0 lc 8 t 'not reachable (over this)'

  "
  	end
  	gnuplot_f.flush
  	gnuplot_f.close
  	`gnuplot #{gnuplot_p}`

  ### bgp IPv6 ##################################################################
  		Log.debug("Process Analyser bgp IPv6 '#{folder_p}'")
  		folder_bgp_p = data_folder_p + "/analyser/IPv6/bgp"
  		folder_plot_p = folder_bgp_p + "/plot"
  		FileUtils::mkdir_p folder_plot_p

  		gnuplot_p = folder_bgp_p + "/plot.gp"
  		gnuplot_f = File.open(gnuplot_p, 'w')

  		Dir["#{folder_bgp_p}/*.csv"].each do |file|
  		tag = File.basename(file).split('.')[-2]


  		gnuplot_f.puts "
  set timefmt '%s'
  set xdata time
  set xlabel 'time'
  set ytics nomirror

  set terminal postscript eps enhanced color
  set output '#{folder_plot_p}/bgp_ipv6_#{tag}.eps'
  set logscale y
  set ylabel 'bgp keys'

  # bgp_f << time_s << ", ";
  #	bgp_f << interf_bgp_balanced << ", ";
  #	bgp_f << interf_bgp_unbalanced << ", ";
  #	bgp_f << interf_bgp_unbalanced_but_reachable << ", ";

  plot \
  	'#{file}' u 1:($2+$3+$4 + 0.11) w filledcurve x1 fs pattern 0 lc 2 t 'reachable', \
  	'#{file}' u 1:($3+$4 + 0.1) w filledcurve x1 fs pattern 0 lc 1 t 'not reachable', \
  	'#{file}' u 1:($4 + 0.1) w filledcurve x1 fs pattern 0 lc 8 t 'not reachable (over this)'

  "
  	end
  	gnuplot_f.flush
  	gnuplot_f.close
  	`gnuplot #{gnuplot_p}`


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

		output = File.open(density_cdf_file_p, 'w')
		0.upto(key_max) do |i|
			output.print "#{i}, "
			output.print "#{key_distribution_a[i].get_v_from_q(0.05)}, "
			output.print "#{key_distribution_a[i].get_v_from_q(0.25)}, "
			output.print "#{key_distribution_a[i].get_v_from_q(0.5)}, "
			output.print "#{key_distribution_a[i].get_v_from_q(0.75)}, "
			output.print "#{key_distribution_a[i].get_v_from_q(0.95)}, "
			output.print "\n"

		end
		output.flush
		output.close
	end
	def extract_top_prefix(prefix_folder_p, min_time_count, min_hit_count)

		## prepare density plot file
		prefix_folders = Dir["#{prefix_folder_p}/*"].sort
		prefix_folders.each do |folder|
				if folder.split('/')[-1] =~ /\d/
					top_folder_p = folder + "/top"
					FileUtils::mkdir_p top_folder_p

					data_h = Hash.new

					# get the files
					Dir["#{folder}/1*"].sort.each do |file|
							File.open(file).each_line do |line|
								next if line =~ /#/ or line.size < 5
								data = line.split(', ')

							
								type = data[0].to_i
								if type == 0
									prefix =  data[1]
									unbalanced = data[3].to_i

									if data_h[prefix] == nil
											data_h[prefix] = [1, unbalanced]
									else
											data_h[prefix][0] += 1
											data_h[prefix][1] += unbalanced if data_h[prefix][1] < unbalanced
									end
								end
							end
					end # files

					# mixed
					out = File.open(top_folder_p +"/top_file_time_#{min_time_count}_hit_#{min_hit_count}.csv", 'w')
					data_as = data_h.sort do | a, b |
						comp = (a[1][1] <=> b[1][1])
						comp.zero? ? (a[1][0] <=> b[1][0]) : comp
					end 
					data_as.reverse_each do |key, data|
						if data[0] >= min_time_count or data[1] >= min_hit_count
							out.puts "#{key}, #{data*", "}"
						end
					end
					out.close
					# hit
					out = File.open(top_folder_p +"/top_file_hit_#{min_hit_count}.csv", 'w')
					data_as = data_h.sort do | a, b |
						a[1][1] <=> b[1][1]
					end 
					data_as.reverse_each do |key, data|
						if data[0] >= min_time_count or data[1] >= min_hit_count
							out.puts "#{key}, #{data*", "}"
						end
					end
					out.close
					# time
					out = File.open(top_folder_p +"/top_file_time_#{min_time_count}.csv", 'w')
					data_as = data_h.sort do | a, b |
						a[1][0] <=> b[1][0]
					end 
					data_as.reverse_each do |key, data|
						if data[0] >= min_time_count or data[1] >= min_hit_count
							out.puts "#{key}, #{data*", "}"
						end
					end
					out.close

				end
		end
	end
#### density plots #############################################################
	intensity_lines_a = [1,2,3,4,5,10,15,20,30]
	min_time_count = 10
	min_hit_count = 10
### density host IPv4 ###############################################################
		Log.debug("Process Analyser host prefix IPv4 '#{folder_p}'")
		folder_density_host_p = data_folder_p + "/analyser/IPv4/prefix_host"
		Dir["#{folder_density_host_p}/*"].each do |folder|
			tag = File.basename(folder).split('/')[-1]

			folder_plot_p = folder + "/plot"
			FileUtils::mkdir_p folder_plot_p


			density_file_p = folder_plot_p + "/density_ipv4.csv"
			generate_density_file(density_file_p, folder, intensity_lines_a)

			density_cdf_file_p = folder_plot_p + "/density_cdf_ipv4.csv"
			generate_density_cdf_file(density_cdf_file_p, folder + "/IPv4")


			gnuplot_p = folder_plot_p + "/plot.gp"
			gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set terminal postscript eps enhanced color
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

### Host Density
set autoscale
set ylabel 'Unbalanced host keys'
set output '#{folder_plot_p}/host_density_ipv4_#{tag}.eps'
set logscale y

"
				i = 0
				intensity_lines_a.each do |intensity|
					if i == 0
						gnuplot_f.puts "plot  '#{density_file_p}' u 1:#{i+2} w l t 'i:#{intensity}' \\"
					else
						gnuplot_f.puts ", '' u 1:($#{i+2} + 0.1) w l t 'i:#{intensity}' \\"
					end
					i+=1
				end 
gnuplot_f.puts "

### DIST I
reset
set autoscale
set output '#{folder_plot_p}/host_density_cdf_ipv4_#{tag}.eps'
unset logscale y
set xrange [1:10]

plot  \
	'#{density_cdf_file_p}' u 1:2 w l t 'P 05', \
	'#{density_cdf_file_p}' u 1:3 w l t 'P 25', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 50', \
	'#{density_cdf_file_p}' u 1:5 w l t 'P 74', \
	'#{density_cdf_file_p}' u 1:6 w l t 'P 95' \


set yrange [0.1:100]
set logscale y
set output '#{folder_plot_p}/host_density_cdf_log_ipv4_#{tag}.eps'
replot

"
				gnuplot_f.flush
				gnuplot_f.close
	`gnuplot #{gnuplot_p}`

			extract_top_prefix(folder, min_time_count, min_hit_count)
		end

	### density net IPv4 ###############################################################
		Log.debug("Process Analyser net prefix IPv4 '#{folder_p}'")
		folder_density_host_p = data_folder_p + "/analyser/IPv4/prefix_net"
		Dir["#{folder_density_host_p}/*"].each do |folder|
			tag = File.basename(folder).split('/')[-1]

			folder_plot_p = folder + "/plot"
			FileUtils::mkdir_p folder_plot_p

			density_file_p = folder_plot_p + "/density_ipv4.csv"
			generate_density_file(density_file_p, folder, intensity_lines_a)

			density_cdf_file_p = folder_plot_p + "/density_cdf_ipv4.csv"
			generate_density_cdf_file(density_cdf_file_p, folder )

			gnuplot_p = folder_plot_p + "/plot.gp"
			gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set terminal postscript eps enhanced color
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

### NET Density
set autoscale
set ylabel 'Unbalanced net keys'
set output '#{folder_plot_p}/net_density_ipv4_#{tag}.eps'
set logscale y
"
				i = 0
				intensity_lines_a.each do |intensity|
					if i == 0
						gnuplot_f.puts "plot  '#{density_file_p}' u 1:#{i+2} w l t 'i:#{intensity}' \\"
					else
						gnuplot_f.puts ", '' u 1:($#{i+2} + 0.1) w l t 'i:#{intensity}' \\"
					end
					i+=1
				end 
gnuplot_f.puts " 
### DIST I
reset
set autoscale
set output '#{folder_plot_p}/net_density_cdf_ipv4_#{tag}.eps'
unset logscale y
set xrange [1:10]

plot  \
	'#{density_cdf_file_p}' u 1:2 w l t 'P 05', \
	'#{density_cdf_file_p}' u 1:3 w l t 'P 25', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 50', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 74', \
	'#{density_cdf_file_p}' u 1:5 w l t 'P 95' \


set yrange [0.1:100]
set logscale y
set output '#{folder_plot_p}/net_density_cdf_log_ipv4_#{tag}.eps'
replot

"
				gnuplot_f.flush
				gnuplot_f.close
	`gnuplot #{gnuplot_p}`

			extract_top_prefix(folder, min_time_count, min_hit_count)
		end
	### density bgp IPv4 ##############################################################
		Log.debug("Process Analyser bgp prefix IPv4 '#{folder_p}'")
		folder_density_bgp_p = data_folder_p + "/analyser/IPv4/prefix_bgp"
		Dir["#{folder_density_bgp_p}/*"].each do |folder|
			tag = File.basename(folder).split('/')[-1]

			folder_plot_p = folder + "/plot"
			FileUtils::mkdir_p folder_plot_p

			density_file_p = folder_plot_p + "/density_ipv4.csv"
			generate_density_file(density_file_p, folder, intensity_lines_a)

			density_cdf_file_p = folder_plot_p + "/density_cdf_ipv4.csv"
			generate_density_cdf_file(density_cdf_file_p, folder + "/IPv4")

			gnuplot_p = folder_plot_p + "/plot.gp"
			gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set terminal postscript eps enhanced color
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

### BGP Density
set autoscale
set output '#{folder_plot_p}/bgp_density_ipv4_#{tag}.eps'
set logscale y
"
				i = 0
				intensity_lines_a.each do |intensity|
					if i == 0
						gnuplot_f.puts "plot  '#{density_file_p}' u 1:#{i+2} w l t 'i:#{intensity}' \\"
					else
						gnuplot_f.puts ", '' u 1:($#{i+2} + 0.1) w l t 'i:#{intensity}' \\"
					end
					i+=1
				end 
gnuplot_f.puts "
### DIST I
reset
set autoscale
set ylabel 'Unbalanced host keys'
set output '#{folder_plot_p}/bgp_density_cdf_ipv4_#{tag}.eps'
unset logscale y
set xrange [1:10]

plot  \
	'#{density_cdf_file_p}' u 1:2 w l t 'P 05', \
	'#{density_cdf_file_p}' u 1:3 w l t 'P 25', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 50', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 74', \
	'#{density_cdf_file_p}' u 1:5 w l t 'P 95' \


set yrange [0.1:100]
set logscale y
set output '#{folder_plot_p}/bgp_density_cdf_log_ipv4_#{tag}.eps'
replot

"
				gnuplot_f.flush
				gnuplot_f.close
	`gnuplot #{gnuplot_p}`

			extract_top_prefix(folder, min_time_count, min_hit_count)
		end


### density host IPv6 ###############################################################
		Log.debug("Process Analyser host prefix IPv6 '#{folder_p}'")
		folder_density_host_p = data_folder_p + "/analyser/IPv6/prefix_host"
		Dir["#{folder_density_host_p}/*"].each do |folder|
			tag = File.basename(folder).split('/')[-1]

			folder_plot_p = folder + "/plot"
			FileUtils::mkdir_p folder_plot_p


			density_file_p = folder_plot_p + "/density_ipv6.csv"
			generate_density_file(density_file_p, folder, intensity_lines_a)

			density_cdf_file_p = folder_plot_p + "/density_cdf_ipv6.csv"
			generate_density_cdf_file(density_cdf_file_p, folder + "/IPv6")


			gnuplot_p = folder_plot_p + "/plot.gp"
			gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set terminal postscript eps enhanced color
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

### Host Density
set autoscale
set ylabel 'Unbalanced host keys'
set output '#{folder_plot_p}/host_density_ipv6_#{tag}.eps'
set logscale y

"
				i = 0
				intensity_lines_a.each do |intensity|
					if i == 0
						gnuplot_f.puts "plot  '#{density_file_p}' u 1:#{i+2} w l t 'i:#{intensity}' \\"
					else
						gnuplot_f.puts ", '' u 1:($#{i+2} + 0.1) w l t 'i:#{intensity}' \\"
					end
					i+=1
				end 
gnuplot_f.puts "

### DIST I
reset
set autoscale
set output '#{folder_plot_p}/host_density_cdf_ipv6_#{tag}.eps'
unset logscale y
set xrange [1:10]

plot  \
	'#{density_cdf_file_p}' u 1:2 w l t 'P 05', \
	'#{density_cdf_file_p}' u 1:3 w l t 'P 25', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 50', \
	'#{density_cdf_file_p}' u 1:5 w l t 'P 74', \
	'#{density_cdf_file_p}' u 1:6 w l t 'P 95' \


set yrange [0.1:100]
set logscale y
set output '#{folder_plot_p}/host_density_cdf_log_ipv6_#{tag}.eps'
replot

"
				gnuplot_f.flush
				gnuplot_f.close
	`gnuplot #{gnuplot_p}`

			extract_top_prefix(folder, min_time_count, min_hit_count)
		end

	### density net IPv6 ###############################################################
		Log.debug("Process Analyser net prefix IPv6'#{folder_p}'")
		folder_density_host_p = data_folder_p + "/analyser/IPv6/prefix_net"
		Dir["#{folder_density_host_p}/*"].each do |folder|
			tag = File.basename(folder).split('/')[-1]

			folder_plot_p = folder + "/plot"
			FileUtils::mkdir_p folder_plot_p

			density_file_p = folder_plot_p + "/density_ipv6.csv"
			generate_density_file(density_file_p, folder, intensity_lines_a)

			density_cdf_file_p = folder_plot_p + "/density_cdf_ipv6.csv"
			generate_density_cdf_file(density_cdf_file_p, folder + "/IPv6")

			gnuplot_p = folder_plot_p + "/plot.gp"
			gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set terminal postscript eps enhanced color
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

### NET Density
set autoscale
set ylabel 'Unbalanced net keys'
set output '#{folder_plot_p}/net_density_ipv6_#{tag}.eps'
set logscale y
"
				i = 0
				intensity_lines_a.each do |intensity|
					if i == 0
						gnuplot_f.puts "plot  '#{density_file_p}' u 1:#{i+2} w l t 'i:#{intensity}' \\"
					else
						gnuplot_f.puts ", '' u 1:($#{i+2} + 0.1) w l t 'i:#{intensity}' \\"
					end
					i+=1
				end 
gnuplot_f.puts " 
### DIST I
reset
set autoscale
set output '#{folder_plot_p}/net_density_cdf_ipv6_#{tag}.eps'
unset logscale y
set xrange [1:10]

plot  \
	'#{density_cdf_file_p}' u 1:2 w l t 'P 05', \
	'#{density_cdf_file_p}' u 1:3 w l t 'P 25', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 50', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 74', \
	'#{density_cdf_file_p}' u 1:5 w l t 'P 95' \


set yrange [0.1:100]
set logscale y
set output '#{folder_plot_p}/net_density_cdf_log_ipv6_#{tag}.eps'
replot

"
				gnuplot_f.flush
				gnuplot_f.close
	`gnuplot #{gnuplot_p}`

			extract_top_prefix(folder, min_time_count, min_hit_count)
		end
	### density bgp IPv6 ##############################################################
		Log.debug("Process Analyser bgp prefix IPv6 '#{folder_p}'")
		folder_density_bgp_p = data_folder_p + "/analyser/IPv6/prefix_bgp"
		Dir["#{folder_density_bgp_p}/*"].each do |folder|
			tag = File.basename(folder).split('/')[-1]

			folder_plot_p = folder + "/plot"
			FileUtils::mkdir_p folder_plot_p

			density_file_p = folder_plot_p + "/density_ipv6.csv"
			generate_density_file(density_file_p, folder, intensity_lines_a)

			density_cdf_file_p = folder_plot_p + "/density_cdf_ipv6.csv"
			generate_density_cdf_file(density_cdf_file_p, folder + "/IPv6")

			gnuplot_p = folder_plot_p + "/plot.gp"
			gnuplot_f = File.open(gnuplot_p, 'w')
		gnuplot_f.puts "
set terminal postscript eps enhanced color
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror

### BGP Density
set autoscale
set output '#{folder_plot_p}/bgp_density_ipv6_#{tag}.eps'
set logscale y
"
				i = 0
				intensity_lines_a.each do |intensity|
					if i == 0
						gnuplot_f.puts "plot  '#{density_file_p}' u 1:#{i+2} w l t 'i:#{intensity}' \\"
					else
						gnuplot_f.puts ", '' u 1:($#{i+2} + 0.1) w l t 'i:#{intensity}' \\"
					end
					i+=1
				end 
gnuplot_f.puts "
### DIST I
reset
set autoscale
set ylabel 'Unbalanced host keys'
set output '#{folder_plot_p}/bgp_density_cdf_ipv6_#{tag}.eps'
unset logscale y
set xrange [1:10]

plot  \
	'#{density_cdf_file_p}' u 1:2 w l t 'P 05', \
	'#{density_cdf_file_p}' u 1:3 w l t 'P 25', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 50', \
	'#{density_cdf_file_p}' u 1:4 w l t 'P 74', \
	'#{density_cdf_file_p}' u 1:5 w l t 'P 95' \


set yrange [0.1:100]
set logscale y
set output '#{folder_plot_p}/bgp_density_cdf_log_ipv6_#{tag}.eps'
replot

"
				gnuplot_f.flush
				gnuplot_f.close
	`gnuplot #{gnuplot_p}`

			extract_top_prefix(folder, min_time_count, min_hit_count)
		end
end



################################################################################
######## MAIN ##################################################################
################################################################################
<def process(data_folder_p)
	Log.debug("Process data from folder #{data_folder_p}")

	process_summary(data_folder_p)

	process_process_monitor(data_folder_p)

	# data parser
	process_data_parser(data_folder_p)

	# filter bank
	process_filter_ipv6(data_folder_p)
	process_filter_border(data_folder_p)
	process_filter_in_out(data_folder_p)
	process_filter_blacklist(data_folder_p)

	# connection matrix
	process_connection_matrix(data_folder_p)

	# analyser
	process_analyser(data_folder_p)

end
################################################################################

if (
	ARGV[0] == nil or 
	File.directory?(ARGV[0]) == false 
	#or File.directory?(ARGV[0]+"/connection_matrix") == false
)
	puts "Usage <extract connectivity folder>"
	exit
end
process(ARGV[0])
#process_analyser(ARGV[0])

=begin
analyser_tags_a = [
	'analyser__r_A_i_A_all__32_bgp', 
	'analyser__r_01_i_02_all__32_bgp',
	'analyser__r_01_i_06_all__32_bgp',
	'analyser__r_02_i_01_all__32_bgp',
	'analyser__r_02_i_02_all__32_bgp',
	'analyser__r_02_i_11_all__32_bgp',
	'analyser__r_02_i_15_all__32_bgp',
	'analyser__r_02_i_16_all__32_bgp',
	'analyser__r_03_i_11_all__32_bgp',
	'analyser__r_04_i_76_all__32_bgp',
	'analyser__r_04_i_81_all__32_bgp',
	'analyser__r_04_i_86_all__32_bgp',
	'analyser__r_05_i_05_all__32_bgp',
	'analyser__r_05_i_36_all__32_bgp',
	'analyser__r_A_i_A_all__32_24', 
	'analyser__r_01_i_02_all__32_24',
	'analyser__r_01_i_06_all__32_24',
	'analyser__r_02_i_01_all__32_24',
	'analyser__r_02_i_02_all__32_24',
	'analyser__r_02_i_11_all__32_24',
	'analyser__r_02_i_15_all__32_24',
	'analyser__r_02_i_16_all__32_24',
	'analyser__r_03_i_11_all__32_24',
	'analyser__r_04_i_76_all__32_24',
	'analyser__r_04_i_81_all__32_24',
	'analyser__r_04_i_86_all__32_24',
	'analyser__r_05_i_05_all__32_24',
	'analyser__r_05_i_36_all__32_24'
]
################################################################################
######## Analyser BGP ##########################################################
################################################################################
def process_analyser(data_folder_p, tag)
	folder_p = data_folder_p + "/#{tag}"
	return if File.directory?(folder_p) == false


	folder_plot_p = folder_p + "/plot"
	FileUtils::mkdir_p folder_plot_p

	Log.debug("Process Analyser '#{folder_p}'")


	## prepare density plot file
	intensity_lines_a = [1,2,3,4,5,10,15,20,30]


	density_output_p = "#{folder_plot_p}/analyser_bgp_density.csv"
	density_output = File.open(density_output_p, 'w')
	density_files = Dir["#{folder_p}/density/*/*.csv"].sort
	density_files.each do |file|
		data_h = Hash.new
		time_s = file.split('/')[-1].split('.')[0].to_i
		File.open(file).each_line do |line|
			data = line.split(', ')
			data_h[data[0].to_i] = [data[1].to_i, data[2].to_i]
		end
		density_output.print "#{time_s}, "
		intensity_lines_a.each do |intensity|
			density_output.print "#{data_h[intensity][1]}, "
		end
		density_output.print "\n"
	end
	density_output.flush
	density_output.close


	gnuplot_p = folder_plot_p + "/plot.gp"
	gnuplot_f = File.open(gnuplot_p, 'w')
	gnuplot_f.puts "
#set size 0.5,0.5
set timefmt '%s'
set xdata time
set xlabel 'time'
set ytics nomirror
set ylabel '5tp'
set terminal postscript eps enhanced color

### 5Tupels  ###################################################################
set output '#{folder_plot_p}/#{tag}__5tp.eps'
plot \
	'#{folder_p}/stat.csv' u 1:2 w l lt 0 t 'processed', \
	'' u 1:3 w l lt 2 t 'balanced', \
	'' u 1:4 w l lt 1 t 'unbalanced', \
	'' u 1:5 w l lt 3 t 'unbalanced (weird connections)', \
	'' u 1:6 w l lt 4 t 'unbalanced (non signal)'

set logscale y
set output '#{folder_plot_p}/#{tag}__5tp_log.eps'
plot \
	'#{folder_p}/stat.csv' u 1:2 w l lt 0 t 'processed', \
	'' u 1:($3+0.1) w l lt 2 t 'balanced', \
	'' u 1:($4+0.1) w l lt 1 t 'unbalanced', \
	'' u 1:($5+0.1) w l lt 3 t 'unbalanced (weird connections)', \
	'' u 1:($6+0.1) w l lt 4 t 'unbalanced (non signal)'
unset logscale y

set output '#{folder_plot_p}/#{tag}__5tp_balance.eps'
plot \
	'#{folder_p}/stat.csv' u 1:3 w l lt 2 t 'balanced', \
	'' u 1:4 w l lt 1 t 'unbalanced'

set logscale y
set output '#{folder_plot_p}/#{tag}__5tp_balance_log.eps'
plot \
	'#{folder_p}/stat.csv' u 1:($3 + 0.1) w l lt 2 t 'balanced', \
	'' u 1:($4 + 0.1) w l lt 1 t 'unbalanced'
unset logscale y

### NET -- NET  ################################################################
set ylabel 'unique net-net keys'
set output '#{folder_plot_p}/#{tag}__key_net_net.eps'
plot \
	'#{folder_p}/stat.csv' u 1:7 w l lt 0 t 'keys total', \
	'' u 1:8 w l lt 2 t 'keys balanced', \
	'' u 1:9 w l lt 1 t 'keys unbalanced'

set logscale y
set output '#{folder_plot_p}/#{tag}__key_net_net_log.eps'
plot \
	'#{folder_p}/stat.csv' u 1:($7+0.1) w l lt 0 t 'keys total', \
	'' u 1:($8+0.1) w l lt 2 t 'keys balanced', \
	'' u 1:($9+0.1) w l lt 1 t 'keys unbalanced'
unset logscale y

set ylabel 'unique net-net keys'
set yrange [0:105]
set output '#{folder_plot_p}/#{tag}__key_net_net_percentage.eps'
plot '#{folder_p}/stat.csv' u 1:($8*100/$7) w l lt 2 t 'keys balanced', \
	'' u 1:($9*100/$7) w l lt 1 t 'keys unbalanced'


### NET ########################################################################
set autoscale

set ylabel 'unique net keys'
set output '#{folder_plot_p}/#{tag}__key_net.eps'
plot \
	'#{folder_p}/stat.csv' u 1:10 w l lt 0 t 'keys total', \
	'' u 1:11 w l lt 2 t 'keys balanced', \
	'' u 1:12 w l lt 1 t 'keys unbalanced'

set logscale y
set output '#{folder_plot_p}/#{tag}__key_net_log.eps'
plot \
	'#{folder_p}/stat.csv' u 1:($10 + 0.1) w l lt 0 t 'keys total', \
	'' u 1:($11 + 0.1) w l lt 2 t 'keys balanced', \
	'' u 1:($12 + 0.1) w l lt 1 t 'keys unbalanced'
unset logscale y

set yrange [0:105]
set output '#{folder_plot_p}/#{tag}__key_net_percentage.eps'
plot '#{folder_p}/stat.csv' u 1:($11*100/$10) w l lt 2 t 'keys balanced', \
	'' u 1:($12*100/$10) w l lt 1 t 'keys unbalanced'

set logscale y
set yrange [0.001:105]
set output '#{folder_plot_p}/#{tag}__key_net_percentage_log.eps'
plot '#{folder_p}/stat.csv' u 1:($11*100/$10 + 0.001) w l lt 2 t 'keys balanced', \
	'' u 1:($12*100/$10 + 0.001) w l lt 1 t 'keys unbalanced'
unset logscale y
unset yrange


### NET Density
set autoscale
set ylabel 'Number of unbalanced net keys'
set output '#{folder_plot_p}/#{tag}__density.eps'
"
	i = 0
	intensity_lines_a.each do |intensity|
		if i == 0
			gnuplot_f.puts "plot  '#{density_output_p}' u 1:#{i+2} w l t 'i:#{intensity}' \\"
		else
			gnuplot_f.puts ", '' u 1:#{i+2} w l t 'i:#{intensity}' \\"
		end
		i+=1
	end 

	gnuplot_f.flush
	gnuplot_f.close
	`gnuplot #{gnuplot_p}`
end
=end


