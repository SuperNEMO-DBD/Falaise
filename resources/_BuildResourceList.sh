#!/usr/bin/env bash

resources_list_cmake="_resources.cmake"
rm -f ${resources_list_cmake}
touch ${resources_list_cmake}

cat > ${resources_list_cmake} <<EOF
set(Falaise_RESOURCES
EOF

find ./ -type f \
  | grep -v "/_" \
  | grep -v "~$" \
  | grep -v "\#$" \
  | grep -v /.svn/ \
  | grep -v "CMakeLists.txt" \
  | grep -v "/\..*" \
  | sed -e 's@^./@${PROJECT_SOURCE_DIR}/resources/@g' \
  | sed -e 's@//@/@g' \
  | sort \
  >> ${resources_list_cmake}

cat >> ${resources_list_cmake} <<EOF
)
EOF

exit 0
