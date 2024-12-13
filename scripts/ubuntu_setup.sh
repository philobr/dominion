#!/bin/bash

# Print help message
print_help() {
    echo "Help Message Not Implemented Yet"
}


# Parse command line arguments
while [ "$#" -gt 0 ]; do
    case "$1" in
        -h|--help)
            print_help
            exit 0
            ;;
        *)
            echo "Error: Unrecognized option: $1" 1>&2
            print_help
            exit 1
            ;;
    esac
done

# Check if cmake is installed
if ! command -v cmake &> /dev/null; then
    echo "cmake not found, installing..."

    # Update the package list
    sudo apt update

    # Install cmake
    sudo apt install -y cmake

    # Confirm installation
    if command -v cmake &> /dev/null; then
        echo "cmake installed successfully."
    else
        echo "Failed to install cmake."
    fi
else
    echo "cmake is already installed."
fi

# Variable for status of wxWidgets
# 0 = installed with a high enough version
# 1 = not installed
# 2 = installed, but version not high enough
WXWIDGETS_STATUS=0

#Check if wxWidgets is installed
if ! command -v wx-config &> /dev/null; then
    WXWIDGETS_STATUS=1
else
    # Get the wxWidgets version
    WX_VERSION=$(wx-config --version)

    # Compare the version with 3.2.6
    if [[ "$(printf '%s\n' "$WX_VERSION" "3.2.6" | sort -V | head -n1)" == "3.2.6" ]]; then
        WXWIDGETS_STATUS=0
    else
        WXWIDGETS_STATUS=2
    fi
fi

if WXWIDGETS_STATUS==1; then
    echo "WxWidgets version too low, removing it"
    #TODO
    WXWIDGETS_STATUS==2
fi

if WXWIDGETS_STATUS==0; then
    echo "WxWidgets installation found and version high enough"
else
    echo "installing wxWidgets"
    cd ~
    git clone https://github.com/wxWidgets/wxWidgets.git
    cd ./wxWidgets
    mkdir buildgtk
    cd buildgtk
    ../configure --with-gtk
    make -j$(nprocs)
    sudo make install
    sudo ldconfig
fi

# Search for the "dominion" directory, starting from current directory and going up
DIR="$HOME"
while [ "$DIR" != "/" ]; do
    if [ -d "$DIR/dominion" ]; then
        DOMINION_DIR="$DIR/dominion"
        break
    fi
    DIR=$(dirname "$DIR")
done

if [ -z "$DOMINION_DIR" ]; then
    echo "Error: Could not find dominion directory" 1>&2
    exit 1
fi

if [ -d "$DOMINION_DIR/build" ]; then
    echo "Creating build directory"
    mkdir "$DOMINION_DIR/build"
fi

EXEC_DIR="$DOMINION_DIR/build"

# build dominion
cd $EXEC_DIR
cmake ..
make -j$(nprocs)

