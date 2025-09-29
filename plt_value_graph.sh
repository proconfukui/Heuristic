# step_vs_value.gp
set encoding utf8
set terminal pngcairo size 1000,700 font "Meiryo,16"
set output "step_vs_value.png"
set title "Step と Value の関係"
set xlabel "Step" font "Meiryo,18"
set ylabel "Value" font "Meiryo,18"
set grid
set key right top
plot "data.txt" using 1:2 with linespoints lw 2 pt 7 ps 1.5 lc rgb "#0072B2" title "Value"
unset output