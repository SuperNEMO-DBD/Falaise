



cppFlags=$(gsl-config --cflags)
ldFlags=$(gsl-config --libs)

g++ ${cppFlags} ex02.cxx ${ldFlags} -o ex02

./ex02 > ex02.res

gnuplot ex02.gp
