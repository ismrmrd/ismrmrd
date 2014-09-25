/*
 * ismrmrd_fftw.h
 *
 *  Created on: Apr 1, 2013
 *      Author: Michael S. Hansen
 */

#include "fftw3.h"

namespace ISMRMRD {

template<typename TI, typename TO> void circshift(TO *out, const TI *in, int xdim, int ydim, int xshift, int yshift)
{
  for (int i =0; i < ydim; i++) {
    int ii = (i + yshift) % ydim;
    for (int j = 0; j < xdim; j++) {
      int jj = (j + xshift) % xdim;
      out[ii * xdim + jj] = in[i * xdim + j];
    }
  }
}

#define fftshift(out, in, x, y) circshift(out, in, x, y, (x/2), (y/2))


int fft2c(NDArray<complex_float_t> &a, bool forward)
{
    if (a.getNDim() < 2) {
		std::cout << "fft2c Error: input array must have at least two dimensions" << std::endl;
		return -1;
	}

	size_t elements =  a.getDims()[0]*a.getDims()[1];
	size_t ffts = a.getNumberOfElements()/elements;

	//Array for transformation
	fftwf_complex* tmp = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*a.getNumberOfElements());

	if (!tmp) {
		std::cout << "Error allocating temporary storage for FFTW" << std::endl;
		return -1;
	}

	for (size_t f = 0; f < ffts; f++) {
		size_t index = f*elements;
		fftshift(reinterpret_cast<std::complex<float>*>(tmp),&a.getData()[index],a.getDims()[0],a.getDims()[1]);

		//Create the FFTW plan
		fftwf_plan p;
		if (forward) {
			p = fftwf_plan_dft_2d(a.getDims()[1], a.getDims()[0], tmp,tmp, FFTW_FORWARD, FFTW_ESTIMATE);
		} else {
			p = fftwf_plan_dft_2d(a.getDims()[1], a.getDims()[0], tmp,tmp, FFTW_BACKWARD, FFTW_ESTIMATE);
		}
		fftwf_execute(p);

		fftshift(&a.getData()[index],reinterpret_cast<std::complex<float>*>(tmp),a.getDims()[0],a.getDims()[1]);

		//Clean up.
		fftwf_destroy_plan(p);
	}

	std::complex<float> scale(std::sqrt(1.0f*elements),0.0);
        for (size_t n=0; n<a.getNumberOfElements(); n++) {
            a.getData()[n] /= scale;
        }
	fftwf_free(tmp);
	return 0;
}

int fft2c(NDArray<complex_float_t> &a) {
	return fft2c(a,true);
}

int ifft2c(NDArray<complex_float_t> &a) {
	return fft2c(a,false);
}

};
