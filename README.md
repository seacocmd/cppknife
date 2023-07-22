# cppknife
A C++ library and command line tools.

## Installation
If you want to use the library you can install it under Linux: see linux/install.sh

## Applications

- [dbknife](doc/dbknife.md)
- [fileknife](doc/fileknife.md)
- [geoknife](doc/geoknife.md)
- [sesknife](doc/sesknife.md)
- [textknife](doc/textknife.md)

## Binaries
- Binaries for the architecture amd64: see releases/amd64

## Build
Run the script **CreateRelease**

## Starting from Scratch
```
apt install libgtest-dev cmake libgdal-dev
cmake -H. -Bbuild
cmake -Dmode=productive -DCMAKE_BUILD_TYPE=Release -S . -B $BASE_BUILD
cmake --build build.release
```


