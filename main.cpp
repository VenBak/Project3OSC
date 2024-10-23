/**
  * Assignment: memory
  * Operating Systems
  */

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
const int64_t SIZE = 10ULL;
const int64_t REPEAT = 1ULL;

int main(int argc, char* argv[]) {
  float* img = new float[SIZE * SIZE];

  // fill with dummy data
  for (int64_t i = 0; i < SIZE; i++) {
    for (int64_t j = 0; j < SIZE; j++) {
      img[j * SIZE + i] = (2 * j + i) % 32768;
      cout << "Array contents:" << img[j * SIZE + i] << " at index:" << j* SIZE + i << endl;
    }
  }

  // this dummy value is needed to avoid compilers eliminating the loop as part of a optimisation
  uint64_t dummy = 0;

  // ADJUST BELOW, BUT keep writing to the dummy variable
  float* res = new float[SIZE * SIZE];  // result of mean filter
  float cache;

  // Apply an averaging imaging filter to some input image, and write in to an output image.
  // A pixel in the output image is calculated by averaging 9 pixels: the pixel at the same
  // coordinates in the input image, and the adjecent pixels.
  for (int64_t r = 0; r < 1; ++r) {
    for (int64_t i = 1; i < 5 - 1; i++) {
      for (int64_t j = 1; j < 5 - 1; j++) {
        // Set all values of the res array to 0 initialized
        // res[j * SIZE + i] = 0;
        if (i*j==1) {
          for (long k = -1; k < 2; k++) {
            for (long l = -1; l < 2; l++) {
              res[j * SIZE + i] += img[(j + l) * SIZE + i + k];
              cout << "Added: " << img[(j + l) * SIZE + i + k] << " to " << res[j * SIZE + i] << " at " << j* SIZE+i << endl;
            }
          }
          float nuber =res[j * SIZE + i] /= 9;
          cout << "First averaging value: " << nuber << endl;
          cout << "Index of "<< j*i <<  " averaging: " << j * SIZE + i << endl;
        } else if (j * SIZE + i - SIZE > 0){
          cache = res[j * SIZE + i - SIZE] * 9;
          cout << "New index: " << j * SIZE + i << endl;
          cout << "Previous average: " << cache  << " index: " << j*SIZE +i - SIZE << endl;
          cout << "New: " << res[j * SIZE + i] << endl;
          for (long k = -1; k < 2; k++) {

            cache -= img[(j-2 ) * SIZE + i + k];
            cout << "removed: " << img[(j-2 ) * SIZE + i + k] << " at index: " << (j-2 ) * SIZE + i + k << endl;

            if ((j+1) * SIZE + i + k != SIZE * SIZE - 1) {
              cache += img[(j+1) * SIZE + i + k];
              cout << "added: " << img[(j-2 ) * SIZE + i + k] << " at index " << (j+1) * SIZE + i + k << endl;
            }
          }
          res[j * SIZE + i] = cache / 9;
        }
      }
    }

    cout << "\n Default functionality: \n" << endl;
    for (int64_t i = 1; i < 5 - 1; i++) {
      for (int64_t j = 1; j < 5 - 1; j++) {
        // Set all values of the res array to 0 initialized
        res[j * SIZE + i] = 0;
        
        for (long k = -1; k < 2; k++) {
          for (long l = -1; l < 2; l++) {
            res[j * SIZE + i] += img[(j + l) * SIZE + i + k];
            cout << " (img) added: " << img[(j + l) * SIZE + i + k] << " at index "<< (j + l) * SIZE + i + k;
            cout << res[j * SIZE + i] << " at index: " << j * SIZE + i << endl;
          }
        }
        res[j * SIZE + i] /= 9;
        cout << "------------------------------" << endl;
      }
    }

    cout << "\n Semi-Default functionality: \n" << endl;
    for (int64_t i = 1; i < 5 - 1; i++) {
      for (int64_t j = 1; j < 5 - 1; j++) {
        // Set all values of the res array to 0 initialized
        res[j * SIZE + i] = 0;
        // First average
        if (i* j == 1) {
          for (long k = -1; k < 2; k++) {
            for (long l = -1; l < 2; l++) {
              res[j * SIZE + i] += img[(j + l) * SIZE + i + k];
              cout << " (img) added: " << img[(j + l) * SIZE + i + k] << " at index "<< (j + l) * SIZE + i + k;
              cout << res[j * SIZE + i] << " at index: " << j * SIZE + i << endl;
            }
          }
        } else if (j * SIZE + i - SIZE > 0) {
          cout << j * SIZE + i - SIZE << endl;
          for (long k = -1; k < 2; k++) {
            for (long l = -1; l < 2; l++) {
              res[j * SIZE + i] += img[(j + l) * SIZE + i + k];
              cout << " (img) added: " << img[(j + l) * SIZE + i + k] << " at index "<< (j + l) * SIZE + i + k;
              cout << res[j * SIZE + i] << " at index: " << j * SIZE + i << endl;
            }
          }
        }
      
        res[j * SIZE + i] /= 9;
        cout << "------------------------------" << endl;
      }
    }

    for (int64_t i = 1; i < SIZE - 1; i++) {
      for (int64_t j = 1; j < SIZE - 1; j++) {
        dummy += res[j * SIZE + i];
      }
    }
  }

  delete[] res;

  // ADJUST ABOVE, BUT keep writing to the dummy variable
  delete[] img;

  struct rusage usage;
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
