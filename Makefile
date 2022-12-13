CFLAGS=-g -Wall -Werror

all: tests lib_tar.o submit perso

lib_tar.o: lib_tar.c lib_tar.h

tests: tests.c lib_tar.o

clean:
	rm -f lib_tar.o tests soumission.tar

submit: all
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c *.h *.c Makefile tests.dSYM test/ > soumission.tar

perso:
	cp tests soumission.tar /home/juvanton/Documents/syst-infoprojet2/