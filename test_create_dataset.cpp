/*
 * test_create_dataset.cpp
 *
 *  Created on: Sep 4, 2012
 *      Author: Michael S. Hansen (michael.hansen@nih.gov)
 */

#include <iostream>
#include "ismrmrd_hdf5.h"

using namespace ISMRMRD;

template <typename T, int size_x, int size_y> int appendImageArray(IsmrmrdDataset& d, const char* varname)
{
	T a[size_x*size_y];
	std::vector<unsigned int> dims(2,0);
	dims[0] = size_x;
	dims[1] = size_y;

	//Let's make a simple square (rectangle depending on dimensions)
	for (int y = 0; y < size_y; y++) {
		for (int x = 0; x < size_x; x++) {
			if ( (x > (size_x>>2)) && (x < (size_x-(size_x>>2))) &&
				 (y > (size_y>>3)) && (y < (size_y-(size_y>>3)))) {
				a[y*size_x + x] = 1.0;
			} else {
				a[y*size_x + x] = 0.0;
			}
		}
	}

	NDArrayContainer<T> tmp(dims,a);

	return d.appendArray(tmp, varname);
}


int main(int argc, char** argv)
{
	std::cout << "ISMRMRD Test Dataset Creation App" << std::endl;

	IsmrmrdDataset d("testdata.h5","dataset");

	//Let's create the "original" image in the file for reference
	if (appendImageArray< std::complex<float>, 256, 128 >(d, "the_square") < 0) {
		std::cout << "Error adding image to dataset" << std::endl;
		return -1;
	}

	//Read it back from the file
	boost::shared_ptr< NDArrayContainer<std::complex<float> > > img_test =
			d.readArray< std::complex<float> >("the_square", 0);

	if (img_test.get() == 0) {
		std::cout << "Error reading image array from file" << std::endl;
		return -1;
	}

	std::cout << "Image Array dimensions: ";
	for (int di = 0; di < img_test->dimensions_.size(); di++) {
		std::cout << img_test->dimensions_[di] << " ";
	}
	std::cout << std::endl;

	return 0;
}


