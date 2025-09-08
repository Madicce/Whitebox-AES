LATEX = pdflatex
FLAGS = --shell-escape

all: report.pdf

report.pdf: report.tex
	$(LATEX) $(FLAGS) $^
	 bibtex report
	$(LATEX) $(FLAGS) $^ 
	$(LATEX) $(FLAGS) $^
	

clean:
	rm -f *.aux *.log *.toc report.pdf 
	rm -f -r _minted-report/
