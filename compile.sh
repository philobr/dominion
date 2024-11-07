#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Get the directory where the script is located (project root)
PROJECT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="${PROJECT_ROOT}/build"

# Store current directory to return to it later
ORIGINAL_DIR="$(pwd)"

# Function to show progress bar
show_progress() {
    local pid=$1
    local task=$2
    local bar_length=30
    
    printf "%-20s [" "$task"
    
    # Fill with spaces initially
    for ((i=0; i<bar_length; i++)); do
        printf " "
    done
    printf "]"
    
    local i=0
    while kill -0 $pid 2>/dev/null; do
        i=$(( (i+1) % (bar_length+1) ))
        printf "\r%-20s [" "$task"
        
        # Print filled portion
        for ((j=0; j<i; j++)); do
            printf "="
        done
        
        # Print cursor
        if [ $i -lt $bar_length ]; then
            printf ">"
            
            # Print remaining space
            for ((j=i+1; j<bar_length; j++)); do
                printf " "
            done
        else
            printf "="
        fi
        
        printf "]"
        sleep .1
    done
    
    wait $pid
    if [ $? -eq 0 ]; then
        printf "\r%-20s [" "$task"
        printf '%*s' "$bar_length" "" | tr ' ' '='
        printf "]${GREEN}✓${NC}\n"
        return 0
    else
        printf "\r%-20s [" "$task"
        printf '%*s' "$bar_length" "" | tr ' ' '='
        printf "]${RED}✗${NC}\n"
        return 1
    fi
}

# Ensure we're in the build directory
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Check if CMake needs to be run
if [ ! -f "Makefile" ]; then
    cmake "$PROJECT_ROOT" > /dev/null 2>&1 &
    show_progress $! "Configuring CMake"
    if [ $? -ne 0 ]; then
        cd "$ORIGINAL_DIR"
        exit 1
    fi
fi

# Build project
cmake --build . > /dev/null 2>&1 &
show_progress $! "Building project"
if [ $? -ne 0 ]; then
    cd "$ORIGINAL_DIR"
    exit 1
fi

# Run tests
cmake --build . --target test > /dev/null 2>&1 &
show_progress $! "Running tests"
if [ $? -ne 0 ]; then
    cd "$ORIGINAL_DIR"
    exit 1
fi

# Format code
cmake --build . --target format > /dev/null 2>&1 &
show_progress $! "Formatting code"
if [ $? -ne 0 ]; then
    cd "$ORIGINAL_DIR"
    exit 1
fi

# Run linter
cmake --build . --target check > /dev/null 2>&1 &
show_progress $! "Running linter"
if [ $? -ne 0 ]; then
    cd "$ORIGINAL_DIR"
    exit 1
fi

echo -e "\n${GREEN}✅ All checks passed!${NC}"

# Return to the original directory
cd "$ORIGINAL_DIR"
