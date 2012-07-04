TEX_SOURCES = \
	    *.tex \
      images/*.eps \
      images/*.pdf


all: main.pdf report

pdf: main.pdf report

ps: main.ps

ispell::
	ispell -d american $(TEX_SOURCES)

main.dvi: $(TEX_SOURCES) 95_bib.bib
	latexmk -dvi main.tex

main.ps: main.tex $(TEX_SOURCES) 95_bib.bib
	latexmk -ps main.tex

main.pdf: main.tex $(TEX_SOURCES) 95_bib.bib
	latexmk -pdf main.tex

report: intermediate_report.tex $(TEX_SOURCES) 95_bib.bib
	latexmk -pdf intermediate_report.tex

clean:
	/usr/bin/find . -name "*.aux" -exec /bin/rm {} \;
	/usr/bin/find . -name "*.bbl" -exec /bin/rm {} \;
	/usr/bin/find . -name "*.blg" -exec /bin/rm {} \;
	/usr/bin/find . -name "*.dvi" -exec /bin/rm {} \;
	/usr/bin/find . -name "*.lof" -exec /bin/rm {} \;
	/usr/bin/find . -name "*.log" -exec /bin/rm {} \;
	/usr/bin/find . -name "*.lot" -exec /bin/rm {} \;
	/usr/bin/find . -name "*.toc" -exec /bin/rm {} \;
	/usr/bin/find . -name "*.bak" -exec /bin/rm {} \;
	/usr/bin/find . -name "*~" -exec /bin/rm {} \;
	/usr/bin/find . -maxdepth 1 -name "*.pdf" -exec /bin/rm {} \;
	/usr/bin/find . -maxdepth 1 -name "*.ps" -exec /bin/rm {} \;
	latexmk -CA
