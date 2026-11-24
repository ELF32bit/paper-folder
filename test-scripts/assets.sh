#!/bin/bash
cd "../"

ASSETS="assets"
EXE="./paper-folder"

OUTPUT="test-output"
mkdir -p "$OUTPUT"

output() {
	local file="$1"
	local extension="${2:-fold}"
	local output_file="${file#"$ASSETS"/}"
	output_file="${output_file//\//_}"
	echo "$OUTPUT/${output_file%.*}.${extension}"
}
