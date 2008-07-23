INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/z80ex -I$(top_srcdir)/src/z80ex/include $(AYFLY_CXXFLAGS) $(all_includes)

if IS_UNIX
EXTRAINCLUDES += $(SDL_CFLAGS)
endif

if IS_WINDOWS
EXTRAINCLUDES += $(DX_CFLAGS)
endif

INCLUDES += $(EXTRAINCLUDES)

noinst_LIBRARIES = libgui.a
libgui_a_SOURCES = AyflyApp.cpp AyflyFrame.cpp DnDFiles.cpp AyflyBindingsDlg.cpp

datarootdir = @datarootdir@
