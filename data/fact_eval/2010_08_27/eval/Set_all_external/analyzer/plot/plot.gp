
## Host
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/plot/host.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'Host'
plot   '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/csv/density_host.csv' u 1:2 w filledcurve x1 t '1',   '' u 1:3 w filledcurve x1 t '2',   '' u 1:4 w filledcurve x1 t '5',   '' u 1:5 w filledcurve x1 t '10'
set log y
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/plot/host_log.eps'
replot


## Net
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/plot/net.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'Net'
plot   '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/csv/density_net.csv' u 1:2 w filledcurve x1 t '1',   '' u 1:3 w filledcurve x1 t '2',   '' u 1:4 w filledcurve x1 t '5',   '' u 1:5 w filledcurve x1 t '10'
set log y
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/plot/net_log.eps'
replot

## BGP
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/plot/bgp.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'BGP'
plot   '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/csv/density_bgp.csv' u 1:2 w filledcurve x1 t '1',   '' u 1:3 w filledcurve x1 t '2',   '' u 1:4 w filledcurve x1 t '5',   '' u 1:5 w filledcurve x1 t '10'
set log y
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_all_external/analyzer/plot/bgp_log.eps'
replot

