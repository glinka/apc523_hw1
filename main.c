#include "mpi.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <time.h>
int in_set(const double *x, const double *y, const int *max_iter) {
  double complex pt = *x + (*y)*I;
  double complex z = 0;
  for(int i = 0; i < (*max_iter); i++) {
    z = z*z + pt;
  }
  if(cabs(z) > 2) {
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  /**
     instead of properly parsing argumnets, let's go with
     command x_min y_min x_max y_max n_xpts n_ypts max_iter nthreads
  **/
    const int STOP = 14587312;
  const double x_min = atof(argv[1]);
  const double y_min = atof(argv[2]);
  const double x_max = atof(argv[3]);
  const double y_max = atof(argv[4]);
  const int n_xpts = atoi(argv[5]);
  const int n_ypts = atoi(argv[6]);
  const int npts = n_xpts*n_ypts;
  const int max_iter = atoi(argv[7]);
  const int nthreads = atoi(argv[8]);
  int ntasks, rank;
  clock_t start = clock();
  int i, j;
  int **values;
  int mpi_check = MPI_Init(&argc, &argv);
  if(mpi_check != MPI_SUCCESS) {
      printf("MPI initialization failed");
      return 1;
      MPI_Abort(MPI_COMM_WORLD, mpi_check);
  }
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Status status;
  if(rank == 0) {
      double x_loc = x_min;
      double y_loc = y_max;
      double x_inc = (x_max-x_min)/(n_xpts-1);
      double y_inc = (y_max-y_min)/(n_ypts-1);
      int init_count = 0;
      int outcome;
      values = (int**) calloc(n_ypts, sizeof(int*));
      for(i = 0; i < n_ypts; i++) {
	  values[i] = (int *) calloc(n_xpts , sizeof(int));
      }
      for(i = 0; i < n_ypts; i++) {
	  x_loc = x_min;
	  for(j = 0; j < n_xpts; j++) {
	      if(init_count < ntasks-1) {
		  printf("send: %i\n", j);
		  int xy[2] = {x_loc, y_loc};
		  MPI_Send(&xy, 2, MPI_INT, init_count+1, 0, MPI_COMM_WORLD);
		  init_count++;
		  x_loc += x_inc;
	      }
	      else {
		  printf("send: %i\n", j);
		  MPI_Recv(&outcome, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		  x_loc += x_inc;
		  int xy[2] = {x_loc, y_loc};
		  MPI_Send(xy, 2, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
	      }
	      values[i][j] = outcome;
	  }
	  y_loc -= y_inc;
      }
      int xy[2] = {STOP, STOP};
      for(int thread = 0; thread < ntasks-1; thread++) {
	  MPI_Send(xy, 2, MPI_INT, thread+1, 0, MPI_COMM_WORLD);
      }
  }
  else {
      int xy[2];
      int count = 0;
      MPI_Recv(xy, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      printf("rcvd\n");
      while(xy[0] != STOP) {
	  int outcome = in_set(&xy[0], &xy[1], &max_iter);
	  MPI_Send(&outcome, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	  printf("worker sent\n");
	  MPI_Recv(xy, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	  printf("worker rcvd, loopiter = %i\n", count++);
      }
  }
  MPI_Finalize();
  printf("finished: %i\n", rank);
  clock_t end = clock();
  double elapsed_time = (end-start)/CLOCKS_PER_SEC;
  /**
  char mandset_outputfilename[1024];
  snprintf(mandset_outputfilename, sizeof mandset_outputfilename, "%s%d%s%d%s%d%s", "./data/omp_mandelbrodt_numthreads_", nthreads, "_nnodes_", n_xpts*n_ypts, "_maxiter_", max_iter , ".csv");
  FILE *mandsetvals_file = fopen(mandset_outputfilename, "w");
  for(i = 0; i < n_ypts; i++) {
    for(j = 0; j < n_xpts; j++) {
	   fprintf(mandsetvals_file, "%d", values[i][j]);
	   if(j != n_xpts-1) {
	   fprintf(mandsetvals_file, ",");
      }
    }
    fprintf(mandsetvals_file, "\n");
  }
  fclose(mandsetvals_file);
  char times_outputfilename[1024];
  snprintf(times_outputfilename, sizeof times_outputfilename, "%s%s", "./data/omp_mandelbrodt_times", ".csv");
  FILE *times_file = fopen(times_outputfilename, "a");
  fprintf(times_file, "%-7d\t%-8d\t%-2d\t%f\n", n_xpts, max_iter, nthreads, elapsed_time);
  fclose(times_file);
  for(i = 0; i < n_ypts; i++) {
    free(values[i]);
  }
  free(values);
  **/
  return 0;
}
