#!/bin/bash

SERVER_PID=$(pidof pacman_server)
CLIENT_PID=$(pidof pacman_client)

if [ -n "$SERVER_PID" ]; then
    echo "Stopping pacman_server (PID: $SERVER_PID)..."
    kill -9 "$SERVER_PID" && echo "Server stopped."
else
    echo "No pacman_server running."
fi

if [ -n "$CLIENT_PID" ]; then
    echo "Stopping pacman_client (PID: $CLIENT_PID)..."
    kill -9 "$CLIENT_PID" && echo "Client stopped."
else
    echo "No pacman_client running."
fi
