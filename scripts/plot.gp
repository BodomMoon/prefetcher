set terminal 'png'
set title 'PFDIST'
set datafile separator " "
set output 'time.png'
set ylabel 'time(sec)'
plot "DistanceAVG.txt" using 1:2 with lines title 'PFDIST'
