
SET(LIB_NAME     dtNetGM)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

INCLUDE_DIRECTORIES( ${GNE_INCLUDE_DIR}  
                                    ${HAWKNL_INCLUDE_DIR}
                                    ${Boost_INCLUDE_DIR})

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                        GNE_LIBRARY
                        HAWKNL_LIBRARY
                        CURSES_LIBRARY)
                        
TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTGAME_LIBRARY}
                     )
                                   
IF (WIN32)
TARGET_LINK_LIBRARIES(${LIB_NAME} wsock32)
ENDIF (WIN32)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_NETGM_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
