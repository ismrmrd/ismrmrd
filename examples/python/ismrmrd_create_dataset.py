# coding: utf-8
import os
import ismrmrd
import ismrmrd_xsd
import numpy as np
import matplotlib.pyplot as plt


filename = 'testdata.h5'
if os.path.isfile(filename):
    os.remove(filename)
# Create an empty ISMRMRD dataset
dset = ismrmrd.IsmrmrdDataset(filename, 'dataset')

# Synthesize the object
nX, nY = 256, 256
rho = np.zeros((nX, nY))
x0, x1 = nX / 4, 3 * nX / 4
y0, y1 = nY / 4, 3 * nY / 4
rho[x0:x1, y0:y1] = 1

plt.imshow(rho)

# Synthesize some coil sensitivities
X, Y = np.meshgrid(np.arange(nX) / nX / 2.0, np.arange(nY) / nY / 2.0)
C = np.zeros((nX, nY, 4), dtype=np.complex64)
C[:,:,0] = np.exp(-((X - 0.5) ** 2 + Y ** 2) + 1j * (X - 0.5))
C[:,:,1] = np.exp(-((X + 0.5) ** 2 + Y ** 2) - 1j * (X + 0.5))
C[:,:,2] = np.exp(-(X ** 2 + (Y - 0.5) ** 2) + 1j * (Y - 0.5))
C[:,:,3] = np.exp(-(X ** 2 + (Y + 0.5) ** 2) - 1j * (Y + 0.5))
ncoils = np.size(C, 2)

# Synthesize the k-space data
nreps = 5
noiselevel = 0.05
K = np.zeros((nX, nY, ncoils, nreps), dtype=np.complex64)
for rep in range(nreps):
    for coil in range(ncoils):
        noise = noiselevel * (np.random.randn(nX, nY) + 1j * np.random.randn(nX, nY))
        K[:,:,coil,rep] = np.fft.fftshift(np.fft.fft2(np.fft.fftshift(C[:,:,coil] * rho + noise)))

rep0 = np.sqrt(np.sum(np.abs(K) ** 2, 2))
plt.imshow(rep0[:,:,0])

# Append a block of acquisitions, one repetition at a time (faster than one acquisition at a time)
# Re-use the same Acquisition object because why not?
acq = ismrmrd.Acquisition()
for rep in range(nreps):
    for line in range(nY):
        head = ismrmrd.AcquisitionHeader()
        head.version = 1
        head.number_of_samples = nX
        head.center_sample = nX / 2
        head.active_channels = ncoils
        rd, pd, sd = ismrmrd.floatArray(3), ismrmrd.floatArray(3), ismrmrd.floatArray(3)
        rd[0] = 1; pd[1] = 1; sd[2] = 1
        head.read_dir = rd
        head.phase_dir = pd
        head.slice_dir = sd
        head.scan_counter = rep * nY + line
        head.idx.kspace_encode_step_1 = line
        head.idx.repetition = rep

        # Note: the correct API for setting Acquisition flags looks like this:
        #   acq.setFlag(ismrmrd.FlagBit(ismrmrd.ACQ_FIRST_IN_ENCODE_STEP1))
        # but using this API would require using only ugly "acq.setXXX" methods
        # since the call to "acq.setHead()" below overwrites the Acquisition's header
        head.flags = 0
        if line == 0:
            head.flags |= 1 << ismrmrd.ACQ_LAST_IN_ENCODE_STEP1
            head.flags |= 1 << ismrmrd.ACQ_FIRST_IN_SLICE
            head.flags |= 1 << ismrmrd.ACQ_FIRST_IN_REPETITION
        elif line == nY - 1:
            head.flags |= 1 << ismrmrd.ACQ_LAST_IN_ENCODE_STEP1
            head.flags |= 1 << ismrmrd.ACQ_LAST_IN_SLICE
            head.flags |= 1 << ismrmrd.ACQ_LAST_IN_REPETITION

        acq.setHead(head)
        # copy the array to ensure it's a continuous block of memory
        data = np.zeros((2 * nX, ncoils), dtype=np.float32)
        data[::2] = np.array([c.real for c in np.array(K[:,line,:,rep])])
        data[1::2] = np.array([c.imag for c in np.array(K[:,line,:,rep])])
        acq.setData(data.flatten())
        dset.appendAcquisition(acq)


# Fill the XML header
header = ismrmrd_xsd.ismrmrdHeader()

# Experimental Conditions
exp = ismrmrd_xsd.experimentalConditionsType()
exp.H1resonanceFrequency_Hz = 128000000
header.experimentalConditions = exp

# Acquisition System Information
sys = ismrmrd_xsd.acquisitionSystemInformationType()
sys.receiverChannels = ncoils
header.acquisitionSystemInformation = sys

# Encoding
encoding = ismrmrd_xsd.encoding()
encoding.trajectory = ismrmrd_xsd.trajectoryType.cartesian

# Encoded Space
fov = ismrmrd_xsd.fieldOfView_mm()
fov.x = 256
fov.y = 256
fov.z = 5

matrix = ismrmrd_xsd.matrixSize()
matrix.x = np.size(K, 0)
matrix.y = np.size(K, 1)
matrix.z = 1

space = ismrmrd_xsd.encodingSpaceType()
space.matrixSize = matrix
space.fieldOfView_mm = fov

# Set encoded and recon space (same)
encoding.encodedSpace = space
encoding.reconSpace = space

# Encoding limits
limits = ismrmrd_xsd.encodingLimitsType()

limits0 = ismrmrd_xsd.limitType()
limits0.minimum = 0
limits0.center = np.size(K, 0) / 2
limits0.maximum = np.size(K, 0) - 1
limits.kspaceEncodingStep0 = limits0

limits1 = ismrmrd_xsd.limitType()
limits1.minimum = 0
limits1.center = np.size(K, 1) / 2
limits1.maximum = np.size(K, 1) - 1
limits.kspaceEncodingStep1 = limits1

limits_rep = ismrmrd_xsd.limitType()
limits_rep.minimum = 0
limits_rep.center = nreps / 2
limits_rep.maximum = nreps - 1
limits.repetition = limits_rep

limits_slice = ismrmrd_xsd.limitType()
limits_slice.minimum = 0
limits_slice.center = 0
limits_slice.maximum = 0
limits.slice = limits_slice

limits_rest = ismrmrd_xsd.limitType()
limits_rest.minimum = 0
limits_rest.center = 0
limits_rest.maximum = 0
limits.average = limits_rest
limits.contrast = limits_rest
limits.kspaceEncodingStep2 = limits_rest
limits.phase = limits_rest
limits.segment = limits_rest
limits.set = limits_rest

encoding.encodingLimits = limits
header.encoding.append(encoding)

dset.writeHeader(header.toxml('utf-8'))
dset.close()
