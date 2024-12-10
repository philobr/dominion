#!/bin/bash

# Fail on error
set -e

INPUT_DIR="assets/color"
OUTPUT_DIR="assets/black_and_white"

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

# Process each image
for infile in "$INPUT_DIR"/*.{png,jpg,jpeg}; do
    # Skip if no files match
    [ -e "$infile" ] || continue

    filename=$(basename "$infile")
    outfile="$OUTPUT_DIR/$filename"

    # Skip if output file already exists
    [ -e "$outfile" ] && continue

    magick "$infile" -colorspace Gray "$outfile"
    echo "Converted $infile to $outfile" >&2
done
