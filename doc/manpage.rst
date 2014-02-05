===
fmf
===

--------------------------
fake music files generator
--------------------------

:Author: gd@iotide.com
:Copyright: gd@iotide.com
:Version: fake music files generator
:Manual section: 1
:Manual group: User commands

SYNOPSIS
========

fmf ``-d`` <cddb directory> ``-o`` <output directory> [``-n`` <number of cddb files>] [``-c`` <number of threads>] [``-u`` <update_cache>]

fmf ``-i`` <path to a cddb file> ``-o`` <output directory>

DESCRIPTION
===========

fmf's purpose is to generate tagged music files for testsing media servers.
The Free CDDB is used as data source for tagging.

Preparing the cddb
------------------

CDDB can be downloaded from http://ftp.freedb.org/pub/freedb/. Any update db can be used instead of the full db.

Example:

     | cd /tmp
     | wget http://ftp.freedb.org/pub/freedb/freedb-update-20120101-20120201.tar.bz2
     | mkdir /tmp/cddb
     | cd /tmp/cddb
     | tar jxvf freedb-update-20120101-20120201.tar.bz2

/tmp/cddb can now be used as argument for ``--cddb`` option.

OPTIONS
=======
-d, --cddb       The CDDB directory.
                 This is the directory where CDDB archive was extracted.
                 CDDB files will be picked randomly from this database.

-i, --in         Path to a CDDB file.
                 This can be used instead of -d in order to use a specific CDDB file as input.

-o, --out        Output directory.
                 This is the directory where fake music files will be generated.
                 Sub directories will be created for the titles: ARTIST/ALBUM/TITLE

-n, --num-albums Number of CDDB files to use for generating fake music files.
                 Default: 1

-u, --update     Update the CDDB cache.
                 fmf creates a cache with lists of all the CDDB files by scanning the CDDB directory (-d).
                 Use this option to force a re-scan of the CDDB directory.
                 The cache will be automatically updated if the value of -d option does not match the previously cached directory.

-t, --template   The template music file to use for creating fake music files.
                 This file will be copied and renamed and then tagged for each generated fake music file.
                 Keep it small if generating many files.
                 Default: %DEFAULT_TEMPLATE%

-c, --threads    The number of threads to use.
                 Default: 1

-v, --verbose    Increase output verbosity.

    --version    Output version.

-h, --help       Output this message.
	--usage

FILES
=====
./fmf.cache/cddb.cache*
	created at the currect directory when fmf starts

LICENSE
=======
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.