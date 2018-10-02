#include "WireCellUtil/FFTBestLength.h"

#include <boost/assign/std/vector.hpp>
#include <iostream>

using namespace boost::assign;
using namespace WireCell;

// flag = 0, does not care if window_length is odd or even
// flag = 1, if the window_length is odd, the returned value will be odd
//           if the window_length is even, the returned value will be even

std::size_t WireCell::fft_best_length(std::size_t window_length,
                                      bool keep_odd_even)
{

  std::vector<std::size_t> edges;
  if (!keep_odd_even) {
    edges += 33, 35, 40, 42, 44, 56, 64, 66, 70, 77, 80, 96, 98, 100, 112, 121, 128, 140, 143, 160, 168, 169, 200, 224, 256, 280, 320, 400, 448, 512, 560, 640, 686, 768, 800, 847, 896, 1024, 1120, 1280, 1331, 1372, 1400, 1536, 1600, 1792, 2048, 2560, 2662, 2744, 3200, 4096, 4116, 4480, 4802, 5120, 5324, 5376, 5544, 5600, 5632, 5929, 6174, 6272, 6292, 6655, 7168, 7840, 8192, 8232, 8960, 9317, 10240, 10648, 11979, 14336, 15972, 16384; 
  }else{
    if (window_length%2==0){
      // even
      edges += 33, 77, 128, 130, 200, 256, 280, 320, 400, 448, 512, 560, 605, 686, 768, 800, 847, 882, 960, 980, 1280, 1331, 1372, 1536, 1600, 1792, 2048, 2560, 2662, 2744, 3200, 4096, 4116, 4480, 4802, 5120, 5324, 5544, 5600, 5632, 6174, 6272, 6468, 7168, 7840, 8192, 8232, 8960, 9317, 10240, 10648, 11979, 14336, 15972, 16000, 16384;

    }else{
      // odd
      edges += 33, 35, 39, 49, 55, 63, 65, 77, 81, 91, 99, 121, 143, 169, 175, 187, 225, 231, 245, 275, 297, 343, 363, 375, 385, 405, 441, 455, 495, 539, 605, 637, 693, 735, 847, 875, 891, 945, 1001, 1089, 1331, 1573, 1859, 1875, 2197, 2299, 2431, 2541, 2625, 2695, 3025, 3267, 3993, 4095, 4125, 4719, 4725, 5005, 5145, 5445, 5929, 6655, 7007, 7623, 7865, 9317, 9801, 11979, 14157, 14175, 15379, 16335;
    }
  }

  // std::cout << flag << " " << window_length << " " << edges.back() << std::endl;
  
  if (window_length <= edges.back()){
    for (auto it = edges.begin(); it!=edges.end(); it++){
      if ((*it) >= window_length)
	return *it;
    }
  }
  
  return window_length;
}
