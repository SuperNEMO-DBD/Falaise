#!/usr/bin/env bash

opwd=$(pwd)

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Falaise is not setup!"
    exit 1
fi

work_dir="$(pwd)/_work.d"
mkdir ${work_dir}
cp flsimulate.conf seeds.conf vprofile.conf ${work_dir}/
cd ${work_dir}/

echo >&2 "[info] Running flsimulate..."
flsimulate \
    --verbosity "debug" \
    --config "flsimulate.conf" \
    --output-metadata-file "flSD.meta" \
    --embedded-metadata=1 \
    --output-file "flSD.brio"
if [  $? -ne 0 ]; then
    echo >&2 "[error] flsimulate failed!"
    cd ${opwd}
    exit 1
fi

echo >&2 "[info] Running flvisualize..."
flvisualize \
    --detector-config-file "$(flquery --resourcedir)/config/snemo/demonstrator/geometry/4.0/manager.conf" \
    --variant-config "$(flquery --resourcedir)/config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
    --variant-load "vprofile.conf" \
    --input-file "flSD.brio"

cd ${opwd}
echo >&2 "[info] The end."
exit 0

# end
