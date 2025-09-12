#!/bin/bash
echo "tcp client is waiting for connection"
./tcp_rcv
sleep 5

echo "decoding audio and video is going on..."
./flactowav 
sleep 5

mkdir -p frames
./h265torgb recv_output.h265
sleep 5

echo "playing audio and video ..."
./main

