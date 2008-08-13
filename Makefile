# order of build of subdirs is important
#
SUBDIRS=src tests

all: Makefiles include/openpgpsdk/configure.h headers default

headers:
	cd include/openpgpsdk && make headers

default:
	@set -e; for d in $(SUBDIRS); do \
	(cd $$d; echo "+++ make in $$d"; make -w; echo "** submake returns $$?\n"; echo "--- $$d"); \
	done

include/openpgpsdk/configure.h: include/openpgpsdk/configure.h.template configure
	echo re-run configure && exit 1

force_depend:
	@set -e; for d in $(SUBDIRS); do \
	(cd $$d; echo "+++ make force_depend in $$d"; make force_depend ; echo "--- $$d"); \
	done

clean:
	@set -e; for d in $(SUBDIRS); do \
	(cd $$d; echo "+++ make clean in $$d"; make clean; echo "--- $$d"); \
	done
	find . -name '*.core' | xargs rm -f
	rm -rf oink-links
	-rm lib/*
	# reinstall the CUnit libs
	(cd CUnit-2.1-0; make install)

Makefiles:
	@set -e; for d in $(SUBDIRS); do \
	(cd $$d; echo "+++ make Makefile in $$d"; make Makefile; echo "--- $$d"); \
	done

tags:
	rm -f TAGS
	find . -name '*.[ch]' | xargs etags

test::
	cd tests && make 

doc::
	cd doc && make

cunit:
	if [ ! -d CUnit-2.1-0 ]; then \
        	tar xvfz CUnit-2.1-0-src.tar.gz; \
        	(cd CUnit-2.1-0 && ./configure --prefix $(PWD) && make && make install); \
	fi


coverity::
	cov-build --dir coverity make
	cd coverity && cov-analyze -e emit/ --outputdir output/ --enable VOLATILE --security --enable CHROOT --enable OPEN_ARGS --enable SECURE_CODING --enable SECURE_TEMP --enable TAINTED_POINTER --enable TOCTTOU && cov-commit-errors -e ./emit -o ./output -d /home/rachel/openpgpsdk/coverity/database/ --name ben

oink:
	rm -rf oink-links
	mkdir oink-links
	cd oink-links \
	&& find ../src ../examples -name '*.[ihc]' -exec ln -s {} \; \
	&& ln -s ../util/Makefile.oink Makefile
