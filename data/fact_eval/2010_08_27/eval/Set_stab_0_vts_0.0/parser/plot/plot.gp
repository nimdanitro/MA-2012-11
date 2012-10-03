

## SPEED
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/plot/speed.eps'

set nokey
set xlabel 'processing time for 5 min flow trace'
set ylabel 'occurrence'

binwidth=5
bin(x,width)=width*floor(x/width) + binwidth/2.0
plot '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/csv/stat.csv' using (bin($4,binwidth)):(1.0) smooth freq with boxes

## # UDP PACKETS
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/plot/packets.eps'

set nokey
set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set ylabel 'udp netflow packets'
plot '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/csv/stat.csv' u 1:5 w l t 'all'

## ERRORS
reset
set size 1,1
set terminal postscript eps enhanced color
set output '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/plot/iv_errors.eps'

set timefmt '%s'
set xdata time
set xlabel ''
set ytics nomirror
set yrange[1:10000]
set log y
set ylabel 'errors'
plot   '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/csv/stat.csv' u 1:6 w p t 'iv min size',  '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/csv/stat.csv' u 1:7 w p t 'iv format',  '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/csv/stat.csv' u 1:8 w p t 'iv exporter',  '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/csv/stat.csv' u 1:9 w p t 'iv export time too small',  '/largefs3/asdaniel/events/2010_08_27/fact/eval/Set_stab_0_vts_0.0/parser/csv/stat.csv' u 1:10 w p t 'iv export time too big'
