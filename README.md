[WIP] kde-auto-brightness
=========================

Auto brightness control for KDE.

Requirements
------------

 - iio-sensor-proxy
 - Qt5
 - KDE Frameworks 5
 - clang/gcc, CMake, etc.

Build
-----

```bash
mkdir build && pushd build
cmake .. -GNinja
ninja -j0
sudo ninja install
popd
```
