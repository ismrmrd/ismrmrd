find_package(Java)

if (NOT JAVA_FOUND)
    message(STATUS "Java (xjc) Not Found - Not generating XML bindings")
else (NOT JAVA_FOUND)

    set(ISMRMRD_SCHEMA ${CMAKE_SOURCE_DIR}/schema/ismrmrd.xsd)
    set(ISMRMRD_JAR "${CMAKE_CURRENT_BINARY_DIR}/ismrmrd-matlab.jar")
    set(ISMRMRD_PACKAGE "org/ismrm/ismrmrd")
    set(ISMRMRD_PACKAGE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${ISMRMRD_PACKAGE}")

    set(XML_STRING "XMLString")
    set(XML_STRING_JAVA "XMLString.java")

    set(XJC_CONFIG "cfg.jxb")
    file (COPY ${XJC_CONFIG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    file (MAKE_DIRECTORY ${ISMRMRD_PACKAGE_PATH})
    file (COPY ${XML_STRING_JAVA} DESTINATION ${ISMRMRD_PACKAGE_PATH})

    foreach (class
        ${XML_STRING}
        AccelerationFactorType
        ExperimentalConditionsType
        MeasurementInformationType
        TrajectoryDescriptionType
        AcquisitionSystemInformationType
        FieldOfViewMm
        ObjectFactory
        TrajectoryType
        CalibrationModeType
        InterleavingDimensionType
        ParallelImagingType
        UserParameterBase64Type
        DicomParametersType
        IsmrmrdHeader
        ReferencedImageSequence
        UserParameterDoubleType
        Encoding
        LimitType
        SequenceParametersType
        UserParameterLongType
        EncodingLimitsType
        MRImageModule
        StudyInformationType
        UserParameters
        EncodingSpaceType
        MatrixSize
        SubjectInformationType
        package-info
    )
        list (APPEND ISMRMRD_XJC_SOURCES "${ISMRMRD_PACKAGE}/${class}.java")
        list (APPEND ISMRMRD_XJC_CLASSES "${ISMRMRD_PACKAGE}/${class}.class")
    endforeach()

    add_custom_command(
        OUTPUT ${ISMRMRD_JAR}
        COMMAND ${Java_XJC_EXECUTABLE} -quiet -b ${XJC_CONFIG} ${ISMRMRD_SCHEMA}
        COMMAND ${Java_JAVAC_EXECUTABLE} ${ISMRMRD_XJC_SOURCES}
        COMMAND ${Java_JAR_EXECUTABLE} cf ${ISMRMRD_JAR} ${ISMRMRD_XJC_CLASSES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating ISMRMRD XML bindings..." VERBATIM
        DEPENDS ${ISMRMRD_SCHEMA} ${XML_STRING_JAVA}
    )

    # make a fake target called 'xxx.jar' that will be built
    # when you type make and depends on the file 'xxx.jar'
    add_custom_target(ismrmrd-matlab-jar ALL DEPENDS ${ISMRMRD_JAR})
endif(NOT JAVA_FOUND)

INSTALL(DIRECTORY +ismrmrd DESTINATION matlab)
INSTALL(FILES ${ISMRMRD_JAR} DESTINATION matlab/+ismrmrd/+util)
