

.PHONY : all
all : falaise_urn_view1.pdf 

.PHONY : clean
clean : clean1
	find . -name "*~" -exec rm -f \{\} \; 

.PHONY : show1
show1 : falaise_urn_view1.pdf
	xdg-open $< &		

.PHONY : clean1
clean1 :
	$(RM) falaise_urn_view1.pdf
	$(RM) falaise_urn_view1.dot

falaise_urn_view1.pdf  : falaise_urn_view1.dot		
	dot -Tpdf $< -ofalaise_urn_view1.pdf

falaise_urn_view1.dot : \
  falaise_urn_view1-in.dot \
  falaise_urn_experiment.dot \
  falaise_urn_geometry-5.0.dot \
  falaise_urn_setup-2.0.dot  \
  falaise_urn_decays-1.4.dot  \
  falaise_urn_vertex-5.0.dot   \
  falaise_urn_simulation-2.4.dot    \
  falaise_urn_reconstruction-3.0.dot   \
  falaise_urn-5.0.0.dot 
	m4 falaise_urn_view1-in.dot > falaise_urn_view1.dot

# end
