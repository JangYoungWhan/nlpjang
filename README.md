# nlpjang
[![Build Status](https://travis-ci.org/JangYoungWhan/nlpjang.svg?branch=master)](https://travis-ci.org/JangYoungWhan/nlpjang)

nlpjang provides a set of C++ natural language processing tools.
It supports multi-platforms and the goal is to make it easy to use NLP algorithms.

##Prerequisites

- GCC version 4.8 or greater.
- MSVC version VC110(2012) or greater.
- [CMake](http://www.cmake.org/) version 3.0 or greater.

##Submodules

- [garnut](https://github.com/JangYoungWhan/garnut)

##Getting Started

```bash
# Get the latest repository.
git clone https://github.com/JangYoungWhan/nlpjang.git nlpjang

# Move to the repository
cd nlpjang

# Download submodules
git submodule init
git submodule update

# Move and create a workspace
mkdir build
cd build

# Configure and build
cmake ../
make

```

##Contact

[stdjangyoungwhan@gmail.com](https://github.com/JangYoungWhan)
