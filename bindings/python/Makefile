CPPFLAGS = -I$(ISMRMRD_HOME)/include
LDFLAGS = -I$(ISMRMRD_HOME)/lib

# CPPFLAGS="$(CPPFLAGS)" LDFLAGS="$(LDFLAGS)" python setup.py build_ext -i

ismrmrd: ismrmrd.pyx cismrmrd.pxd setup.py
	python setup.py build_ext -i
clean:
	rm ismrmrd.c ismrmrd.so
