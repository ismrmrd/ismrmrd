find_package(HDF5 1.8 COMPONENTS C)

if (HDF5_FOUND)
    add_subdirectory(c)
    add_subdirectory(protocol)
endif (HDF5_FOUND)
