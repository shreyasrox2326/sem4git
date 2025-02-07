#!/bin/bash

# Ensure the source file exists
if [[ ! -f ./disk-files/foo.pdf ]]; then
  echo "Error: Source file ./disk-files/foo.pdf not found!"
  exit 1
fi
# Create 5000 copies of foo.pdf
for ((i=0; i<5000; i++)); do
  cp ./disk-files/foo.pdf "./disk-files/foo$i.pdf"
done
echo "Successfully copied foo.pdf to 5000 files."
