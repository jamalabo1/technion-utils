#!/bin/sh

export PERL5LIB="${HOME}/perl5/lib/perl5${PERL5LIB:+:${PERL5LIB}}"
exec /Library/TeX/texbin/latexindent "$@"
