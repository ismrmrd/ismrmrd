/*
 * ismrmrd_phanthom.cpp
 *
 *  Created on: Apr 1, 2013
 *      Author: Michael S. Hansen
 */

#include "ismrmrd_phantom.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <cstring>

namespace ISMRMRD {


boost::shared_ptr<NDArray<complex_float_t> > phantom(std::vector<PhantomEllipse>& ellipses, unsigned int matrix_size)
{
    std::vector<size_t> dims(2,matrix_size);
    boost::shared_ptr<NDArray<complex_float_t> > out(new NDArray<complex_float_t>(dims));
    memset(out->getDataPtr(), 0, out->getDataSize());
    for (std::vector<PhantomEllipse>::iterator it = ellipses.begin(); it != ellipses.end(); it++) {
        for (unsigned int y = 0; y < matrix_size; y++) {
            float y_co = (1.0*y-(matrix_size>>1))/(matrix_size>>1);
            for (unsigned int x = 0; x < matrix_size; x++) {
                float x_co = (1.0*x-(matrix_size>>1))/(matrix_size>>1);
                if (it->isInside(x_co,y_co)) {
                    (*out)(x,y) += std::complex<float>(it->getAmplitude(),0.0);
                }
            }
        }
    }
    return out;
}


boost::shared_ptr<NDArray<complex_float_t> > shepp_logan_phantom(unsigned int matrix_size)
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

boost::shared_ptr<NDArray<complex_float_t> > generate_birdcage_sensititivies(unsigned int matrix_size, unsigned int ncoils, float relative_radius)
{
    //This function is heavily inspired by the mri_birdcage.m Matlab script in Jeff Fessler's IRT packake
    //http://web.eecs.umich.edu/~fessler/code/

    std::vector<size_t> dims(2,matrix_size);
    dims.push_back(ncoils);
    boost::shared_ptr<NDArray<complex_float_t> > out(new NDArray<complex_float_t>(dims));
    memset(out->getDataPtr(), 0, out->getDataSize());

    for (unsigned int c = 0; c < ncoils; c++) {
        float coilx = relative_radius*std::cos(c*(2*3.14159265359/ncoils));
        float coily = relative_radius*std::sin(c*(2*3.14159265359/ncoils));
        float coil_phase = -c*(2*3.14159265359/ncoils);
        for (unsigned int y = 0; y < matrix_size; y++) {
            float y_co = (1.0*y-(matrix_size>>1))/(matrix_size>>1)-coily;
            for (unsigned int x = 0; x < matrix_size; x++) {
                float x_co = (1.0*x-(matrix_size>>1))/(matrix_size>>1)-coilx;
                float rr = std::sqrt(x_co*x_co+y_co*y_co);
                float phi = atan2(x_co, -y_co) + coil_phase;
                (*out)(x,y,c) = std::polar(1 / rr, phi);
            }
        }
    }

    return out;
}


boost::mt19937& get_noise_seed()
{
    static boost::mt19937 rng;
    return rng;
}

int add_noise(NDArray<complex_float_t> & a, float sd)
{

    boost::normal_distribution<float> nd(0.0, sd);
    boost::variate_generator<boost::mt19937&,
                             boost::normal_distribution<float> > var_nor(get_noise_seed(), nd);

    for (size_t i = 0; i < a.getNumberOfElements(); i++) {
        a.getDataPtr()[i] += std::complex<float>(var_nor(),var_nor());
    }

    return 0;
}

int add_noise(Acquisition& a, float sd)
{

    boost::normal_distribution<float> nd(0.0, sd);
    boost::variate_generator<boost::mt19937&,
                             boost::normal_distribution<float> > var_nor(get_noise_seed(), nd);

    for (uint16_t c=0; c<a.active_channels(); c++) {
        for (uint16_t s=0; s<a.number_of_samples(); s++) {
            a.data(s,c) += std::complex<float>(var_nor(), var_nor());
        }
    }

    return 0;
}
};
