# -*- mode: make; -*-

THE_DIR=.
XFIG_DIR=$(THE_DIR)/fig
PDFTEX_DIR=$(THE_DIR)/pdftex
PDFTEX_T_DIR=$(THE_DIR)/pdftex_t
PDFFMT=pdf

MAIN=fecom_usage

.PHONY : all
all : pdf

.PHONY : show
show : $(MAIN).pdf
	evince $< &

SUFFIXES : .tex .fig .pdftex .pdftex_t .pdf

vpath %.fig $(XFIG_DIR)
vpath %.pdf $(PDFTEX_DIR)
vpath %.pdftex_t $(PDFTEX_T_DIR)

fig_files := $(notdir $(shell ls $(XFIG_DIR)/*.fig))
fig2pdftex_files  := $(fig_files:%.fig=%.pdf)
fig2pdftex_t_files  := $(fig_files:%.fig=%.pdftex_t)

.PHONY : clean
clean :
	find . -name "*~" -exec $(RM) \{\} \;
	find . -name "*.fig.bak" -exec $(RM) \{\} \;
	$(RM) $(MAIN).aux
	$(RM) $(MAIN).log
	$(RM) $(MAIN).nav
	$(RM) $(MAIN).out
	$(RM) $(MAIN).pdf
	$(RM) $(MAIN).snm
	$(RM) $(MAIN).toc
	$(RM) $(MAIN).bbl
	$(RM) $(MAIN).blg
	$(RM) missfont.log
	$(RM) -r pdftex pdftex_t

.PHONY : clean_pdftex
clean_pdftex :
	@$(RM) $(addprefix $(PDFTEX_DIR)/,$(fig2pdftex_files))

.PHONY : clean_pdftex_t
clean_pdftex_t :
	@$(RM) $(addprefix $(PDFTEX_T_DIR)/,$(fig2pdftex_t_files))

tex_sources=*.tex #biblio.bib

.PHONY : pdf
pdf : $(MAIN).pdf

$(MAIN).pdf : $(MAIN).tex $(tex_sources) pictures
	pdflatex $<
#	bibtex $(MAIN)
	pdflatex $<
	pdflatex $<

.PHONY : pictures
pictures : fig2pdftex_t


%.pdftex_t : %.fig
	test ! -d pdftex_t && mkdir -p pdftex_t || echo -n
	fig2dev -L pdftex_t  -l landscape \
	  -p "\pdfteximgpath/"$*.pdf	  $< \
	  $(PDFTEX_T_DIR)/$(notdir $(@))

%.pdf : %.fig
	test ! -d pdftex && mkdir -p pdftex || echo -n
	fig2dev -L pdftex -l landscape $< $(PDFTEX_DIR)/$(notdir $(@))


.PHONY : fig2pdftex_t
fig2pdftex_t : $(fig2pdftex_t_files) fig2pdftex

.PHONY : fig2pdftex
fig2pdftex : $(fig2pdftex_files)

# end
