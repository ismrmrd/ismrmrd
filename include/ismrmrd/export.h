#ifndef ISMRMRD_EXPORT_H_
#define ISMRMRD_EXPORT_H_

#if defined(WIN32)
#if defined(ISMRMRD_EXPORT)
#define EXPORTISMRMRD __declspec(dllexport)
#elif defined(ISMRMRD_IMPORT)
#define EXPORTISMRMRD __declspec(dllimport)
#else
#define EXPORTISMRMRD
#endif
#else
#define EXPORTISMRMRD
#endif

#endif /* ISMRMRD_EXPORT_H_ */
