
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtUtil)


SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                        OSG_LIBRARY 
                        OSGDB_LIBRARY
                        OSGUTIL_LIBRARY
                        OSGSIM_LIBRARY
                        XERCES_LIBRARY
                        OPENTHREADS_LIBRARY)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_UTIL_LIBRARY)
           
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)

IF (PCHSupport_FOUND)
   ADD_PRECOMPILED_HEADER(${LIB_NAME} precomp.cpp prefix/dtutilprefix-src.h true)
ENDIF (PCHSupport_FOUND)
