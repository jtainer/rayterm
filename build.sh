#!/bin/bash
gcc src/main.c src/keymap.c -lraylib -lm -ldl -lrt -lpthread -lGL -lX11
