OPTION(HDF5 "Build support for HDF5" OFF)

# See https://cmake.org/cmake/help/v3.0/module/FindHDF5.html

IF(HDF5)
    ADD_DEFINITIONS(-DUSEHDF5)
    FIND_PACKAGE(HDF5 REQUIRED COMPONENTS C CXX HL)

    INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})

    SET(COMPILE_FLAGS "${COMPILE_FLAGS} ${HDF5_COMPILE_FLAGS}")
    SET(LINK_EXTERNAL_LIBS ${LINK_EXTERNAL_LIBS} ${HDF5_LIBRARIES})

    MESSAGE(STATUS "Using HDF5 include: ${HDF5_INCLUDE_DIRS}")
    MESSAGE(STATUS "Using HDF5 libs: ${HDF5_LIBRARIES}")
ENDIF()