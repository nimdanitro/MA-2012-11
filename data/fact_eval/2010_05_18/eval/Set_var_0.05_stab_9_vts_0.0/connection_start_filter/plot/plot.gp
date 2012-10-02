

### STAT index MAP
#   1         2               3         4     5         6            
# time_s, processing_time_s, sessions, aged, biflows, biflows_bi_new,
#   7                      8                   9              
# biflows_bi_new_event, biflows_bi_known, biflows_in_out_new, 
#    10                      11                     12            
# biflows_in_out_new_bpp, biflows_in_out_known, biflows_out_in_new
#   13
# biflows_out_in_known

## BIFLOWS Types
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_var_0.05_stab_9_vts_0.0/connection_start_filter/plot/biflow_classes.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'flows'

plot 	'/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_var_0.05_stab_9_vts_0.0/connection_start_filter/csv/stat.csv' u ($1):($6+$8 +$9+$11 +$12+$13) w filledcurve x1 fs pattern 0 lc 6  t 'bi new', 	'' u ($1):($8 +$9+$11 +$12+$13) w filledcurve x1 fs pattern 0 lc 5  t 'bi known', 	'' u ($1):($9+$11 +$12+$13) w filledcurve x1 fs pattern 0 lc 4  t 'in out new', 	'' u ($1):($11 +$12+$13) w filledcurve x1 fs pattern 0 lc 3  t 'in out known', 	'' u ($1):($12+$13) w filledcurve x1 fs pattern 0 lc 2  t 'out in new', 	'' u ($1):($13) w filledcurve x1 fs pattern 0 lc 1  t 'out in known', 	'' u ($1):($5) w l  t 'control'

## Strange BIFLOWS
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_var_0.05_stab_9_vts_0.0/connection_start_filter/plot/biflow_errors.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'flows'

plot 	'/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_var_0.05_stab_9_vts_0.0/connection_start_filter/csv/stat.csv' u ($1):($10) w filledcurve x1 fs pattern 0 lc 6  t 'bpp'

