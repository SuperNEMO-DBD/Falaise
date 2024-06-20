
flsimulate \
    --verbosity "debug" \
    --config "simu1.conf" \
    -o "SD.brio" \
    > flsim1.log 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] flsimulate failed!"
    exit 1
fi

flvisualize \
    --variant-profile "profile.conf" \
    -i "SD.brio"

exit 0
