#!/usr/bin/env bash


flreconstruct -p rec.conf -i simu.brio -o rec.brio
flvisualize --variant-profile simu.profile -i rec.brio
