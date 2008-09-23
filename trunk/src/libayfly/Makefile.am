INCLUDES = -I$(top_srcdir)/src/libayfly -I$(top_srcdir)/src/libayfly/z80ex -I$(top_srcdir)/src/libayfly/z80ex/include $(SDL_CFLAGS) $(DX_CFLAGS) $(all_includes)
SUBDIRS = z80ex
noinst_LIBRARIES = libayfly.a
libayfly_a_SOURCES = AbstractAudio.cpp ay.cpp formats.cpp speccy.cpp lha.cpp common.cpp ayflyString.cpp

if IS_WINDOWS
EXTRASUBDIRS += windows
else
EXTRASUBDIRS += unix
endif

SUBDIRS += $(EXTRASUBDIRS)

if IS_WINDOWS
EXTRALIBS += $(top_builddir)/src/libayfly/windows/DXAudio.o
else
EXTRALIBS += $(top_builddir)/src/libayfly/unix/SDLAudio.o
endif

libayfly_a_LIBADD += $(EXTRALIBS) $(top_builddir)/src/libayfly/z80ex/z80ex.o

datarootdir = @datarootdir@
