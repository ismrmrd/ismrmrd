package org.ismrm.ismrmrd.xmlhdr;

import javax.xml.bind.*;
import java.io.StringReader;
import java.io.StringWriter;


public class XMLString {
    public static IsmrmrdHeader StringToIsmrmrdHeader(String xmlstring) throws javax.xml.bind.JAXBException
    {
        JAXBContext jc = JAXBContext.newInstance(IsmrmrdHeader.class);
        Unmarshaller u  = jc.createUnmarshaller();
        StringReader reader = new StringReader(xmlstring);
        IsmrmrdHeader hdr = (IsmrmrdHeader) u.unmarshal(reader);
        return hdr;
    }

    public static String IsmrmrdHeaderToString(IsmrmrdHeader header) throws javax.xml.bind.JAXBException
    {
        JAXBContext jc = JAXBContext.newInstance(IsmrmrdHeader.class);
        Marshaller m = jc.createMarshaller();
        m.setProperty(Marshaller.JAXB_SCHEMA_LOCATION, "http://www.ismrm.org/ISMRMRD ismrmrd.xsd");
        m.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, Boolean.TRUE);
        StringWriter writer = new StringWriter();
        m.marshal(header, writer);
        return writer.toString();
    }
}
