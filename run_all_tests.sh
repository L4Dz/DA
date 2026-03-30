#!/bin/bash

g++ -std=c++17 main.cpp -I. -o myProg

if [ $? -ne 0 ]; then
    echo "Erro na compilação! Verifica os teus #includes."
    exit 1
fi

mkdir -p results_output

for input_file in dataset/input/*.csv; do
    base_name=$(basename "$input_file" .csv)
    
    echo "Testing $input_file..."
    
    ./myProg -b "$input_file" "results_output/output_${base_name}.csv"
done

echo "All tests complete. Check the 'results_output' folder."