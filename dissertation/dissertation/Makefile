all : paper.pdf

LATEX=pdflatex
LATEX_ARGS=-file-line-error -halt-on-error


paper.pdf : paper.tex introduction.tex background.tex model.tex implementation.tex advanced.tex evaluation.tex conclusion.tex paper.bib
	$(LATEX) $(LATEX_ARGS) paper
	bibtex paper
	$(LATEX) $(LATEX_ARGS) paper
	$(LATEX) $(LATEX_ARGS) paper

.PHONY : clean
clean :
	-rm -f paper.aux paper.pdf
