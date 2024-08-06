from conan import ConanFile


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("boost/1.80.0")
        self.requires("fftw/3.3.9")
        self.requires("hdf5/1.10.6")
        self.requires("pugixml/1.12.1")
