lib_LTLIBRARIES	= libudis86.la

include_ladir = ${includedir}
libudis86_ladir = ${includedir}/libudis86

libudis86_la_DATA = x86optable.xml

libudis86_la_SOURCES =	itab.c \
			input.c \
			decode.c \
			syn.c \
			syn-intel.c \
			syn-att.c \
			udis86.c \
			input.h \
			syn.h \
			decode.h \
			extern.h \
			types.h \
			itab.h \
			x86optable.xml \
			opgen.py

BUILT_SOURCES        =  itab.c \
			itab.h

include_la_HEADERS   = udis86.h
libudis86_la_HEADERS = types.h extern.h itab.h

# DLLs may not contain undefined symbol references.  We have the linker
# check this explicitly.
if TARGET_WINDOWS
libudis86_la_LDFLAGS=-no-undefined -version-info 0:0:0
endif

itab.c: x86optable.xml opgen.py itab.h
	python ./opgen.py

itab.h: x86optable.xml opgen.py
	python ./opgen.py


