#ifndef ISMRMRD_XML_EXPORT_H_
#define ISMRMRD_XML_EXPORT_H_

#if defined (WIN32)
#if defined (ismrmrd_xml_EXPORTS)
#define EXPORTISMRMRDXML __declspec(dllexport)
#else
#define EXPORTISMRMRDXML __declspec(dllimport)
#endif
#else
#define EXPORTISMRMRDXML
#endif

#endif /* ISMRMRD_XML_EXPORT_H_ */
