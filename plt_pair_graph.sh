# step_vs_pair_ration.gp
set encoding utf8
set terminal pngcairo size 1000,700 font "Meiryo,16"
set output "step_vs_pair_ration.png"
set title "Step と Pair Ration の関係"
set xlabel "Step" font "Meiryo,18"
set ylabel "Pair Ration" font "Meiryo,18"
set grid
set key left bottom
plot "data.txt" using 1:3 with linespoints lw 2 pt 5 ps 1.5 lc rgb "#56b4e9" title "Pair Ration"
unset output