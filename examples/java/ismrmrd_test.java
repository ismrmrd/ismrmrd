import org.ismrm.ismrmrd.*;
import org.ismrm.ismrmrd.xmlhdr.*;

public class ismrmrd_test {

    public static void main(String argv[]) {
        // Load the ISMRMRD JNI library
        try {
            JNILibLoader.load();
        }
        catch (java.net.URISyntaxException e) {
            System.err.println("JNI Library failed to load: " + e.getMessage());
            System.exit(1);
        }

        String filename = "";
        if (argv.length == 1) {
            filename = argv[0];
        }
        else {
            System.err.println("Give me a filename.");
            System.exit(1);
        }

        IsmrmrdDataset f = new IsmrmrdDataset(filename, "/dataset");
        String xmlstring = f.readHeader();

        System.out.println(xmlstring);

        IsmrmrdHeader hdr = new IsmrmrdHeader();
        try {
            hdr = XMLString.StringToIsmrmrdHeader(xmlstring);
        } catch (javax.xml.bind.JAXBException e) {
            System.err.format("XML Header could not be parsed.%n%s", e.getMessage());
            System.exit(1);
        }

        long freq = hdr.getExperimentalConditions().getH1ResonanceFrequencyHz();
        System.out.format("%n%n The resonance frequency is %d.%n", freq);

        long nacq = f.getNumberOfAcquisitions();
        System.out.format("%n%n The number of Acquisitions is %d.%n", nacq);

        String xmlcopy = "";
        try {
            xmlcopy = XMLString.IsmrmrdHeaderToString(hdr);
        } catch (javax.xml.bind.JAXBException e) {
            System.err.format("XML Header could not be generated.%n%s", e.getMessage());
            System.exit(1);
        }

        System.out.println(xmlcopy);
    }

}
