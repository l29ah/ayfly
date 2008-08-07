INCLUDES = -I$(top_srcdir)/src/libayfly $(SDL_CFLAGS) $(DX_CFLAGS) $(all_includes)
SUBDIRS = z80ex
noinst_LIBRARIES = libayfly.a
libayfly_a_SOURCES = AbstractAudio.cpp ay.cpp formats.cpp speccy.cpp

if IS_WINDOWS
EXTRASUBDIRS += windows
else
EXTRASUBDIRS += unix
endif

SUBDIRS += $(EXTRASUBDIRS)

if IS_WINDOWS
EXTRALIBS += $(top_srcdir)/libayfly/windows/libwindows.a
else
EXTRALIBS += $(top_srcdir)/libayfly/unix/libunix.a
endif

libayfly_a_LIBADD += $(EXTRALIBS) $(top_srcdir)/libayfly/z80ex/libz80ex.a

datarootdir = @datarootdir@
