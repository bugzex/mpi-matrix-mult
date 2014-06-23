mpicc matmult.c -o matmult
export PATH=.:$PATH
mpiexec -n 10 matmult