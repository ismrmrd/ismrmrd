public class runme {
    public static void main(String argv[]) {
        System.loadLibrary("jismrmrd");

        //System.out.format("ISMRMRD_USER_INTS: %d%n", ismrmrd.ISMRMRD_USER_INTS);
        IsmrmrdDataset f = new IsmrmrdDataset("testdata.h5", "/dataset");
        String xml = f.readHeader();
        System.out.println(xml);

        Acquisition a = f.readAcquisition(10);
        float[] data = a.getData();
        for (int i=0; i < a.getNumberOfSamples() * a.getActiveChannels(); i++) {
            System.out.format("%f ", data[i]);
        }
    }
}
