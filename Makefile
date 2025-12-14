CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = ipcalc
SRC = ipcalc.c
VERSION = 1.1

PREFIX ?= /usr
DESTDIR ?=

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	rm -rf rpmbuild debian-build *.rpm *.deb *.tar.gz

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/

dist:
	mkdir -p $(TARGET)-$(VERSION)
	cp $(SRC) Makefile README.md CHANGELOG.md LICENSE $(TARGET)-$(VERSION)/
	tar -czvf $(TARGET)-$(VERSION).tar.gz $(TARGET)-$(VERSION)
	rm -rf $(TARGET)-$(VERSION)

rpm: dist
	mkdir -p rpmbuild/BUILD
	mkdir -p rpmbuild/RPMS
	mkdir -p rpmbuild/SOURCES
	mkdir -p rpmbuild/SPECS
	mkdir -p rpmbuild/SRPMS
	cp $(TARGET)-$(VERSION).tar.gz rpmbuild/SOURCES/
	cp ipcalc.spec rpmbuild/SPECS/
	rpmbuild --define "_topdir $$(pwd)/rpmbuild" -bb rpmbuild/SPECS/ipcalc.spec
	cp rpmbuild/RPMS/*/*.rpm .

deb: $(TARGET)
	mkdir -p debian-build/DEBIAN
	mkdir -p debian-build$(PREFIX)/bin
	cp debian/control debian-build/DEBIAN/
	cp $(TARGET) debian-build$(PREFIX)/bin/
	dpkg-deb --build debian-build .
