VERSION=0.1.1
CC=cc
PLANTUML=plantuml
INSTALL=install
CFLAGS=$(shell pkg-config --cflags sqlite3) $(shell pcre2-config --cflags) -fPIC
LIBS=$(shell pcre2-config --libs8)
prefix=/usr

.PHONY : doc check install dist clean

pcre2.so: pcre2.c
	${CC} -shared -o $@ ${CFLAGS} -W -Werror $^ ${LIBS}


doc: test/50-run.svg


%.svg: %.puml
	${PLANTUML} -tsvg $<

# Generate the test spec
test/db.sqlite3: test/10-init/*.sql
	cat $^ | sqlite3 test/db.sqlite3

# Run the test execution
test/50-run.phony: test/db.sqlite3 test/50-run.sh pcre2.so test/lib.sh
	# sqlite3 -cmd '.load ./pcre2' test/db.sqlite3 .selftest
	test/50-run.sh test/db.sqlite3 './pcre2'
	touch $@

# Run the test report commands
check: test/50-run.phony test/70-print-summary.sh
	test/70-print-summary.sh

install: pcre2.so
	sudo ${INSTALL} -p $^ ${prefix}/lib/sqlite3/pcre2.so

dist: clean
	mkdir sqlite3-pcre2-${VERSION}
	cp -f pcre2.c Makefile readme.txt sqlite3-pcre2-${VERSION}
	tar -czf sqlite3-pcre2-${VERSION}.tar.gz sqlite3-pcre2-${VERSION}

clean:
	-rm -f pcre2.so test/db.sqlite3 test/*.phony
