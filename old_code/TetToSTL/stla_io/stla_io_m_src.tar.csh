#!/bin/csh
#
#  Purpose:
#
#    Create a GZIP'ed TAR file of the m_src/stla_io files.
#
#  Modified:
#
#    02 January 2006
#
#  Author:
#
#    John Burkardt
#
#  Move to the directory just above the "stla_io" directory.
#
cd $HOME/public_html/m_src
#
#  Delete any TAR or GZ file in the directory.
#
echo "Remove TAR and GZ files."
rm stla_io/*.tar
rm stla_io/*.gz
#
#  Create a TAR file of the "stla_io" directory.
#
echo "Create TAR file."
tar cvf stla_io_m_src.tar stla_io/*
#
#  Compress the file.
#
echo "Compress the TAR file."
gzip stla_io_m_src.tar
#
#  Move the compressed file into the "stla_io" directory.
#
echo "Move the compressed file into the directory."
mv stla_io_m_src.tar.gz stla_io
#
#  Say goodnight.
#
echo "The stla_io_m_src gzip file has been created."
