#include <omp.h>
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
     command x_min y_min x_max y_max n_xpts n_ypts max_iter n_threads
  **/
  const double x_min = atof(argv[1]);
  const double y_min = atof(argv[2]);
  const double x_max = atof(argv[3]);
  const double y_max = atof(argv[4]);
  const int n_xpts = atoi(argv[5]);
  const int n_ypts = atoi(argv[6]);
  const int max_iter = atoi(argv[7]);
  const int n_threads = atoi(argv[8]);
  int **values = (int**) malloc(n_ypts*sizeof(int*));
  for(int i = 0; i < n_xpts; i++) {
    values[i] = (int *) malloc(n_xpts*sizeof(int));
  }
  double x_loc = x_min;
  double y_loc = y_max;
  double x_inc = (x_max-x_min)/(n_xpts-1);
  double y_inc = (y_max-y_min)/(n_ypts-1);
  int i, j;
  clock_t start = clock();
#pragma omp parallel default(shared) private(i, j) num_threads(n_threads)
  {
    for(i = 0; i < n_ypts; i++) {
      x_loc = x_min;
#pragma omp for schedule(dynamic)
      for(j = 0; j < n_xpts; j++) {
	values[i][j] = in_set(&x_loc, &y_loc, &max_iter);
	x_loc += x_inc;
      }
      y_loc -= y_inc;
    }
  }
  clock_t end = clock();
  double elapsed_time = (end-start)/CLOCKS_PER_SEC;

  char mandset_outputfilename[1024];
  snprintf(mandset_outputfilename, sizeof mandset_outputfilename, "%s%d%s", "omp_mandelbrodt_num_threads_", n_threads, ".csv");
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
  snprintf(times_outputfilename, sizeof times_outputfilename, "%s%s", "omp_mandelbrodt_times", ".csv");
  FILE *times_file = fopen(times_outputfilename, "a");
  fprintf(times_file, "%-7d\t%-8d\t%-2d\t%f\n", n_xpts, max_iter, n_threads, elapsed_time);
  fclose(times_file);

  for(i = 0; i < n_xpts; i++) {
    free(values[i]);
  }
  free(values);
  return 0;
}
