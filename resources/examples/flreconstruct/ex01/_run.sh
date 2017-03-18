#!/usr/bin/env bash

opwd=$(pwd)

which flreconstruct > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Falaise is not setup!"
    exit 1
fi

samples_dir="$(pwd)/samples"
work_dir="$(pwd)/_work.d"
mkdir ${work_dir}
cp flreconstruct.conf vprofile.conf ${work_dir}/
cd ${work_dir}/

echo >&2 "[info] Running flreconstruct..."
flreconstruct \
    --verbosity "trace" \
    --pipeline "flreconstruct.conf" \
    --input-metadata-file "${samples_dir}/input/flSD.meta" \
    --input-file "${samples_dir}/input/flSD.brio" \
    --output-metadata-file "flRec.meta" \
    --embedded-metadata=1 \
    --output-file "flRec.brio"
if [  $? -ne 0 ]; then
    echo >&2 "[error] flreconstruct failed!"
    cd ${opwd}
    exit 1
fi

echo >&2 "[info] Running flvisualize..."
flvisualize \
    --detector-config-file "$(flquery --resourcedir)/config/snemo/demonstrator/geometry/4.0/manager.conf" \
    --variant-config "$(flquery --resourcedir)/config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
    --variant-load "vprofile.conf" \
    --input-file "flRec.brio"

cd ${opwd}
echo >&2 "[info] The end."
exit 0

# end
