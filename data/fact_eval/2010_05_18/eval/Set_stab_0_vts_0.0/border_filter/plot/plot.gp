

#  1        2           3     4       5           6 
# time_s, duration_s, flows, other, border_in, border_out, 

## FLOWS
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_stab_0_vts_0.0/border_filter/plot/flows.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'flows'

plot 	'/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_stab_0_vts_0.0/border_filter/csv/stat.csv' u ($1):($4+$5+$6) w filledcurve x1 fs pattern 0 lc 3  t 'border out', 	'' u ($1):($4+$5) w filledcurve x1 fs pattern 0 lc 2  t 'border in',	'' u ($1):($4) w filledcurve x1 fs pattern 0 lc 1  t 'other',   '' u ($1):($3) w l t 'all'



