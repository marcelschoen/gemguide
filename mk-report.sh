## This script creates the guide
## replacing the default title page with a more informative one.

# Create the pdf from asciidoc
a2x -f pdf --dblatex-opts "-P latex.output.revhistory=0" guide.txt 

# Remove the title page
pdftk guide.pdf cat 2-end output temp.pdf

# Add the separate title page
pdftk guide-title.pdf temp.pdf cat output guide.pdf

# Tidy up
rm temp.pdf

# Show the final report
evince guide.pdf
