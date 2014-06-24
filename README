fmf - fake music files generator
================================

Description
-----------
fmf's purpose is to generate tagged music files for testsing media servers.
The Free CDDB is used as data source for tagging.

Preparing the cddb
------------------
CDDB can be downloaded from http://ftp.freedb.org/pub/freedb/. Any update db can be used instead of the full db.

    cd /tmp
    wget http://ftp.freedb.org/pub/freedb/freedb-update-20120101-20120201.tar.bz2
    mkdir /tmp/cddb
    cd /tmp/cddb
    tar jxvf ../freedb-update-20120101-20120201.tar.bz2

Now `/tmp/cddb` can be used as argument for `--cddb` option.

Usage
-----
    ./fmf --cddb <cddb directory> --out <output directory>

Options:

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
                     Default: /usr/local/share/fmf/template/template.mp3
    
    -c, --threads    The number of threads to use.
                     Default: 1
    
    -v, --verbose    Increase output verbosity.
    
        --version    Output version.
    
    -h, --help
    	--usage      Output this message.

Building
--------

1. taglib - >= 1.7 tagging library
2. uchardet - charset detection library
3. python-docutils - rst2man required for generating manpage
4. gcc >= 4.7 - required `-std=c++11`
5. autoconf >= 2.6 (for maintainer)

Dependencies installation:

    apt-get install g++ pkg-config libuchardet-dev libtag1-dev python-docutils

License
-------
GPLv2
