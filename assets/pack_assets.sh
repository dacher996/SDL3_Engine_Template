#!/bin/bash

cd "$(dirname "$0")"

rm -rf output
mkdir output

# Pack sprite atlases
./sprites/pack_atlases.sh
cp -r ./sprites/output/. ./output/atlases/

# Compile shaders
./shaders/compile.sh
cp -r ./shaders/compiled/. ./output/shaders/

# Remove hidden macOS related files
rm -f .DS_Store