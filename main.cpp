//Assignment: memory
//Operating Systems

// function/class definitions you are going to use
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

// although it is good habit, you don't have to type 'std::' before many objects by including this line
using namespace std;

// size of the image
const int64_t SIZE = 4096ULL;
const int64_t REPEAT = 20ULL;
const int RUN_ATTEMPT = 21;
const int PAGE_SIZE = 1024; //nr of floats or ints that fit into a page
const int SQR_PAGE_SIZE = 32; //1024 = 32^2
const int SUBMATRICES = (SIZE * SIZE) / PAGE_SIZE;

void fillSubMatrix(float array[SIZE * SIZE], int row_offset, int col_offset){
  for(int i = 0; i < SQR_PAGE_SIZE; i++){
    int cur_row = i * SIZE;
    for(int j = 0; j < SQR_PAGE_SIZE; j++){
      int new_index = row_offset + cur_row + col_offset + j;
      array[new_index] = (2 * (col_offset + j) + row_offset/SIZE + i) % 32768;
    }
  }
}

uint64_t SecuredFillSubMatrix(float new_array[SIZE * SIZE], float old_array[SIZE * SIZE], int row_offset, int col_offset, uint64_t sum){
  int start_row = 0;
  int end_row = SQR_PAGE_SIZE;
  int start_col = 0;
  int end_col = SQR_PAGE_SIZE;
  int max_row = SQR_PAGE_SIZE + row_offset;
  int max_col = SQR_PAGE_SIZE + col_offset;
  if(row_offset == 0){
    start_row = 1;
  }
  if(max_row == (SIZE - 1)){
    end_row--;
  }
  if(col_offset == 0){
    start_col = 1;
  }
  if(max_col == (SIZE - 1)){
    end_col--;
  }


  //First phase of computation
  for(int i = start_row; i < end_row; i++){
    int cur_row = i * SIZE;
    for(int j = start_col; j < end_col; j++){
      int new_index = row_offset + cur_row + col_offset + j;
      new_array[new_index] = 0;
      for (long l = -1; l < 2; l++) {
          for (long k = -1; k < 2; k++) {
            int old_index = (col_offset + j + l) * SIZE + row_offset/SIZE + i + k;
            if(old_index >= 0 && old_index < (SIZE * SIZE)){
              new_array[new_index] += old_array[old_index];
            }
          }
        }
        new_array[new_index] /= 9;
        sum += new_array[new_index];
    }
  }

  //Second phase of computation
  /*for(int i = start_row; i < end_row; i++){
    int cur_row = i * SIZE;
    for(int j = start_col; j < end_col; j++){
      sum += new_array[row_offset + cur_row + col_offset + j];
    }
  }*/

  return sum;
  
}



int main(int argc, char* argv[]) {
  float* img = new float[SIZE * SIZE];


  for(int m = 0; m < SUBMATRICES; m++){
    int GLOBAL_ROW_OFFSET = SIZE * (m / PAGE_SIZE);
    int GLOBAL_COL_OFFSET = SQR_PAGE_SIZE * (m % PAGE_SIZE);
    fillSubMatrix(img, GLOBAL_ROW_OFFSET, GLOBAL_COL_OFFSET);
    
  }

  /* fill with dummy data
  for (int64_t i = 0; i < SIZE; i++) {
    int cur_row = i * SIZE;
    for (int64_t j = 0; j < SIZE; j++) {
      img[cur_row + j] = (2 * j + i) % 32768;
    }
  }*/

  // this dummy value is needed to avoid compilers eliminating the loop as part of a optimisation
  uint64_t dummy = 0;

  // ADJUST BELOW, BUT keep writing to the dummy variable
  float* res = new float[SIZE * SIZE];  // result of mean filter

  for(int64_t r = 0; r < REPEAT; ++r) {
    for(int m = 0; m < SUBMATRICES; m++){
      int GLOBAL_ROW_OFFSET = SIZE * (m / PAGE_SIZE);
      int GLOBAL_COL_OFFSET = SQR_PAGE_SIZE * (m % PAGE_SIZE);
      uint64_t cur_sum = 0;
      dummy += SecuredFillSubMatrix(res, img, GLOBAL_ROW_OFFSET, GLOBAL_COL_OFFSET, cur_sum);
      if(dummy > 0){
        cout << "value now is: " << dummy << endl;
      }
    }
  }

  // Apply an averaging imaging filter to some input image, and write in to an output image.
  // A pixel in the output image is calculated by averaging 9 pixels: the pixel at the same
  // coordinates in the input image, and the adjecent pixels.
  /*for (int64_t r = 0; r < REPEAT; ++r) {
    for (int64_t i = 1; i < SIZE - 1; i++) {
      int cur_row = i * SIZE;
      for (int64_t j = 1; j < SIZE - 1; j++) {
        res[cur_row + j] = 0;
        for (long l = -1; l < 2; l++) {
          for (long k = -1; k < 2; k++) {
            res[cur_row + j] += img[(j + l) * SIZE + i + k];
          }
        }
        res[cur_row + j] /= 9;
      }
    }

    for (int64_t i = 1; i < SIZE - 1; i++) {
      int cur_row = i * SIZE;
      for (int64_t j = 1; j < SIZE - 1; j++) {
        dummy += res[cur_row + j];
      }
    }
  }*/

  delete[] res;

  // ADJUST ABOVE, BUT keep writing to the dummy variable
  delete[] img;

  struct rusage usage;
  std::cout << "run:                          " << RUN_ATTEMPT << endl;
  std::cout << "SIZE:                         " << SIZE << endl;
  std::cout << "REPEAT:                       " << REPEAT << endl;
  getrusage(RUSAGE_SELF, &usage);

  std::cout << "user time:                    " << usage.ru_utime.tv_sec << "." << std::fixed << std::setw(6) << std::setprecision(6) << std::setfill('0') << usage.ru_utime.tv_usec << " s" << std::endl;
  std::cout << "soft page faults:             " << usage.ru_minflt << std::endl;
  std::cout << "hard page faults:             " << usage.ru_majflt << std::endl;
#ifdef __APPLE__
  std::cout << "max memory:                   " << usage.ru_maxrss/1024 << " KiB" << std::endl;
#else
  std::cout << "max memory:                   " << usage.ru_maxrss << " KiB" << std::endl;
#endif
  std::cout << "voluntary context switches:   " << usage.ru_nvcsw << std::endl;
  std::cout << "involuntary context switches: " << usage.ru_nivcsw << std::endl;
  std::cout << "dummy value (ignore):         " << dummy << std::endl; // this value is printed to avoid optimisations
  std::cout << "typical page size:            " << sysconf(_SC_PAGESIZE) << std::endl;
  // ...
  return 0;
}
