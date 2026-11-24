#!/bin/bash
shopt -s globstar nullglob
cd "$(dirname "$(readlink -fm "$0")")"

source "assets.sh"
for file in "$ASSETS"/**/*.obj; do
	$EXE from-obj $file -o "$(output "$file")" "$@"
done
