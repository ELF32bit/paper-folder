#!/bin/bash
shopt -s globstar nullglob
cd "$(dirname "$(readlink -fm "$0")")"

source "assets.sh"
for file in "$ASSETS"/**/*.svg; do
	$EXE from-svg $file -o "$(output "$file")" "$@"
done
