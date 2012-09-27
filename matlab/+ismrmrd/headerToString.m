function xmlstring = headerToString(hdr)

    jpath = fileparts(mfilename('fullpath'));
    javaaddpath({fullfile(jpath,'ismrmrdxml.jar')});
    import javax.xml.bind.*

    jc = JAXBContext.newInstance('ismrmrdxml', hdr.getClass().getClassLoader());
    m  = jc.createMarshaller();
    m.setProperty(Marshaller.JAXB_SCHEMA_LOCATION, 'http://www.ismrm.org/ISMRMD ismrmrd.xsd');

    strbuff = java.io.StringWriter();
    m.marshal(hdr, strbuff);
    xmlstring = strbuff.toString();

end
