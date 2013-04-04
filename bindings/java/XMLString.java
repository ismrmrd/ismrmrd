package org.ismrm.ismrmrd.xmlhdr;

import javax.xml.bind.*;
import java.io.StringReader;


public class XMLString {
    public static IsmrmrdHeader StringToIsmrmrdHeader(String xmlstring) throws javax.xml.bind.JAXBException
    {
        ObjectFactory factory = new ObjectFactory();
        //JAXBContext jc = JAXBContext.newInstance("org.ismrm.ismrmrd.xmlhdr", factory.getClass().getClassLoader());
        JAXBContext jc = JAXBContext.newInstance(IsmrmrdHeader.class);
        Unmarshaller u  = jc.createUnmarshaller();
        StringReader reader = new StringReader(xmlstring);
        IsmrmrdHeader hdr = (IsmrmrdHeader) u.unmarshal(reader);
        return hdr;
    }

    /*
    public static String IsmrmrdHeaderToString(IsmrmrdHeader header)
    {

    }
    */
}
