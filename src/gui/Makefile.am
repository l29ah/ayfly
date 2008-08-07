INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/z80ex -I$(top_srcdir)/src/z80ex/include $(AYFLY_CXXFLAGS) $(all_includes)

bin_PROGRAMS = ayfly
ayfly_SOURCES = AyflyApp.cpp AyflyFrame.cpp DnDFiles.cpp AyflyBindingsDlg.cpp

if IS_UNIX
EXTRAINCLUDES += $(SDL_CFLAGS)
endif

if IS_WINDOWS
EXTRAINCLUDES += $(DX_CFLAGS)
endif

INCLUDES += $(EXTRAINCLUDES)

ayfly_LDADD = libayfly.a
AM_CXXFLAGS = $(AYFLY_CXXFLAGS)

datarootdir = @datarootdir@
