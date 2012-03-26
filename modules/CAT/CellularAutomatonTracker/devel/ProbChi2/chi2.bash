#!/usr/bin/env bash


g++ $(boost-config --cflags) $(root-config --cflags) chi2.cxx -o chi2 $(boost-config --ldflags) $(root-config --libs)

