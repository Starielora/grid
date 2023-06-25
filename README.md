# grid
I wanted to check one thing in fragment shader and ended up with this...

The grid has reduced moire patters, tho!

![](screenshot.png)

# Build
```powershell
git submodule update --init --recursive
mkdir build
cd build
cmake ..
cmake --build . --config Debug|Release
```
