# coding: utf-8

import os
import ismrmrd
import ismrmrd_xsd
import numpy as np
from numpy.fft import ifft, fftshift
import matplotlib.pyplot as plt

# Load file
filename = 'testdata.h5'
if not os.path.isfile(filename):
    print("%s is not a valid file" % filename)
    raise SystemExit
dset = ismrmrd.IsmrmrdDataset(filename, 'dataset', create_if_needed=False)

header = ismrmrd_xsd.CreateFromDocument(dset.readHeader())
enc = header.encoding[0]

# Matrix size
eNx = enc.encodedSpace.matrixSize.x
eNy = enc.encodedSpace.matrixSize.y
eNz = enc.encodedSpace.matrixSize.z
rNx = enc.reconSpace.matrixSize.x
rNy = enc.reconSpace.matrixSize.y
rNz = enc.reconSpace.matrixSize.z

# Field of View
eFOVx = enc.encodedSpace.fieldOfView_mm.x
eFOVy = enc.encodedSpace.fieldOfView_mm.y
eFOVz = enc.encodedSpace.fieldOfView_mm.z
rFOVx = enc.reconSpace.fieldOfView_mm.x
rFOVy = enc.reconSpace.fieldOfView_mm.y
rFOVz = enc.reconSpace.fieldOfView_mm.z

# Number of Slices, Reps, Contrasts, etc.
nslices = enc.encodingLimits.slice.maximum + 1
ncoils = header.acquisitionSystemInformation.receiverChannels
nreps = enc.encodingLimits.repetition.maximum + 1
ncontrasts = enc.encodingLimits.contrast.maximum + 1

# TODO: Ignore noise scans
for acqnum in range(dset.getNumberOfAcquisitions()):
    acq = dset.readAcquisition(acqnum)
    if acq.getHead().flags & ismrmrd.ACQ_IS_NOISE_MEASUREMENT:
        print("Found noise measurement @ %d" % acqnum)

all_data = np.zeros((eNx, eNy, eNz, ncoils, nslices, ncontrasts, nreps), dtype=np.complex64)
for acqnum in range(dset.getNumberOfAcquisitions()):
    acq = dset.readAcquisition(acqnum)
    head = acq.getHead()
    rep = head.idx.repetition
    contrast = head.idx.contrast
    slice = head.idx.slice
    y = head.idx.kspace_encode_step_1
    z = head.idx.kspace_encode_step_2
    floatdata = acq.getData()
    data = np.array([complex(elem[0], elem[1]) for elem in zip(floatdata[::2], floatdata[1::2])])
    data = data.reshape((eNx, ncoils))
    all_data[:, y, z, :, slice, contrast, rep] = data

fig = plt.figure()
h, w = nreps * ncontrasts, eNz * nslices
i = 0
for rep in range(nreps):
    for contrast in range(ncontrasts):
        for slice in range(nslices):
            for z in range(eNz):
                K = all_data[:,:,z,:,slice, contrast, rep]
                comb = np.sqrt(np.sum(np.abs(K) ** 2, 2))
                a = fig.add_subplot(h, w, i)
                plt.imshow(comb)
                i += 1
fig.set_size_inches(16, 16)

images = []
for rep in range(nreps):
    for contrast in range(ncontrasts):
        for slice in range(nslices):
            K = all_data[:,:,:,:,slice, contrast, rep]
            K = fftshift(ifft(fftshift(K, axes=0), axis=0), axes=0)

            # chop if needed
            if eNx != rNx:
                i0 = (eNx - rNx) / 2
                i1 = (eNx - rNx) / 2 + rNx
                im = K[i0:i1,:,:,:]
            else:
                im = K

            im = fftshift(ifft(fftshift(im, axes=1), axis=1), axes=1)
            if np.size(im, 2) > 1:
                im = fftshift(ifft(fftshift(im, axes=2), axis=2), axes=2)

            im = np.squeeze(np.sqrt(np.sum(np.abs(im) ** 2, 3)))
            images.append(im)

l = len(images)
fig = plt.figure()
for n, im in enumerate(images):
    a = fig.add_subplot(1, 5, n)
    plt.imshow(im)
fig.set_size_inches(16, 4)

# grab the first acquisition for extra info
acqh = dset.readAcquisition(0).getHead()

for n, img in enumerate(images):
    hdr = ismrmrd.ImageHeader()
    hdr.acquisition_time_stamp = acqh.acquisition_time_stamp
    hdr.flags = 0
    hdr.measurement_uid = acqh.measurement_uid
    hdr.phase_dir = acqh.phase_dir
    hdr.physiology_time_stamp = acqh.physiology_time_stamp
    hdr.position = acqh.position
    hdr.read_dir = acqh.read_dir
    hdr.slice_dir = acqh.slice_dir
    hdr.channels = 1
    hdr.image_data_type = ismrmrd.DATA_FLOAT
    hdr.image_type = ismrmrd.TYPE_MAGNITUDE
    hdr.image_index = n
    hdr.slice = n

    dset.appendImageHeader(hdr, "image_%d.hdr" % n)
    dset.appendArray(img, "image_%d.img" % n)
