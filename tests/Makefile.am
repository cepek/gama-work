EXTRA_DIST = CMakeLists.txt

if GNU_GAMA_G3_ENABLED
   GAMA_G3 = gama-g3
endif

if GNU_GAMA_LOCAL_EXTRA_TESTS
   EXTRA_TESTS = matvec statan acord2 extra-tests
endif

GAMA_LOCAL = gama-local
GAMA_LOCAL_DEFORMATION = gama-local-deformation
KRUMM = krumm


# uncomment for testing purposes

# undefine GAMA_LOCAL
# undefine GAMA_LOCAL_DEFORMATION
# undefine KRUMM
# undefine GAMA_G3
# undefine EXTRA_TESTS


SUBDIRS = $(GAMA_LOCAL) $(GAMA_LOCAL_DEFORMATION) \
          $(KRUMM) $(GAMA_G3) $(EXTRA_TESTS)
