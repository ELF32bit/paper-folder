#!/bin/bash
shopt -s globstar nullglob
cd "$(dirname "$(readlink -fm "$0")")"

source "assets.sh"
for file in "$ASSETS"/**/*.fold; do
	$EXE to-svg $file -o "$(output "$file" "svg")" "$@"
done
