import ismrmrd

acq = ismrmrd.Acquisition()
# acq.version = 42

f = ismrmrd.Dataset('./testdata.h5', '/dataset', True)
print f.filename
# xml = f.readHeader()

# a = f.readAcquisition(10)

# print(a.getPosition(0), a.getPosition(1), a.getPosition(2))
# print(a.getReadDirection(0), a.getReadDirection(1), a.getReadDirection(2))
