#!/bin/bash

set -o pipefail
cd build && cmake .. && make && cd ../ && ./build/PacManGame



