package org.ismrm.ismrmrd;

public class JNILibLoader {
    public static void load() throws java.net.URISyntaxException {
        ClassLoader loader = JNILibLoader.class.getClassLoader();
        System.out.println(loader.getResource("org/ismrm/ismrmrd/JNILibLoader.class").toURI().getPath());
	//System.load(JNIFile);
	//System.load("/usr/local/ismrmrd/lib/libjismrmrd.jnilib");
	//System.loadLibrary("jismrmrd");
    }
}




