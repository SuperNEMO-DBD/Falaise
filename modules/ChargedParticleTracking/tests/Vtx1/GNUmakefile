.PHONY : all
all : run1

.PHONY : clean
clean :
	$(RM) flrec1.log
	$(RM) PTD.brio
	$(RM) flrec0.log
	$(RM) TTD.brio
	find . -name "*~" -exec $(RM) \{\} \;

.PHONY : run1
run1 : PTD.brio

PTD.brio : TTD.brio pipeline1.conf profile.conf reco1.conf
	./run_reco1.sh 


.PHONY : run0
run0 : TTD.brio

TTD.brio : PTD_link.brio pipeline0.conf profile.conf reco0.conf
	./run_reco0.sh 

