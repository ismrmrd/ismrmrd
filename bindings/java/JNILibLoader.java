package org.ismrm.ismrmrd;

import java.io.File;


public class JNILibLoader {
    public static void load() throws java.net.URISyntaxException {
        File myjar = new File(JNILibLoader.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath());
        File windowsLibDir = new File(myjar.getParent(), "jismrmrd.dll");
        File macLibDir = new File(myjar.getParent(), "libjismrmrd.jnilib");
        File unixLibDir = new File(myjar.getParent(), "libjismrmrd.so");

        // TODO: check OS name then load the correct library
        if (unixLibDir.exists()) {
            System.load(unixLibDir.getPath());
        } else if (windowsLibDir.exists()) {
            System.load(windowsLibDir.getPath());
        } else if (macLibDir.exists()) {
            System.load(macLibDir.getPath());
        } else {
            System.err.println("Failed to load ISMRMRD JNI library");
        }

        //ClassLoader loader = JNILibLoader.class.getClassLoader();
        //System.out.println(loader.getResource("org/ismrm/ismrmrd/JNILibLoader.class").toURI().getPath());
	//System.load(JNIFile);
	//System.load("/usr/local/ismrmrd/lib/libjismrmrd.jnilib");
	//System.loadLibrary("jismrmrd");
    }
}




