#!/bin/bash

echo "começar a limpar..."

# 1. Apagar os executáveis (os nomes que usamos anteriormente)
rm -f myProg
rm -f a.out

# 2. Apagar a pasta de resultados gerada pelo run_all_tests
if [ -d "results_output" ]; then
    rm -rf results_output
    echo "dir 'results_output' removida."
fi

# 3. Apagar ficheiros CSV soltos que foram gerados por erro na raiz
# Isso limpa os ficheiros com nomes errados tipo "output_dataset3.csv.csv"
rm -f *.csv.csv
rm -f output_*.csv
rm -f output_dataset*

# 4. Apagar pastas de build se usaste CMake em algum momento
if [ -d "build" ]; then
    rm -rf build
    echo "Pasta 'build' removida."
fi

echo "testes apagados com sucesso!"