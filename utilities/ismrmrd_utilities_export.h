/*
 * ismrmrd_utilities_export.h
 *
 *  Created on: Apr 1, 2013
 *      Author: Michael S. Hansen
 */

#ifndef ISMRMRD_UTILITIES_EXPORT_H_
#define ISMRMRD_UTILITIES_EXPORT_H_

#if defined (WIN32)
#if defined (ismrmrd_utilities_EXPORTS)
#define EXPORTISMRMRDUTILITIES __declspec(dllexport)
#else
#define EXPORTISMRMRDUTILITIES __declspec(dllimport)
#endif
#else
#define EXPORTISMRMRDUTILITIES
#endif

#endif /* ISMRMRD_UTILITIES_EXPORT_H_ */
