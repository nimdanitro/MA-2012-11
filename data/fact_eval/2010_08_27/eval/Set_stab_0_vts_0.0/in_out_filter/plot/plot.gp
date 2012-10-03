

#  1        2           3     4       5       6        7
# time_s, duration_s, flows, in_in, in_out, out_in, out_out

## FLOWS
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/in_out_filter/plot/flows.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'flows'

plot 	'/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/in_out_filter/csv/stat.csv' u ($1):($4+$5+$6+$7) w filledcurve x1 fs pattern 0 lc 4  t 'out out', 	'' u ($1):($4+$5+$6) w filledcurve x1 fs pattern 0 lc 3  t 'out in', 	'' u ($1):($4+$5) w filledcurve x1 fs pattern 0 lc 2  t 'in out',	'' u ($1):($4) w filledcurve x1 fs pattern 0 lc 1  t 'in in',   '' u ($1):($3) w l t 'all'

