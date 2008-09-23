OBJS = AbstractAudio.o ay.o common.o formats.o speccy.o DXAudio.o z80ex.o ayflyString.o
DX_INCLUDES = -I/opt/directx/include
DX_LIBS = /opt/directx/lib/dxguid.lib /opt/directx/lib/dsound.lib 
INCLUDES = -I../../src/libayfly -I../../src/libayfly/z80ex -I../../src/libayfly/z80ex/include $(DX_INCLUDES) -DAYFLY_EXPORTS
LIBS = $(DX_LIBS) -lwinmm -lole32 -luuid -lsupc++

CFLAGS = -O3 -g0 -s -ffast-math
CC = mingw32-gcc
CXX = mingw32-gcc

all: ayfly.dll

ayfly.dll: $(OBJS)
	$(CC) -s -shared -Wl,-subsystem,windows -o ayfly.dll $(OBJS) $(LIBS)

clean:
	rm -rf *.o *.dll

AbstractAudio.o: ../../src/libayfly/AbstractAudio.cpp
	$(CXX) -c $(INCLUDES) $(CFLAGS) ../../src/libayfly/AbstractAudio.cpp -o AbstractAudio.o
ay.o: ../../src/libayfly/ay.cpp
	$(CXX) -c $(INCLUDES) $(CFLAGS) ../../src/libayfly/ay.cpp -o ay.o
common.o: ../../src/libayfly/common.cpp
	$(CXX) -c $(INCLUDES) $(CFLAGS) ../../src/libayfly/common.cpp -o common.o
formats.o: ../../src/libayfly/formats.cpp
	$(CXX) -c $(INCLUDES) $(CFLAGS) ../../src/libayfly/formats.cpp -o formats.o
speccy.o: ../../src/libayfly/speccy.cpp
	$(CXX) -c $(INCLUDES) $(CFLAGS) ../../src/libayfly/speccy.cpp -o speccy.o
DXAudio.o: ../../src/libayfly/windows/DXAudio.cpp
	$(CXX) -c $(INCLUDES) $(CFLAGS) ../../src/libayfly/windows/DXAudio.cpp -o DXAudio.o
z80ex.o: ../../src/libayfly/z80ex/z80ex.c
	$(CC) -c $(INCLUDES) $(CFLAGS) ../../src/libayfly/z80ex/z80ex.c -o z80ex.o
ayflyString.o: ../../src/libayfly/ayflyString.cpp
	$(CXX) -c $(INCLUDES) $(CFLAGS) ../../src/libayfly/ayflyString.cpp -o ayflyString.o
