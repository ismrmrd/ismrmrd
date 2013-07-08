## The loader and the wrapper classes
javac org/ismrm/ismrmrd/*.java

## The XML Header classes
# Make a class out of the schema
xjc -p org.ismrm.ismrmrd.xmlhdr -b ../../../bindings/java/cfg.jxb ../../../schema/ismrmrd.xsd
javac org/ismrm/ismrmrd/xmlhdr/*.java

# Build a big jar
jar -cvf ismrmrd.jar org/ismrm/ismrmrd/*.class org/ismrm/ismrmrd/xmlhdr/*.class

# Build the java docs
javadoc -quiet -d ismrmrd.javadoc -subpackages org.ismrm.ismrmrd
