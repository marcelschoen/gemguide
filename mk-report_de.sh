## This script creates the guide
## replacing the default title page with a more informative one.

# Create the pdf from asciidoc
a2x --verbose -a encoding=utf-8 -f pdf --dblatex-opts "-P latex.encoding=utf8 -P latex.unicode.use=1 -P latex.output.revhistory=0" guide_de.txt 

# Remove the title page
pdftk guide_de.pdf cat 2-end output temp.pdf

# Add the separate title page
pdftk guide_de-title.pdf temp.pdf cat output guide_de.pdf

# Tidy up
rm temp.pdf

# Show the final report
evince guide_de.pdf
