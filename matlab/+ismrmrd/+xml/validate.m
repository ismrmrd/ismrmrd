function validate( xml,schemaFile )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
import java.io.*;
import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.*;

factory = SchemaFactory.newInstance('http://www.w3.org/2001/XMLSchema');
schemaLocation = File(schemaFile);
schema = factory.newSchema(schemaLocation);
validator = schema.newValidator();
sr = StringReader(xml);
source = StreamSource(sr);
validator.validate(source);



end

