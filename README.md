
[![Build Status](https://travis-ci.org/hpicgs/bhtsne.svg)](https://travis-ci.org/hpicgs/bhtsne)
[![Build Status](https://ci.appveyor.com/api/projects/status/1d4q1pd8d3h6rpoe?svg=true)](https://ci.appveyor.com/project/chistopher/bhtsne)


This software package contains a Barnes-Hut implementation of the t-SNE algorithm as a C++ library and CLI. The implementation is described in [this paper](http://lvdmaaten.github.io/publications/papers/JMLR_2014.pdf).


# Requirements #
- C++14
- openMP (supported on all major plattforms including Solaris, AIX, HP-UX, Linux, macOS, and Windows)
- CPU with AVX2 (first supported by Intel with the Knights Landing architecture)

# Installation #

This project uses the [cmake-init](https://github.com/cginternals/cmake-init) template.

For MacOS and Linux:
```
./configure
cmake --build build
```
For Windows we recommend the cmake GUI to create a Visual Studio solution.

# C++ Library Usage Example#
```
#include <bhtsne/TSNE.h>

int main(int argc, char * argv[])
{
  bhtsne::TSNE alg;
  alg.loadCSV("..\data\small.csv");
  alg.setPerplexity(0.2);
  alg.run();
  tsne.saveSVG();
  tsne.saveCSV();
  return 0;
}
```
For a more information please refer to our doxygen documentation. The documentation can be enabled with the OPTION_BUILD_DOCS cmake option.

# CLI Usage Examples #
```
./bhtsne_cmd --help
./bhtsne_cmd --version
./bhtsne_cmd --iterations 1000 --random-seed 42 -svg ~/data.dat
cat data.csv | ./bhtsne_cmd -legacy
./bhtsne_cmd -stdout -csv -legacy -svg ~/small.csv
```
