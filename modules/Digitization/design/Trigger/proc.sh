#!/usr/bin/env bash

CPPFLAGS="-I. $(gsl-config --cflags) -I$(bxquery --incdir) -I$(bxquery --incdir)/bayeux"
LDFLAGS="-L$(bxquery --libdir) -lBayeux"
g++ -std=c++11 ${CPPFLAGS} snt0.cxx ${LDFLAGS} -o snt0
g++ -std=c++11 ${CPPFLAGS} build_mem1.cxx ${LDFLAGS} -o build_mem1
g++ -std=c++11 ${CPPFLAGS} build_mem2.cxx ${LDFLAGS} -o build_mem2
# g++ -std=c++11 ${CPPFLAGS} test_mem2.cxx ${LDFLAGS} -o test_mem2 -Wl,-rpath=$(bxquery --libdir)



exit 0

# end
