# -*- mode: make; -*-

.PHONY : all
all : trigger_configuration.html

.PHONY : clean
clean :
	rm -f trigger_configuration.html	
	find . -name "*~" -exec rm -f \{\} \;
	find . -name "*.bak" -exec rm -f \{\} \;

.PHONY : show
show : trigger_configuration.html
	xdg-open $< &

trigger_configuration.html : trigger_configuration.rst img
	rst2html trigger_configuration.rst > trigger_configuration.html
	rst2latex trigger_configuration.rst > trigger_configuration.latex
	pdflatex trigger_configuration.latex

.PHONY : img
img :

# fig2dev -L jpeg fig/arch0.fig images/arch0.jpeg
# fig2dev -L jpeg fig/arch1.fig images/arch1.jpeg

# end
