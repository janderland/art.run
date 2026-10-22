mkdir -p bin
cd bin || exit 1
clang -std=c23 ../art.c -o art.run \
  ../minifb/build/libminifb.a \
  -framework CoreFoundation \
  -framework CoreGraphics \
  -framework Foundation \
  -framework AppKit \
  -framework Carbon \
  -framework MetalKit \
  -framework Metal \
  -lc++ \
  || exit 1
./art.run
