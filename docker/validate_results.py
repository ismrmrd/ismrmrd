import os
import argparse
import h5py
import numpy as np
from pathlib import Path

def test_basic_recon(reference_filename, testdata_filename):
    reference_data = h5py.File(reference_filename)
    input_data = h5py.File(testdata_filename)
    coil_images = reference_data['dataset']['coil_images']
    coil_images = np.squeeze(np.array(coil_images['real']) + 1j*np.array(coil_images['imag']))
    recon_reference = np.abs(np.sqrt(np.sum(coil_images * np.conj(coil_images), 0)))
    ro_length = recon_reference.shape[-1]
    recon_reference = recon_reference[:, int(ro_length/4):int(ro_length/4+ro_length/2)]

    # Take care of the different data types
    if (len(input_data['dataset']['image_0']['data'].dtype)) == 2:
        reconstruction = np.squeeze(np.array(input_data['dataset']['image_0']['data']['real']) + 1j*np.array(input_data['dataset']['image_0']['data']['imag']))
    else:
        reconstruction = np.squeeze(np.array(input_data['dataset']['image_0']['data']))

    # We need to scale the images, since the recon does not
    reconstruction = np.abs(reconstruction) / np.sqrt(coil_images.shape[-1]*coil_images.shape[-2])

    assert np.linalg.norm(reconstruction - recon_reference) / np.linalg.norm(recon_reference) < 2e-5

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Validate results of an ismrmrd reconstruction')
    parser.add_argument('--reference', type=str, help='Reference file', required=True)
    parser.add_argument('--testdata', type=str, help='Test data file', required=True)
    args = parser.parse_args()
    test_basic_recon(args.reference, args.testdata)
