#-------------------------------------------------------------------------------
# sub dirs
#-------------------------------------------------------------------------------
SRC_DIR      = $(srcdir)/src
TEMPLATE_DIR = $(srcdir)/template

#-------------------------------------------------------------------------------
# data files
#-------------------------------------------------------------------------------
templatedir = $(pkgdatadir)/template
dist_template_DATA = 	$(TEMPLATE_DIR)/template.mp3 \
						$(TEMPLATE_DIR)/template.flac

#-------------------------------------------------------------------------------
# program
#-------------------------------------------------------------------------------
bin_PROGRAMS = fmf

fmf_SOURCES = $(SRC_DIR)/CDDB.cpp \
				$(SRC_DIR)/CDDB.h \
				$(SRC_DIR)/CDDBParser.cpp \
				$(SRC_DIR)/CDDBParser.h \
				$(SRC_DIR)/Context.cpp \
				$(SRC_DIR)/Context.h \
				$(SRC_DIR)/Dir.cpp \
				$(SRC_DIR)/Dir.h \
				$(SRC_DIR)/EncodingDetector.cpp \
				$(SRC_DIR)/EncodingDetector.h \
				$(SRC_DIR)/File.cpp \
				$(SRC_DIR)/File.h \
				$(SRC_DIR)/Launcher.cpp \
				$(SRC_DIR)/Launcher.h \
				$(SRC_DIR)/main.cpp \
				$(SRC_DIR)/MusicFileCreator.cpp \
				$(SRC_DIR)/MusicFileCreator.h \
				$(SRC_DIR)/MusicFilesGenerator.cpp \
				$(SRC_DIR)/MusicFilesGenerator.h \
				$(SRC_DIR)/Options.cpp \
				$(SRC_DIR)/Options.h \
				$(SRC_DIR)/Tracer.cpp \
				$(SRC_DIR)/Tracer.h \
				$(SRC_DIR)/TrackInfo.cpp \
				$(SRC_DIR)/TrackInfo.h \
				$(SRC_DIR)/Utf8Converter.cpp \
				$(SRC_DIR)/Utf8Converter.h

fmf_CPPFLAGS = $(PTHREAD_CFLAGS) $(UCHARDET_CFLAGS) $(DEPS_CFLAGS) -DFMF_TEMPLATES_PATH=\"$(templatedir)\"
fmf_LDADD = $(PTHREAD_LIBS) $(PTHREAD_CFLAGS) $(UCHARDET_LIBS) $(DEPS_LIBS)

#-------------------------------------------------------------------------------
# man page
#-------------------------------------------------------------------------------
$(srcdir)/fmf.1: 	$(srcdir)/doc/manpage.rst
					rst2man $(srcdir)/doc/manpage.rst > $(srcdir)/fmf.1
					touch $(srcdir)/fmf.1

dist_doc_DATA = $(srcdir)/README
dist_man_MANS = $(srcdir)/fmf.1

EXTRA_DIST = $(srcdir)/doc/manpage.rst

CLEANFILES = $(srcdir)/fmf.1
