#!/bin/bash
LIST=`cat grepmyhep.txt | cut -d':' -f1`
for file in $LIST; do
mv $file ../mybhep/
done