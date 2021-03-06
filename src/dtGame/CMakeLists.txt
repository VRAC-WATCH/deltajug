
SET(LIB_NAME     dtGame)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h" "${HEADER_PATH}/*.inl")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    OSGSIM_LIBRARY)

TARGET_LINK_LIBRARIES(${LIB_NAME}  
                      ${CARBON_LIBRARY}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_GAME_LIBRARY)

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)

IF (PCHSupport_FOUND)
   ADD_PRECOMPILED_HEADER(${LIB_NAME} precomp.cpp  true)
ENDIF (PCHSupport_FOUND)

