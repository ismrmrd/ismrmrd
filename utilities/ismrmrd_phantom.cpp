/*
 * ismrmrd_phanthom.cpp
 *
 *  Created on: Apr 1, 2013
 *      Author: Michael S. Hansen
 */

#include "ismrmrd_phantom.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

namespace ISMRMRD {


boost::shared_ptr<NDArrayContainer< std::complex<float> > > phantom(std::vector<PhantomEllipse>& ellipses, unsigned int matrix_size)
{
	std::vector<unsigned int> dims(2,matrix_size);
	boost::shared_ptr<NDArrayContainer<std::complex<float> > > out(new NDArrayContainer< std::complex<float> >(dims));
	out->data_ = std::complex<float>(0.0,0.0);
	for (std::vector<PhantomEllipse>::iterator it = ellipses.begin(); it != ellipses.end(); it++) {
		for (unsigned int y = 0; y < matrix_size; y++) {
			float y_co = (1.0*y-(matrix_size>>1))/(matrix_size>>1);
			for (unsigned int x = 0; x < matrix_size; x++) {
				size_t index = y*matrix_size + x;
				float x_co = (1.0*x-(matrix_size>>1))/(matrix_size>>1);
				if (it->isInside(x_co,y_co)) {
					out->data_[index] += std::complex<float>(it->getAmplitude(),0.0);
				}
			}
		}
	}
	return out;
}


boost::shared_ptr<NDArrayContainer< std::complex<float> > > shepp_logan_phantom(unsigned int matrix_size)
{
	boost::shared_ptr< std::vector<PhantomEllipse> > e = modified_shepp_logan_ellipses();
	return phantom(*e, matrix_size);
}


boost::shared_ptr< std::vector<PhantomEllipse> > shepp_logan_ellipses()
{
	boost::shared_ptr< std::vector<PhantomEllipse> > out(new std::vector<PhantomEllipse>);
	out->push_back(PhantomEllipse(1,	.69,	.92,	0,		0,		0));
	out->push_back(PhantomEllipse(-.98, .6624, 	.8740,  0,	-.0184,		0));
	out->push_back(PhantomEllipse(-.02, .1100, 	.3100,  .22,	0,	   -18));
	out->push_back(PhantomEllipse(-.02, .1600, .4100, -.22,    0,     18));
	out->push_back(PhantomEllipse(.01, .2100, .2500,   0,    .35,    0));
	out->push_back(PhantomEllipse(.01, .0460, .0460,   0,    .1,     0));
	out->push_back(PhantomEllipse(.01, .0460, .0460,   0,   -.1,     0));
	out->push_back(PhantomEllipse(.01, .0460, .0230, -.08,  -.605,   0));
	out->push_back(PhantomEllipse(.01, .0230, .0230,   0,   -.606,   0));
	out->push_back(PhantomEllipse( .01, .0230, .0460,  .06,  -.605,   0));

	return out;
}

boost::shared_ptr< std::vector<PhantomEllipse> > modified_shepp_logan_ellipses()
{
	boost::shared_ptr< std::vector<PhantomEllipse> > out(new std::vector<PhantomEllipse>);
	out->push_back(PhantomEllipse(  1,   .69,   .92,    0,     0,     0));
	out->push_back(PhantomEllipse(-.8,  .6624, .8740,   0,  -.0184,   0));
	out->push_back(PhantomEllipse(-.2,  .1100, .3100,  .22,    0,    -18));
	out->push_back(PhantomEllipse(-.2,  .1600, .4100, -.22,    0,     18));
	out->push_back(PhantomEllipse(.1,  .2100, .2500,   0,    .35,    0));
	out->push_back(PhantomEllipse(.1,  .0460, .0460,   0,    .1,     0));
	out->push_back(PhantomEllipse(.1,  .0460, .0460,   0,   -.1,     0));
	out->push_back(PhantomEllipse(.1,  .0460, .0230, -.08,  -.605,   0));
	out->push_back(PhantomEllipse(.1,  .0230, .0230,   0,  -.606,   0));
	out->push_back(PhantomEllipse( .1,  .0230, .0460,  .06,  -.605,   0 ));
	return out;
}

boost::shared_ptr<NDArrayContainer< std::complex<float> > > generate_birdcage_sensititivies(unsigned int matrix_size, unsigned int ncoils, float relative_radius)
{
	//This function is heavily inspired by the mri_birdcage.m Matlab script in Jeff Fessler's IRT packake
	//http://web.eecs.umich.edu/~fessler/code/

	std::vector<unsigned int> dims(2,matrix_size);
	dims.push_back(ncoils);
	boost::shared_ptr<NDArrayContainer<std::complex<float> > > out(new NDArrayContainer< std::complex<float> >(dims));
	out->data_ = std::complex<float>(0.0,0.0);

	for (int c = 0; c < ncoils; c++) {
		float coilx = relative_radius*std::cos(c*(2*3.14159265359/ncoils));
		float coily = relative_radius*std::sin(c*(2*3.14159265359/ncoils));
		float coil_phase = -c*(2*3.14159265359/ncoils);
		for (unsigned int y = 0; y < matrix_size; y++) {
			float y_co = (1.0*y-(matrix_size>>1))/(matrix_size>>1)-coily;
			for (unsigned int x = 0; x < matrix_size; x++) {
				size_t index = c*matrix_size*matrix_size+ y*matrix_size + x;
				float x_co = (1.0*x-(matrix_size>>1))/(matrix_size>>1)-coilx;
				float rr = std::sqrt(x_co*x_co+y_co*y_co);
				float phi = atan2(x_co, -y_co) + coil_phase;
				out->data_[index] = std::polar(1 / rr, phi);
			}
		}
	}

	return out;
}

int add_noise(NDArrayContainer< std::complex<float> >& a, float sd)
{

	static boost::mt19937 rng;
	boost::normal_distribution<float> nd(0.0, sd);
	boost::variate_generator<boost::mt19937&,
	                           boost::normal_distribution<float> > var_nor(rng, nd);

	for (size_t i = 0; i < a.data_.size(); i++) {
		a.data_[i] += std::complex<float>(var_nor(),var_nor());
	}

	return 0;
}

};




