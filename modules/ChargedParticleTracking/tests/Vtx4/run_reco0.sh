
flreconstruct \
    --verbosity "debug" \
    --config "reco0.conf" \
    -i "SD.brio" \
    -o "TTD.brio" \
    > flrec0.log 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] flreconstruct failed!"
    exit 1
fi

flvisualize \
    --variant-profile "profile.conf" \
    -i "TTD.brio"


exit 0
