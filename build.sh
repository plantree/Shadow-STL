#!/bin/bash

set -x

cmake . -B build

cd build
make
