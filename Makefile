# $Id: Makefile,v 1.18 2003/07/28 19:11:12 cepek Exp $
#
# this Makefile and all files in ./scripts were tested on Debian GNU/Linux 2.2
#


.PHONY : archive


all:
	@if [ "$(CXX)" = "g++" ]; then					\
	   if awk 'BEGIN {if ($(shell g++ --version) < 3.0)		\
	      exit 0; else exit 1}' ;					\
	   then								\
	      echo ;							\
	      echo your CXX variable is set to g++ compiler version	\
	           $(shell g++ --version);				\
	      echo but g++ 3.0 or later is needed for building		\
                   GNU Gama;						\
	      echo ;							\
	      exit 1;							\
	   fi;								\
	fi
	( cd gamaprog/linux/lib; make -f Makefile-expat; make )
	( cd gamaprog/linux/gama-local ; make )

help:
	@echo
	@echo -e make "    " \
	"\t#" runs make on makefiles created by \`make dep\'
	@echo
	@echo -e make dep \
	"\t#" creates all Makefiles
	@echo
	@echo "     aditional" \
	options can be passed to g++ with export CXXFLAGS=\"...\"
	@echo
	@echo -e make example \
	"\t#" runs program gama on an example input data
	@echo -e make archive \
	"\t#" creates tar archive from working directory
	@echo -e make help    \
	"\t#" this screen
	@echo

dep:
	./scripts/build-makefiles

build:
	./scripts/build-dictionaries
	./scripts/build-ellipsoids

example:
	@if [ ! -x  gamaprog/linux/gama-local/gama-local ]; \
		then make make-project; fi
	(cd gamaprog/linux/gama-local/examples; \
	../gama-local gama-xml.gkf test. ; cat test.txt )

archive:
	./scripts/build-archive

clean:
	rm -f `find gnu_gama gamalib gamaprog scripts -name *\.[o]`
	rm -f `find gnu_gama gamalib gamaprog scripts -name *\.[a]`
	rm -f `find gnu_gama gamalib -name leak.out`
	rm -f `find gnu_gama gamalib -name demo*`
	rm -f `find . -name "*[\~\#]*"`
	rm -f `find gamaprog/linux/gama-local/examples -name test*`

# -----------------------------------------------------------------------

