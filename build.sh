#!/bin/bash
gcc src/main.c src/input_map.c -lraylib -lm -ldl -lrt -lpthread -lGL -lX11
