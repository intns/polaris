#include <image/PPM.hpp>
#include <iostream>

int main(int argc, char **argv) {
  using namespace polaris;

  image::PPM img{255, 255};

  std::ofstream f{"output.ppm"};

  if (f.is_open()) {
    img.Write(f);
  }

  // TODO(intns): main logic loop
  return 0;
}
