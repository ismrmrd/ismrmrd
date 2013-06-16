#ifndef ISMRMRD_EXPORT_H_
#define ISMRMRD_EXPORT_H_

#if defined (WIN32)
#if defined (ismrmrd_EXPORTS)
#define EXPORTISMRMRD __declspec(dllexport)
#else
#define EXPORTISMRMRD __declspec(dllimport)
#endif
#else
#define EXPORTISMRMRD
#endif

#endif /* ISMRMRD_EXPORT_H_ */
