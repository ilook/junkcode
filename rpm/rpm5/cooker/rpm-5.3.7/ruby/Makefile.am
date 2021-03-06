# Makefile for rpm library.

AUTOMAKE_OPTIONS = 1.4 foreign

LINT = splint

AM_CPPFLAGS = -I. \
	-I$(top_srcdir) \
	-I$(top_srcdir)/build \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/rpmdb \
	-I$(top_srcdir)/rpmio \
	-I$(top_srcdir)/misc \
	@WITH_DB_CPPFLAGS@ \
	@WITH_FILE_CPPFLAGS@ \
	@WITH_JS_CPPFLAGS@ \
	@WITH_LUA_CPPFLAGS@ \
	@WITH_SYCK_CPPFLAGS@ \
	@WITH_XAR_CPPFLAGS@ \
	@WITH_ZLIB_CPPFLAGS@

EXTRA_DIST = tscripts hello.rb
EXTRA_PROGRAMS = xruby

RPMMISC_LDADD_COMMON = \
	$(top_builddir)/misc/librpmmisc.la \
	@LTLIBINTL@

RPMIO_LDADD_COMMON = \
	$(top_builddir)/rpmio/librpmio.la \
	$(RPMMISC_LDADD_COMMON)

RPMDB_LDADD_COMMON = \
	$(top_builddir)/rpmdb/librpmdb.la \
	$(RPMIO_LDADD_COMMON)

RPM_LDADD_COMMON = \
	$(top_builddir)/lib/librpm.la \
	$(RPMDB_LDADD_COMMON)

RPMBUILD_LDADD_COMMON = \
	$(top_builddir)/build/librpmbuild.la \
	$(RPM_LDADD_COMMON)

pkglibdir =     @USRLIBRPM@/lib
pkgbindir =     @USRLIBRPM@/bin

pkgbin_PROGRAMS =       trb
pkglib_LTLIBRARIES =    librpmrbm.la

librpmrbm_la_CFLAGS = -fno-strict-aliasing
librpmrbm_la_LDFLAGS = -module -avoid-version
librpmrbm_la_LIBADD = \
	$(top_builddir)/build/librpmbuild.la \
	$(top_builddir)/lib/librpm.la \
	$(top_builddir)/rpmdb/librpmdb.la \
	$(top_builddir)/rpmio/librpmio.la \
	$(top_builddir)/misc/librpmmisc.la \
	@LTLIBINTL@

noinst_HEADERS = \
	rpm-rb.h rpmts-rb.h rpmmi-rb.h rpmhdr-rb.h \
	rpmds-rb.h rpmfi-rb.h rpmte-rb.h rpmmc-rb.h \
	rpmps-rb.h

librpmrbm_la_SOURCES = \
	rpm-rb.c rpmts-rb.c rpmmi-rb.c rpmhdr-rb.c \
	rpmds-rb.c rpmfi-rb.c rpmte-rb.c rpmmc-rb.c \
	rpmps-rb.c

trb_SOURCES = trb.c
trb_LDADD = librpmrbm.la $(RPM_LDADD_COMMON)

xruby_SOURCES = xruby.c
xruby_LDADD = $(RPMIO_LDADD_COMMON)

.PHONY:	lint
lint:
	$(LINT) $(DEFS) $(INCLUDES) $(rpmrb_la_sources)

.PHONY:	check-local
check-local:
	-./trb

