import ismrmrd
import ismrmrd_xsd
import numpy as np

f = ismrmrd.IsmrmrdDataset('./testdata.h5', '/dataset')

a = f.readAcquisition(10)
#print a.getData()
#print(a.getPosition(0), a.getPosition(1), a.getPosition(2))
#print(a.getReadDirection(0), a.getReadDirection(1), a.getReadDirection(2))

d = a.getData()
d2 = np.zeros(d.shape)
a.setData(d2)

i = f.readImage_ushort('the_square')
print(i.getReadDirection(0), i.getReadDirection(1), i.getReadDirection(2))

xml = f.readHeader()
hdr = ismrmrd_xsd.CreateFromDocument(xml)

print hdr.experimentalConditions.H1resonanceFrequency_Hz
