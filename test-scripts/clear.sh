#!/bin/bash
shopt -s globstar nullglob
cd "$(dirname "$(readlink -fm "$0")")"

source "assets.sh"
for file in "$ASSETS"/**/*.fold; do
	$EXE clear $file -o "$(output "$file")" "$@" \
		--VC --VE --VF \
		--EV --EF --EA --EFA --EL --EO \
		--FV --FE --FF --FO \
		--PC --TC --T2C --NC --MN \
		--VP --FT --FT2 --FN --FM
done
