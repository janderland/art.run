#!/usr/bin/env sh
clang -g -I./minifb/include ./minifb/build/libminifb.a ./art.c -o art.run -framework Metal -framework AppKit -framework MetalKit
