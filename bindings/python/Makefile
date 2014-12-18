ISMRMRD_HOME ?= /usr/local

CPPFLAGS = -I$(ISMRMRD_HOME)/include
LDFLAGS = -L$(ISMRMRD_HOME)/lib

ismrmrd.so: ismrmrd.pyx cismrmrd.pxd setup.py
	python setup.py build_ext -i

install: ismrmrd.so
	install -d $(ISMRMRD_HOME)/python
	install $< $(ISMRMRD_HOME)/python/

clean:
	rm ismrmrd.c ismrmrd.so
