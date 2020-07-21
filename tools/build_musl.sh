#!/usr/bin/env bash

# TODO: Check if -d or --debug flag is supplied
# TODO: Add -j override

mkdir -p toolchain

pushd toolchain/epic-musl

if [ $1 == "clean" ]; then
  make clean

  popd
  exit
fi

thread_count=$(grep -c ^processor /proc/cpuinfo)
job_count=$(expr $thread_count / 2)

# TODO: Add --enable-debug if debug flag is passed
./configure CROSS_COMPILE=i686-linux-gnu- --prefix=$(pwd)/build/release

make -j$job_count
make install -j$job_count

popd

# TODO: Change this path if debug flag is passed
ln -fs $(pwd)/toolchain/epic-musl/build/release/bin/musl-gcc toolchain/i686-epic-musl-gcc