#!/bin/bash

for i in data_core_E2670 data_core_PowerA2 data_core_i5_macmini
do
    cp plot.txt ${i}
    cd ${i}
    gnuplot plot.txt
    cd ..
done
