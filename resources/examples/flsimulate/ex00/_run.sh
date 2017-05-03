#!/usr/bin/env bash

opwd=$(pwd)

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Falaise is not setup!"
    exit 1
fi
echo >&2 "[info] Found flsimulate $(flsimulate --version | head -1 | cut -d' ' -f2)"

work_dir="$(pwd)/_work.d"
if [ -d ${work_dir} ]; then
    rm -fr ${work_dir}
fi
mkdir ${work_dir}
cp flsimulate.conf ${work_dir}/
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
flvisualize --input-file "flSD.brio"

# Set tags:
# --detector-config-file "urn:snemo:demonstrator:geometry:4.0" \
# --variant-config "urn:snemo:demonstrator:geometry:4.0:variants" \
# --variant-profile "urn:snemo:demonstrator:geometry:4.0:variants:profiles:default" \

# Set file paths:
# --detector-config-file "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf"
# --variant-config "@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf"
# --variant-load "@falaise:config/snemo/demonstrator/geometry/4.0/variants/profiles/default.profile"

cd ${opwd}
echo >&2 "[info] The end."
exit 0

# end
