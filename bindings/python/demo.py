import ismrmrd

acq = ismrmrd.Acquisition()
acq.head.version = 42
print(acq.head.version) 

img = ismrmrd.Image()

f = ismrmrd.Dataset('./testdata.h5', '/dataset', True)
print f.filename
# xml = f.readHeader()
