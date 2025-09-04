all:
	New-Item -Type Directory -Path build -Force
	xelatex --shell-escape -8bit --output-dir=build main.tex
	xelatex --shell-escape -8bit --output-dir=build main.tex