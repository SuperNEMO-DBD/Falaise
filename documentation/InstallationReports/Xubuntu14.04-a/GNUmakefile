# -*- mode: make; -*-

fl_report_label=xubuntu14.04-a
fl_report_version=0.1
fl_report_name=falaise-trunk_$(fl_report_label)_report
fl_tagged_report_name=falaise-trunk_$(fl_report_label)_report-$(fl_report_version)

.PHONY : all
all : html pdf

.PHONY : clean
clean :
	$(RM) $(fl_tagged_report_name).pdf
	$(RM) $(fl_report_name).html
	$(RM) $(fl_report_name).pdf
	$(RM) $(fl_report_name).aux
	$(RM) $(fl_report_name).log
	$(RM) $(fl_report_name).out
	$(RM) $(fl_report_name).tex
	$(RM) $(fl_report_name).toc
	find . -name "*~" -exec rm -f \{\} \;

.PHONY : tag
tag : final
	cp -f $(fl_tagged_report_name).pdf tagged/

.PHONY : final
final : $(fl_report_name).pdf
	cp -f $(fl_report_name).pdf $(fl_tagged_report_name).pdf

.PHONY : html
html : $(fl_report_name).html

.PHONY : pdf
pdf : $(fl_report_name).pdf

.PHONY : show
show : $(fl_report_name).html
	xdg-open $< &

.PHONY : show_pdf
show_pdf : $(fl_report_name).pdf
	xdg-open $< &

$(fl_report_name).html :  $(fl_report_name).rst
	rst2html $< > $@

$(fl_report_name).pdf : $(fl_report_name).tex
	pdflatex $<
	pdflatex $<

$(fl_report_name).tex :  $(fl_report_name).rst
	rst2latex --documentclass=article \
	  --documentoptions=a4paper,a4wide,10pt \
	  --use-latex-docinfo \
	  --use-verbatim-when-possible \
	  --stylesheet="style/geo" \
	  --use-latex-toc \
	  --syntax-highlight="long" \
	  --section-numbering \
	  $< > $@

# end
