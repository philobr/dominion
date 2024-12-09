#!/bin/bash

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
./server_exe -l info -f server.log --debug &
SERVER_PID=$!

# Check if server started successfully
if ! ps -p $SERVER_PID > /dev/null; then
    echo "Error: Failed to start server" 1>&2
    exit 1
fi

# Start first client in the background
./client_exe -l info -f client1.log &
CLIENT1_PID=$!

# Start second client in the background
./client_exe -l info -f client2.log &
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
