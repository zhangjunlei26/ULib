## Makefile.am for examples/xml2txt

EXTRA_DIST = *.spec*

DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include

xml2txt_LDADD   = $(top_builddir)/src/ulib/lib@ULIB@.la @ULIB_LIBS@
xml2txt_SOURCES = main.cpp
xml2txt_LDFLAGS = $(PRG_LDFLAGS)

noinst_PROGRAMS = xml2txt

clean-local:
	-rm -rf core .libs *.bb* *.da *.gcov *.la *.exe gmon.out
