The ISMRMRD Matlab Interface
============================

The ISMRM raw data file format is based on HDF5.  Each dataset consists of an XML text header and a set of acquisitions or images.  Matlab has very good built-in support for reading HDF5, this makes reading ISMRMRD files into Matlab very straightforward.

XML Header Interface
--------------------
The two methods, readxml and writexml, read and write a XML header
from an ISMRMRD dataset: readxml returns a string writexml takes a
string as input This provides the user with the flexibility to use
whatever tools they prefer to create or modify the XML header string.

Unfortunately, working with XML directly can be difficult and is
error-prone, therefore, the ISMRMRD Matlab interface class provides a
JAVA class for a convient way to interact with the XML header.  This
class is automatically generated from the XML schema describing the
XML header and is therefore guaranteed to be valid.  The examples
provided use this feature to manipulate the XML header.  If you want
to use this feature, then you must have a version that is compiled for
the version of JAVA that your Matlab installation is using.

Installation
------------
The matlab interface consists of a package in a directory called
"+ismrmrd". Warning, the name and the plus sign are important.
Put the +ismrmrd folder somewhere, for example in
/home/jane/ismrmrd/matlab, then add that somewhere to your matlab
search path.

Installing with the Pre-Compiled XML Header Bindings
---------------------------------------------------- 
The compiled XML header JAVA class bundle must be installed and the
Matlab JAVA classpath.  Copy the compiled java class bundle
(xmlhdr.jar) somewhere on your system, for example into the folder
/home/jane/ismrmrd/matlab/+ismrmrd.  The specific location doesn't
matter, but it's important to keep things tidy.  Then modify your
static java classpath.  For example, if you are running R2012b on
linux, create (or edit) the file ~/.matlab/R2012b/javaclasspath.txt
and add the following line:
  /home/jane/ismrmrd/matlab/+ismrmrd/xmlhdr.jar 
Note that you must restart Matlab for the changes to take
effect. Please refer to the Matlab documentation for more information
on the java class path.

Compiling the JAVA bindings to the XML Header
--------------------------------------------- 
Assuming the ismrmrd library was installed in ${ISMRMRD_HOME}, the JAVA interface to the XML header can be created from the XML schema using the JAXB bindings in the following way: 
  cd ${ISMRMRD_HOME}/matlab/+ismrmrd
  xjc -p org.ismrm.ismrmrd.xmlhdr ../schema/ismrmrd.xsd
  javac org/ismrm/ismrmrd/xmlhdr/*.java
  jar -cvf xmlhdr.jar org/ismrm/ismrmrd/xmlhdr/*.class
  javadoc -d xmlhdr.javadoc org/ismrm/ismrmrd/xmlhdr/*.java
  rm -rf org


Modify your Matlab java classpath as described in previous
section. You will need to make sure that you are using the same
version of the java compiler as the jre that matlab is using.  A
description of how Matlab works with the JAVA runtime (JRE) is beyond
the scope of this document.  Please refer to the matlab documentation
for details. Two good starting points are:
  http://www.mathworks.com/help/matlab/ref/version.html
  http://www.mathworks.com/support/solutions/en/data/1-1812J/

