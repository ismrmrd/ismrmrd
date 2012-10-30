function hdr = stringToHeader(xmlstring)

    jpath = fileparts(mfilename('fullpath'));
    javaaddpath({fullfile(jpath,'ismrmrdxml.jar')});
    import javax.xml.bind.*
    of = ismrmrdxml.ObjectFactory();
    jc = JAXBContext.newInstance('ismrmrdxml', of.getClass().getClassLoader());
    u  = jc.createUnmarshaller();
    jstr = java.lang.String(xmlstring);
    hdr = u.unmarshal(java.io.ByteArrayInputStream(jstr.getBytes()));
    
end
