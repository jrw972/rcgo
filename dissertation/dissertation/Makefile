all : paper.pdf

paper.pdf : paper.dvi
	dvipdf $^ $@

LATEX=latex
LATEX_ARGS=-file-line-error -halt-on-error


paper.dvi : paper.tex introduction.tex background.tex model.tex implementation.tex advanced.tex evaluation.tex conclusion.tex paper.bib clock_automaton.tex clocksys.tex clocksys_alpha.tex webserver.tex workflow.tex ethernet.tex reactive_component.eps web_server.eps
	$(LATEX) $(LATEX_ARGS) paper
	bibtex paper
	$(LATEX) $(LATEX_ARGS) paper
	$(LATEX) $(LATEX_ARGS) paper

%.tex : %.pic
	pic -t $< > $@

.PHONY : clean
clean :
	-rm -f paper.aux paper.dvi paper.pdf
