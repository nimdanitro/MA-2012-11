

### STAT index MAP
#  1        2                  3           4         5           6          
# time_s, duration_add, duration_prune, flows_in, flows_valid, biflows_new,
#  7                8                   7             8        9     
# biflows_cached, prune_too_young, prune_exported, elements, buckets, 
#    10         11              12          13
# pool_total, pool_available, pool_new, pool_deleted

## FLOWS
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_var_0.1_stab_9_vts_2/biflow_cache/plot/incomming_flows.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'flows'

plot 	'/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_var_0.1_stab_9_vts_2/biflow_cache/csv/stat.csv' u ($1):($4) w filledcurve x1 fs pattern 0 lc 2  t 'flows in', 	'' u ($1):($5) w filledcurve x1 fs pattern 0 lc 1  t 'valid'

## BiFlows
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_var_0.1_stab_9_vts_2/biflow_cache/plot/biflows_matching.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'flows'

plot 	'/largefs3/asdaniel/events/2010_05_18/fact/eval/Set_var_0.1_stab_9_vts_2/biflow_cache/csv/stat.csv' u ($1):($6+$7) w filledcurve x1 fs pattern 0 lc 2  t 'new', 	'' u ($1):($7) w filledcurve x1 fs pattern 0 lc 1  t 'cached',   '' u ($1):($4) w l t 'control: flows in'



