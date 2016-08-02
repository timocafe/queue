#!/bin/bash -a

for i in E2670 PowerA2 i5
do
    cp plot.txt data_core_${i}
    cd data_core_${i}
    rm *.eps *.pdf
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
    epstopdf mh.eps --outfile mh_$i.pdf
    epstopdf pop.eps --outfile pop_$i.pdf
    epstopdf push.eps --outfile push_$i.pdf
    epstopdf single.eps --outfile single_$i.pdf
    gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=merged_$i.pdf mh_$i.pdf pop_$i.pdf push_$i.pdf single_$i.pdf
    open merged_$i.pdf
    cd ..
done
