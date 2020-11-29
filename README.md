# Parallel-Processing-Assignment-2

I've coded 3 different modes:

- Sequential mode           -> sequential.c
- Parallel mode (Option 1)  -> parallel_v1.c - Uses OpenMP
- Parallel mode (Option 2)  -> parallel_v2.c - Uses OpenMP

The results of my experiments are:
      
    N = 20 * 20
    verbose = false, print_results = false
    
    ITERATION_COUNT = 10
    Sequential - Elapsed time : 0.000131
    Parallel mode (Option 1) - Elapsed time : 0.000630
    Parallel mode (Option 2) - Elapsed time : 0
    
    ITERATION_COUNT = 100
    Sequential - Elapsed time : 0.001228
    Parallel mode (Option 1) - Elapsed time : 0.004359
    Parallel mode (Option 2) - Elapsed time : 0
        
    ITERATION_COUNT = 1000
    Sequential - Elapsed time : 0.026524
    Parallel mode (Option 1) - Elapsed time : 0.052764
    Parallel mode (Option 2) - Elapsed time : 0
    
    -------
    
    N = 1000 * 1000
    verbose = false, print_results = false
    
    ITERATION_COUNT = 10
    Sequential - Elapsed time : 0.286908
    Parallel mode (Option 1) - Elapsed time : 0.117272
    Parallel mode (Option 2) - Elapsed time : 0
    
    ITERATION_COUNT = 100
    Sequential - Elapsed time : 2.914351
    Parallel mode (Option 1) - Elapsed time : 0.900778
    Parallel mode (Option 2) - Elapsed time : 0
        
    ITERATION_COUNT = 1000
    Sequential - Elapsed time : 29.913363
    Parallel mode (Option 1) - Elapsed time : 9.116429
    Parallel mode (Option 2) - Elapsed time : 0

For small N value, sequential mode has better results for timing metric.
BUT when N=1000, parallel modes improves the performance.
Parallel_v1 has the best timing for large iteration counts.

For execution, please run:

    .\playGame.sh

The above sh file is for MacOS.
For Unix environments, you can use:

    #!/bin/bash
    gcc-10 -o playGame -fopenmp main.c
    setenv OMP_NUM_THREADS=4
    ./playGame