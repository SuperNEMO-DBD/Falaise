#!/bin/bash
set -ex
flsimulate -c $1/issue201_sim.conf -o issue201_sim.brio
flreconstruct -i issue201_sim.brio -p "@falaise:snemo/demonstrator/reconstruction/official-2.0.0.conf" -o issue201_reco.brio
flreconstruct -i issue201_reco.brio -p $1/issue201_reco.conf -o issue201_reco_2.brio