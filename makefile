cc = gcc
ccflags = -Wall -Wno-stringop-truncation
sources = irc.c

main: $(sources)
	${cc} ${ccflags} -o $@ $<

clean:
	rm -rf ./main
