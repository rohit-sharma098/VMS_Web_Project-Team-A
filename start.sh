#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
g++ -std=c++17 -O2 backend.cpp -o backend
mkdir -p data
touch data/visitors.txt data/blacklist.txt
node server.js
