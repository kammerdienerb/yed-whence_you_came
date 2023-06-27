#!/usr/bin/env bash
gcc -o whence_you_came.so whence_you_came.c $(yed --print-cflags --print-ldflags)
