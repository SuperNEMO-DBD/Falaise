.PHONY : all
all : falaise_geometry_config_principles.pdf

.PHONY : clean
clean :
	$(RM) falaise_geometry_config_principles.pdf

.PHONY : show
show : falaise_geometry_config_principles.pdf
	xdg-open $< &

falaise_geometry_config_principles.pdf : falaise_geometry_config_principles.dot
	dot -Tpdf falaise_geometry_config_principles.dot -o falaise_geometry_config_principles.pdf	

# end
