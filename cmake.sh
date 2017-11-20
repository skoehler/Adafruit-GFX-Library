#!/bin/sh

( mkdir -p build_deb && cd build_deb && cmake -DCMAKE_BUILD_TYPE=Debug .. )
( mkdir -p build_rel && cd build_rel && cmake -DCMAKE_BUILD_TYPE=Release .. )

make -C build_deb
make -C build_rel
