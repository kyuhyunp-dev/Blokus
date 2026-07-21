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
./build/bin/BlokusClient

echo "Client 2 closed. Cleaning up..."
kill $CLIENT1_PID

# When the client is closed, kill the background server
echo "Client closed. Shutting down server..."
kill $SERVER_PID