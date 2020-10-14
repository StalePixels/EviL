EVIL a.k.a. `Essentially VI Lite`
=================================

Current differences to QE:
 * Renamed ;-)

Original QE README follows

QE, a Vi Adjacent, by David Given
=================================

Ported to NextZXOS by D. Rimron-Soutter
---------------------------------------

*This isn't the source-code of QE, this is just a tribute.*

You can see the "main" repository, with the rest of CP/Mish, where this was
taken from at https://github.com/StalePixels/cpmish/tree/ESXDOS (the
working branch this code was developed in) - which is a fork of David's original
code at https://github.com/davidgiven/cpmish. This repository is just a mirror of
required files to build the editor without the rest of the Operating System.
The licence documents for the entire of the original repository are here too, to
make it easier for you, the reader, to find.

Bugfixes against this QE itself should be made to the parent repository,
bugfixes for the NextZXOS shim version of libcuss (the ESXCUSS folder) are welcome
here.

Building is handled via the Makefile - not part of the usual C/PMish way of doing
things, and this project was last successfully built with z88dk nightly-20200412.
