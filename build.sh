#!/bin/bash
gcc src/*.c -lraylib -lm -ldl -lrt -lpthread -lGL -lX11 -orayterm
