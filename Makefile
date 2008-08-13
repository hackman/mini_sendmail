# Makefile for mini_sendmail

# CONFIGURE: If you are using a SystemV-based operating system, such as
# Solaris, you will need to uncomment this definition.
#SYSV_LIBS =    -lnsl -lsocket

BINDIR =	/usr/local/sbin
MANDIR =	/usr/local/man
CC =		gcc
CFLAGS =	-O 
LDFLAGS =	-s -static
LDFLAGS2 =  -s
LDLIBS =	$(SYSV_LIBS)

CC :=		$(DIET) $(CC)


all:		mini_sendmail

diet:
	make DIET=diet mini_sendmail


mini_sendmail:	mini_sendmail.o
	$(CC) $(CFLAGS) $(LDFLAGS2) mini_sendmail.o $(LDLIBS) -o mini_sendmail

mini_sendmail-static:
	$(CC) $(CFLAGS) $(LDFLAGS) mini_sendmail.o $(LDLIBS) -o mini_sendmail-static

mini_sendmail.o:	mini_sendmail.c version.h
	$(CC) $(CFLAGS) -c mini_sendmail.c


install:	all
	rm -f $(BINDIR)/mini_sendmail
	cp mini_sendmail $(BINDIR)
	rm -f $(MANDIR)/man8/mini_sendmail.8
	cp mini_sendmail.8 $(MANDIR)/man8

clean:
	rm -f mini_sendmail *.o core core.* *.core

tar:
	@name=`sed -n -e '/#define VERSION/!d' -e 's,.*mini_sendmail/,mini_sendmail-,' -e 's, .*,,p' version.h` ; \
	  rm -rf $$name ; \
	  mkdir $$name ; \
	  tar cf - `cat FILES` | ( cd $$name ; tar xfBp - ) ; \
	  chmod 644 $$name/Makefile ; \
	  tar cfz $$name.tar.gz $$name ; \
	  rm -rf $$name ; 
