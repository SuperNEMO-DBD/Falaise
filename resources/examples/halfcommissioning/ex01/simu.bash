#!/usr/bin/env bash


flsimulate -c simu.conf -o simu.brio
flvisualize --variant-profile simu.profile -i simu.brio
