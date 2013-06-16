/*
 * ismrmrd_xsd_export.h
 *
 *  Created on: Sep 13, 2012
 *      Author: hansenms
 */

#ifndef ISMRMRD_XSD_EXPORT_H_
#define ISMRMRD_XSD_EXPORT_H_

#if defined (WIN32)
#if defined (ismrmrd_xsd_EXPORTS)
#define EXPORTISMRMRDXSD __declspec(dllexport)
#else
#define EXPORTISMRMRDXSD __declspec(dllimport)
#endif
#else
#define EXPORTISMRMRDXSD
#endif

#endif /* ISMRMRD_XSD_EXPORT_H_ */
