#!/bin/bash

# Extract file name without extension
folder="${1}"
basename="${2}"
pdf_source="${folder}/.aux/${basename}.pdf"

# Check if PDF exists
if [ ! -f "$pdf_source" ]; then
    echo "❌ PDF not found at $pdf_source"
    exit 1
fi

response=$(osascript <<EOT
set defaultName to "$basename.pdf"
set destFile to choose file name with prompt "Save PDF as:" default name defaultName
return POSIX path of destFile
EOT
)
dest="$response"

if [ -z "$dest" ]; then
    echo "❌ No file selected."
    exit 1
fi

# Copy the PDF
cp "$pdf_source" "$dest"
echo "✅ Saved PDF to $dest"