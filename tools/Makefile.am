
AUTOMAKE_OPTIONS = foreign
GIT_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)

AM_CPPFLAGS = -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\"" -D_FILE_OFFSET_BITS=64 \
	-D_BSD_SOURCE \
	-I$(BLACKMAGIC_SDK_PATH) \
	-I$(top_srcdir)/src \
	-DGIT_VERSION=\"$(GIT_VERSION)\" \
	-DKL_USERSPACE -D__STDC_FORMAT_MACROS

AM_CFLAGS = -Wall -O3

if DEBUG
	CFLAGS += -g
endif

CXXFLAGS = $(CFLAGS) -std=c++11

SRC  = osd.c
SRC += videoformats.c
SRC += v210.cpp
SRC += filter.cpp
SRC += filtervectorscope.cpp
SRC += framebase.cpp
SRC += frameprocessor.cpp
SRC += frameformat.cpp
SRC += framequeue.cpp
SRC += framesync.cpp
SRC += framemetadata.cpp
SRC += frameinput.cpp
SRC += frameinputdecklink.cpp
SRC += frameinputdecklink2.cpp
SRC += frameoutput.cpp
SRC += frameoutputdecklink.cpp
SRC += frameoutputdecklink2.cpp
SRC += frameoutputndi.cpp
SRC += framepipe.cpp
SRC += framepipe_burnwriter.cpp

bin_PROGRAMS  = cello

cello_SOURCES = $(SRC)

