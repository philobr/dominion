#!/bin/bash

# This script starts the server and two clients in the background.
# The server and clients are started with the "info" log level and log to separate files.
# All processes will be terminated when the script receives a SIGINT or SIGTERM signal.
#
# Usage: ./open_game_and_clients.sh [options]
# Options:
# -h, --help:      Display help message
# -D, --debug:     Enable debug mode for client and server
# -l, --log-level: Set log level for client and server

# Print help message
print_help() {
    echo "Usage: $0 [options]" 1>&2
    echo "Options:" 1>&2
    echo "  -h, --help:      Display this help message" 1>&2
    echo "  -D, --debug:     Enable debug mode for client and server" 1>&2
    echo "  -l, --log-level: Set log level for client and server" 1>&2
}

# Parse command line arguments
while [ "$#" -gt 0 ]; do
    case "$1" in
        -h|--help)
            print_help
            exit 0
            ;;
        -D|--debug)
            export DEBUG_OPTIONS="--debug"
            shift
            ;;
        -l|--log-level)
            if [ -z "$2" ]; then
                echo "Error: Missing argument for option $1" 1>&2
                print_help
                exit 1
            fi
            export LOG_LEVEL="--log-level $2"
            shift 2
            ;;
        *)
            echo "Error: Unrecognized option: $1" 1>&2
            print_help
            exit 1
            ;;
    esac
done

# Set log level
#
# If LOG_LEVEL is set, it overrides the default log level.
# If DEBUG_OPTIONS is set, log level is set to "debug" by default.
# Otherwise, log level is set to "warn" by default.
if [ -z "$DEBUG_OPTIONS" ]; then
    DEFAULT_LOG_LEVEL="debug"
else
    DEFAULT_LOG_LEVEL="warn"
fi
LOG_LEVEL=${LOG_LEVEL:-"--log-level $DEFAULT_LOG_LEVEL"}

# Flag to prevent double cleanup
CLEANUP_DONE=0

# Set up trap to kill all child processes when the script exits
cleanup() {
    if [ "$CLEANUP_DONE" -eq 0 ]; then
        CLEANUP_DONE=1
        echo "Cleaning up processes..." 1>&2
        # Kill all processes in the current process group
        kill 0
    fi
}

# Set up trap for various exit signals
trap cleanup EXIT SIGINT SIGTERM

# Search for the "dominion" directory, starting from current directory and going up
DIR="$PWD"
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

EXEC_DIR="$DOMINION_DIR/build"

# Change to the executable directory
cd "$EXEC_DIR" || {
    echo "Error: Cannot access directory containing executables ($EXEC_DIR)" 1>&2
    exit 1
}

# Clear the log files
rm -f server.log client1.log client2.log

# Start the server in the background
./server_exe -f server.log ${LOG_LEVEL} ${DEBUG_OPTIONS} &
SERVER_PID=$!

# Check if server started successfully
if ! ps -p $SERVER_PID > /dev/null; then
    echo "Error: Failed to start server" 1>&2
    exit 1
fi

# Start first client in the background
./client_exe -f client1.log ${LOG_LEVEL} ${DEBUG_OPTIONS} &
CLIENT1_PID=$!

# Start second client in the background
./client_exe -f client2.log ${LOG_LEVEL} ${DEBUG_OPTIONS} &
CLIENT2_PID=$!

# Check if clients started successfully
if ! ps -p $CLIENT1_PID > /dev/null || ! ps -p $CLIENT2_PID > /dev/null; then
    echo "Error: Failed to start one or more clients" 1>&2
    exit 1
fi

echo "Successfully started server (PID: $SERVER_PID) and clients (PIDs: $CLIENT1_PID, $CLIENT2_PID)" 1>&2
echo "Press Ctrl+C to terminate all processes" 1>&2

# Wait forever (or until interrupted)
wait
