
cppFlags=$(gsl-config --cflags)
ldFlags=$(gsl-config --libs)

g++ ${cppFlags} ex03.cxx ${ldFlags} -o ex03

./ex03 > ex03.res

gnuplot ex03.gp
