#!/bin/bash

# The script requires two arguments:
# 1. The path to the source code directory.
# 2. The path to the compile_commands.json file.
# The script also requires the jq tool to parse the compile_commands.json file.
# The script will fail if the compile_commands.json file or the cppcheck suppressions file is not found.

# Fail on any error
set -e

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <source_dir> <path/to/compile_commands.json>"
    exit 1
fi

SRC_DIR=$1
COMPILE_CMDS=$2

# Check if the compile_commands.json file exists
if [ ! -f $COMPILE_CMDS ]; then
    echo "Error: compile_commands.json file not found at $COMPILE_CMDS"
    exit 1
fi

SUPPRESSIONS_FILE=${SRC_DIR}/.cppcheck_suppressions

# Check if the suppressions file exists
if [ ! -f $SUPPRESSIONS_FILE ]; then
    echo "Error: cppcheck suppressions file not found at $SUPPRESSIONS_FILE"
    exit 1
fi

# TODO: This does not work unfortunately
# Get the include paths from the compile_commands.json file
# INCLUDE_PATHS=$(jq -r '.[] | .command' $COMPILE_CMDS | grep -oE '(-I[^ ]+)' | sort -u | sed 's/-I/--include=/g')

cppcheck --version

# Run cppcheck
cd $SRC_DIR
cppcheck \
    --check-config \
    --enable=all \
    --error-exitcode=1 \
    --suppressions-list=$SUPPRESSIONS_FILE \
    $INCLUDE_PATHS \
    modules unit_tests
