TEX_SOURCES = \
	    *.tex \
      images/*.eps \
      images/*.pdf


all: main.pdf

pdf: main.pdf

ps: main.ps

ispell::
	ispell -d american $(TEX_SOURCES)

main.dvi: $(TEX_SOURCES) 95_bib.bib
	latex main
	bibtex main
	latex main
	latex main

main.ps: main.dvi
	dvips -Ppdf -G0 -t a4 main.dvi

main.pdf: main.ps
	ps2pdf -dPDFSETTINGS=/printer -dCompatibilityLevel=1.3 -dEmbedAllFonts=true -dSubsetFonts=true -dEPSCrop=true $< $@

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
