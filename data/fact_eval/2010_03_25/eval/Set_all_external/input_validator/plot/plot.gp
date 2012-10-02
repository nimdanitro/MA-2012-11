

#  1        2           3      4                   5                  6         7          8
# time_s, duration_s, flows, iv_time_freshness, iv_time_causality, iv_bytes, iv_packets, iv_mtu

## FLOWS

reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_03_25/fact/eval/Set_all_external/input_validator/plot/flows.eps'

set nokey
set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'flows'
plot '/largefs3/asdaniel/events/2010_03_25/fact/eval/Set_all_external/input_validator/csv/stat.csv' u 1:3 w l t 'flows'

## ERRORS
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_03_25/fact/eval/Set_all_external/input_validator/plot/iv_errors.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set yrange[1:10000]
set log y
set ylabel 'errors'
plot   '/largefs3/asdaniel/events/2010_03_25/fact/eval/Set_all_external/input_validator/csv/stat.csv' u 1:4 w p t 'iv time freshness',  '/largefs3/asdaniel/events/2010_03_25/fact/eval/Set_all_external/input_validator/csv/stat.csv' u 1:5 w p t 'iv time causality',  '/largefs3/asdaniel/events/2010_03_25/fact/eval/Set_all_external/input_validator/csv/stat.csv' u 1:6 w p t 'iv time causality',  '/largefs3/asdaniel/events/2010_03_25/fact/eval/Set_all_external/input_validator/csv/stat.csv' u 1:7 w p t 'iv bytes',  '/largefs3/asdaniel/events/2010_03_25/fact/eval/Set_all_external/input_validator/csv/stat.csv' u 1:8 w p t 'iv mtu'
