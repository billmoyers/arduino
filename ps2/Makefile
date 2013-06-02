NAME=Keyboard
HS=Keyboard.hs
HSDEP=PS2.hs
CDEP=before.c after.c
SRCS=Keyboard.c
INCLUDES=Keyboard.h

include ../makefile/Makefile.arduino

$(SRCS): $(HS) $(HSDEP) $(CDEP)
	runhaskell $(HS)

clean: clean-arduino
	rm -f $(SRCS) $(INCLUDES)

upload: upload-arduino

stty:
	stty -F $(SERIALDEV) cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts 

test: stty test-arduino

