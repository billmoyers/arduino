NAME=AtomLED
SRCS=AtomLED.c
INCLUDES=AtomLED.h

include Makefile.arduino

clean: clean-arduino

upload: upload-arduino
