#!/bin/bash -a

for i in E2670 PowerA2 i5
do
    cp plot.txt data_core_${i}
    cd data_core_${i}
    case $i in
        E2670)
        sed -i -e "s/NAME/${i}-2.6[GHz]-GCC5.1/g" plot.txt
        sed -i -e "s/FREQ/2.6*10**-9/g" plot.txt
        ;;
        PowerA2)
        sed -i -e "s/NAME/${i}-1.7[GHz]-GCC4.7/g" plot.txt
        sed -i -e "s/FREQ/1.7*10**-9/g" plot.txt
        ;;
        i5)
        sed -i -e "s/NAME/${i}-2.5[GHz]-CLANG3.6/g" plot.txt
        sed -i -e "s/FREQ/2.5*10**-9/g" plot.txt
        ;;
    esac
    gnuplot plot.txt
    open *.eps
    cd ..
done
