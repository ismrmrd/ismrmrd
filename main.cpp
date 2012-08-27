#include <iostream>

#include "ismrmrd.hxx"
#include "ismrmrd_hdf5.h"

using namespace ISMRMRD;

template <typename T, int length> int appendArray(IsmrmrdDataset& d, const char* varname)
{
	T a[length];
	std::vector<unsigned int> dims(1,length);
	NDArrayContainer<T> tmp(dims,a);
	return d.appendArray(tmp, varname);
}

template <typename T, int mat_x, int mat_y, int mat_z, int chan> int appendImage(IsmrmrdDataset& d, const char* varname)
{
	Image< T > img;
	img.head_.matrix_size[0] = mat_x;
	img.head_.matrix_size[1] = mat_y;
	img.head_.matrix_size[2] = mat_z;
	img.head_.channels = chan;
	img.data_ = new T[img.getNumberOfElements()];

	for (size_t i = 0; i < img.getNumberOfElements(); i++) {
		img.data_[i] = i;
	}

	d.appendImage(img,varname);

}

int main (int argc, char* argv[])
{

	IsmrmrdDataset d("test3.h5","/mytestvar");

	appendArray<double, 10>(d,"blah");

	appendImage<float, 128, 128, 10, 1>(d,"myfloatimage");
	appendImage<double, 128, 128, 1, 32>(d,"mydoubleimage");
	appendImage< std::complex<float>, 256, 256, 1, 1>(d,"mycxfloatimage");
	appendImage<unsigned short, 128, 128, 10, 8>(d,"myshortimage");


	boost::shared_ptr< Image< unsigned short > > img = d.readImage< unsigned short >("myshortimage", 0);

	if (img->getNumberOfElements() == 0) {
		std::cout << "Zero image read" << std::endl;
		return -1;
	}

	for (unsigned int i = 0; i < 100 ; i++) {
		std::cout << "i = " << i << ", " << img->data_[i] << std::endl;
	}

	//Now let's append the image with header and data separately
	d.appendImageHeader(img->head_,"mynewshortimage.head");
	std::vector<unsigned int> dimensions;
	dimensions.push_back(img->head_.matrix_size[0]);
	dimensions.push_back(img->head_.matrix_size[1]);
	dimensions.push_back(img->head_.matrix_size[2]);
	dimensions.push_back(img->head_.channels);
	d.appendArray(dimensions,img->data_,"mynewshortimage.img");

    return 0;
}
