#include <stdio.h>
#include <time.h>

int main() {
  clock_t start = clock();
  double seconds = 0.0;

  while (seconds < 20.0) {
    for (long i = 0; i < 10000000; i++); // Consome CPU
    seconds = (double) (clock() - start) / CLOCKS_PER_SEC;
  }
  return 0;
}
