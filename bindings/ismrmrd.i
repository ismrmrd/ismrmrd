%module ismrmrd

//%{
//#include "ismrmrd_hdf5.h"
//%}

%include "stdint.i"
%include "std_string.i"

%include "std_vector.i"

%ignore ISMRMRD::Acquisition::getData;

/* ISMRMRD::IsmrmrdDataset */
%newobject ISMRMRD::IsmrmrdDataset::readAcquisition;

%extend ISMRMRD::IsmrmrdDataset {
    std::string readHeader() {
        boost::shared_ptr<std::string> hdr = $self->readHeader();
        return *(hdr.get()); 
    }

    ISMRMRD::Acquisition* readAcquisition(unsigned long index = 0) {
        ISMRMRD::Acquisition* acq = new ISMRMRD::Acquisition(*$self->readAcquisition(index).get());
        return acq;
    }

}

%ignore ISMRMRD::IsmrmrdDataset::readAcquisition;
%ignore ISMRMRD::IsmrmrdDataset::readHeader;


%include "ismrmrd.h"

%define EXPORTISMRMRD %enddef

namespace H5 {
    
}

%include "ismrmrd_hdf5.h"
%include "ismrmrd_hdf5_datatypes.h"

namespace ISMRMRD {
    %template(Image_ushort) Image<unsigned short int>;
    %template(Image_float)  Image<float>;
    %template(Image_double)  Image<double>;
    %template(Image_cplx_float)  Image< std::complex<float> >;
    %template(Image_cplx_double)  Image< std::complex<double> >;
}

