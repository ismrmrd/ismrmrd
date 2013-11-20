package org.ismrm.ismrmrd;

import javax.xml.bind.*;
import javax.xml.namespace.QName;
import javax.xml.transform.stream.*;
import java.io.StringReader;
import java.io.StringWriter;


public class XMLString {
    public static IsmrmrdHeader StringToIsmrmrdHeader(String xmlstring) throws javax.xml.bind.JAXBException
    {
        JAXBContext jaxbContext = JAXBContext.newInstance(IsmrmrdHeader.class);

        Unmarshaller unmarshaller  = jaxbContext.createUnmarshaller();

        StringReader reader = new StringReader(xmlstring);

        JAXBElement<IsmrmrdHeader> root = unmarshaller.unmarshal(new StreamSource(reader), IsmrmrdHeader.class);

        return root.getValue();
    }

    public static String IsmrmrdHeaderToString(IsmrmrdHeader header) throws javax.xml.bind.JAXBException
    {
        JAXBContext jaxbContext = JAXBContext.newInstance(IsmrmrdHeader.class);

        Marshaller marshaller = jaxbContext.createMarshaller();

        marshaller.setProperty(Marshaller.JAXB_SCHEMA_LOCATION, "http://www.ismrm.org/ISMRMRD ismrmrd.xsd");
        marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, Boolean.TRUE);

        StringWriter writer = new StringWriter();

        QName qname = new QName("http://www.ismrm.org/ISMRMRD", "ismrmrdHeader");
        JAXBElement<IsmrmrdHeader> root = new JAXBElement(qname, IsmrmrdHeader.class, header);

        marshaller.marshal(root, writer);

        return writer.toString();
    }
}
