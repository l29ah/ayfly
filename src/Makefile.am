INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/z80ex -I$(top_srcdir)/src/z80ex/include $(SDL_CFLAGS) $(DX_CFLAGS) $(all_includes)
SUBDIRS = gui z80ex
bin_PROGRAMS = ayfly
ayfly_SOURCES = AbstractAudio.cpp ay.cpp Filter3.cpp formats.cpp speccy.cpp

if IS_WINDOWS
EXTRASUBDIRS += windows
else
EXTRASUBDIRS += unix
endif

SUBDIRS += $(EXTRASUBDIRS)

ayfly_LDADD = z80ex/libz80ex.a gui/libgui.a
AM_CXXFLAGS = $(AYFLY_CXXFLAGS)


if IS_WINDOWS
EXTRALIBS += windows/libwindows.a $(DX_LIBS)
else
EXTRALIBS += unix/libunix.a $(SDL_LIBS)
endif

ayfly_LDADD += $(EXTRALIBS) $(AYFLY_LIBS)

datarootdir = @datarootdir@
