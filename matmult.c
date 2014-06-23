// matmult.c - mult two matrixes using MPI

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void print(int **arr, unsigned size);
void push(int **arr, unsigned size);

int main(int argc, char *argv[])
{
  int **A, **B, **C;      // source and result matrix
  int *a, *c;             // need for slaves...
  unsigned m_size;        
  int size, rank;
  unsigned i, j;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  m_size = size - 1;
  if(rank == 0)     // master?
  {
    A = (int **)malloc(m_size*sizeof(int *));
    B = (int **)malloc(m_size*sizeof(int *));
    C = (int **)malloc(m_size*sizeof(int *));

    for(i = 0; i < m_size; i++)
    {
      A[i] = (int *)malloc(m_size*sizeof(int));
      B[i] = (int *)malloc(m_size*sizeof(int));
      C[i] = (int *)malloc(m_size*sizeof(int));
    }
    
    // pushing matrixes
    push(A, m_size);
    push(B, m_size);
    
    // show them
    puts("First matrix:");
    print(A, m_size);    
    puts("Second matrix:");
    print(B, m_size);         
  
    for(i = 0; i < m_size; i++)
    {
      MPI_Send(A[i], m_size, MPI_INTEGER4, i+1, 0, MPI_COMM_WORLD);
      for(j = 0; j < m_size; j++)
        MPI_Send(B[j], m_size, MPI_INTEGER4, i+1, 1, MPI_COMM_WORLD);
    }
    
    for(i = 0; i < m_size; i++)
      MPI_Recv(C[i], m_size, MPI_INTEGER4, i+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  
    puts("Result matrix:");
    print(C, m_size);         
    
    for(i = 0; i < m_size; i++)
    {
      free(A[i]);
      free(B[i]);
      free(C[i]);
    }
    
    free(A);
    free(B);
    free(C);
  }    
  else        // slave!
  {
    a = (int *)malloc(m_size*sizeof(int));
    B = (int **)malloc(m_size*sizeof(int *));
    c = (int *)malloc(m_size*sizeof(int));
    for(i = 0; i < m_size; i++)
      B[i] = (int *)malloc(m_size*sizeof(int));
    for(i = 0; i < m_size; i++)
      c[i] = 0;
    
    MPI_Recv(a, m_size, MPI_INTEGER4, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for(i = 0; i < m_size; i++)
      MPI_Recv(B[i], m_size, MPI_INTEGER4, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      
    for(i = 0; i < m_size; i++)
    {
      for(j = 0; j < m_size; j++)
        c[i]+= a[j] * B[i][j];
    }
    MPI_Send(c, m_size, MPI_INTEGER4, 0, 0, MPI_COMM_WORLD);
    
    for(i = 0; i < m_size; i++)
      free(B[i]);
      
    free(a);
    free(B);
    free(c);
  }
 
  MPI_Finalize();
  return 0;
}

void print(int **arr, unsigned size) 
{
  unsigned i, j;
  for(i = 0; i < size; i++)
  {
    for(j = 0; j < size; j++)
      printf("%4d ", arr[i][j]);
    puts("");
  }
  puts("");
  fflush(stdout);
}

void push(int **arr, unsigned size)
{
  unsigned i, j;
  for(i = 0; i < size; i++)
    for(j = 0; j < size; j++)
      arr[i][j] = rand() % size;
}