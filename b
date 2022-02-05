#!/bin/bash
set -euxo pipefail

OUT=${OUT:-build}

[[ "$#" -ge 1 ]] && [[ "$1" == "r" ]] && {
	# Rebuild CMake files
	rm -rf "./$OUT"
	mkdir "$OUT"
	cd "$OUT" && cmake ..
	cd ..
}

cd "$OUT" && make -j
