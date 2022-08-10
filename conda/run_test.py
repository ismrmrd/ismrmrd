import os
import h5py
import numpy as np
import pytest
from pathlib import Path
def test_basic_recon():
    testdata_filename = Path('testdata.h5')

    # Generate test data with no noise
    os.system("ismrmrd_generate_cartesian_shepp_logan -o " + str(testdata_filename) + " -n 0.0")

    # Reconstruct
    os.system("ismrmrd_recon_cartesian_2d " + str(testdata_filename))

    input_data = h5py.File(str(testdata_filename))
    coil_images = input_data['dataset']['coil_images']

    
    coil_images = np.squeeze(coil_images['real'] + 1j*coil_images['imag']) 
    recon_reference = np.abs(np.sqrt(np.sum(coil_images * np.conj(coil_images), 0)))
    ro_length = recon_reference.shape[-1]
    recon_reference = recon_reference[:, int(ro_length/4):int(ro_length/4+ro_length/2)]

    # We need to scale the images, since the recon does not
    reconstruction = np.squeeze(np.array(input_data['dataset']['cpp']['data'])) / np.sqrt(coil_images.shape[-1]*coil_images.shape[-2])

    assert np.linalg.norm(reconstruction - recon_reference) / np.linalg.norm(recon_reference) < 2e-5

    testdata_filename.unlink()


# We will invoke pytest here
pytest.main([__file__])