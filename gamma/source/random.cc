// random.cc

#include "gamma.h"

static int next = 1;
void random_seed(int s) {
   next = s;
}

namespace gamma {
   namespace random {
      float normalized() {
         next = next * 1103515245 + 12345;
         return ((unsigned int)(next / 65536) % 32768) / 32768.0f;
      }

      float range(float min, float max) {
         return min + (max - min) * normalized();
      }
   } // !random
} // !gamma
