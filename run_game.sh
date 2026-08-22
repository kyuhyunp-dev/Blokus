#!/bin/bash

# Build the project (Shared, Server, Client)
echo "Building the project..."
cmake --build build

# Check if the build command was successful (exit code 0)
if [ $? -ne 0 ]; then
    echo "Build failed! Aborting run."
    exit 1
fi

echo "Build successful."
echo "---------------------------------"

# Define a cleanup function that runs when the script exits (or if you hit Ctrl+C)
trap 'echo -e "\nCleaning up processes..."; kill $SERVER_PID $CLIENT1_PID $CLIENT2_PID 2>/dev/null' EXIT

# Start the server in the background (&)
echo "Starting Game Server..."
./build/bin/BlokusServer &
SERVER_PID=$!

# Give the server a split second to bind to the port
sleep 0.5

# Start a client in the background
echo "Starting Client 1 (Background)..."
./build/bin/BlokusClient &
CLIENT1_PID=$!

# Start a client in the foreground
echo "Starting Client 2 (Foreground)..."
./build/bin/BlokusClient &
CLIENT2_PID=$!

echo "Game is running. Close both clients to shut down the server."
wait $CLIENT1_PID
wait $CLIENT2_PID