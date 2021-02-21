all: dos linux

clean:
	make -f Makefile.linux clean
	make -f Makefile.dos clean

run: linux
	make -f Makefile.linux run

dos:
	make -f Makefile.dos

dosbox:
	make -f Makefile.dos run

linux:
	make -f Makefile.linux

