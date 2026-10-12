#!/usr/bin/env sh
clang -g ./minifb/build/libminifb.a ./art.c -o art.run -framework Metal -framework AppKit -framework MetalKit
