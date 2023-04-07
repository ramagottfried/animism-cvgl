cmake_minimum_required(VERSION 3.19)
project(cvgl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(WITH_BLACKMAGIC True )

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(WITH_CUDA False )
else()
    set(WITH_CUDA True )
endif()

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

if( DEFINED WITH_BLACKMAGIC )
    # setup BlackMagic
    
    set(DECKLINK_SDK "${PROJECT_SOURCE_DIR}/../Blackmagic DeckLink SDK 12.1")

    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(DECKLINK_PLATFORM_INCLUDE "${DECKLINK_SDK}/Examples/Linux" )
        set(DECKLINK_SDK_INCLUDE_DIR "${DECKLINK_SDK}/Linux/include" )
        set(BLACKMAGIC_PLATFORM_SOURCES
                    ${DECKLINK_SDK_INCLUDE_DIR}/DeckLinkAPIDispatch.cpp
                    ${DECKLINK_PLATFORM_INCLUDE}/platform.cpp
        )
    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(DECKLINK_PLATFORM_INCLUDE "${DECKLINK_SDK}/Examples/Mac" )
        set(DECKLINK_SDK_INCLUDE_DIR "${DECKLINK_SDK}/Mac/include" )
        set(BLACKMAGIC_PLATFORM_SOURCES
                    ${DECKLINK_SDK_INCLUDE_DIR}/DeckLinkAPIDispatch.cpp
                    ${DECKLINK_PLATFORM_INCLUDE}/platform.cpp
        )
    endif()
    
endif()

list(APPEND LIB_LIST dl pthread) #${CMAKE_DL_LIBS}

# setup OpenGL / GLFW / GLAD / OPENCV
set(GLAD_DIR "${PROJECT_SOURCE_DIR}/../glad")

list(APPEND LIB_LIST ${CMAKE_DL_LIBS} )

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)

list(APPEND LIB_LIST glfw OpenGL::GL ${OpenCV_LIBS} )

message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    find_library(FoundationLib CoreFoundation)
    list(APPEND LIB_LIST ${FoundationLib} )
    message(STATUS "    include path: ${FoundationLib}")
endif()

find_package(Eigen3 REQUIRED)
list(APPEND LIB_LIST Eigen3::Eigen)

message(STATUS "    eigen include path: ${Eigen3_INCLUDE_DIRS}")

list(APPEND SRCLIST
    src/mixer/cvglMixer.cpp
    src/camera/cvglCameraBase.cpp
    src/camera/cvglCVCamera.cpp
    src/camera/cvglDeckLinkCamera.cpp
    src/camera/cvglVideoPlayer.cpp
    src/gl/cvglCallbacks.cpp
    src/gl/cvglContext.cpp
    src/gl/cvglObject.cpp
    src/gl/cvglTexture.cpp
    src/gl/cvglShader.cpp
    src/gl/cvglFramebuffer.cpp
    src/gl/cvglShaderFramebuffer.cpp
    src/gl/cvglVAO.cpp
    src/gl/cvglVertex.cpp
    src/gl/delabella.cpp
    src/cv/cvglCV.cpp
    src/cv/cvglConvexHull.cpp
    src/cv/cvglAnalysisData.cpp
    src/cv/cvglGaussian.cpp
    src/osc/MapOSC.cpp
    src/udp/cvglUDPServer.cpp
    src/utils/cvglThreadPool.cpp
    src/utils/cvglConversions.cpp
    src/utils/cvglHelperFunctions.cpp
    src/utils/cvglRandom.cpp
    src/utils/cvglLookupFigure.cpp
    src/cues/cvglCues.cpp
    ${GLAD_DIR}/src/glad.c
)

if( DEFINED WITH_BLACKMAGIC )
    list(APPEND SRCLIST
        src/camera/cvglDeckLinkCamera.cpp
        ${BLACKMAGIC_PLATFORM_SOURCES} 
    )
endif()

add_library(libcvgl STATIC
    ${SRCLIST}
)

if( DEFINED WITH_BLACKMAGIC )
    target_include_directories(libcvgl PUBLIC ${DECKLINK_SDK_INCLUDE_DIR} PUBLIC ${DECKLINK_PLATFORM_INCLUDE})
endif()

target_include_directories(libcvgl PUBLIC ${GLAD_DIR}/include )
target_include_directories(libcvgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_link_libraries(libcvgl ${LIB_LIST}  )

list(APPEND LIB_LIST libcvgl)


# ANIMISM build

set(CUE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cvgl-animism/cues)

file(GLOB CUE_FILES ${CUE_DIR}/*.cpp)

add_executable(cvgl
    ${CUE_FILES}
    cvgl-animism/cvgl_main.cpp # << should not put this in the main src dir
    cvgl-animism/cvglMainProcess.cpp
)

target_include_directories(cvgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
target_include_directories(cvgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
target_include_directories(cvgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cvgl-animism)

target_include_directories(cvgl PUBLIC ${CUE_DIR})

target_link_libraries(cvgl libcvgl)

add_custom_command(TARGET cvgl PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/src/shaders/ $<TARGET_FILE_DIR:cvgl>/shaders/)

message("CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE is ${CMAKE_C_FLAGS_RELEASE}")
message("Shders found ${SHADER_FILES}")
message("Cues found ${CUE_FILES}")


# basic-ex setup

add_executable(basic_app
    cvgl-basic/basicMainProcess.cpp
    cvgl-basic/cvgl-basic_main.cpp
)

target_include_directories(basic_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
target_include_directories(basic_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
target_include_directories(basic_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cvgl-basic)

target_link_libraries(basic_app libcvgl)

add_custom_command(TARGET basic_app PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/src/shaders/ $<TARGET_FILE_DIR:basic_app>/shaders/)


# zhdk build

set(CUE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cvgl-zhdk/cues)

file(GLOB CUE_FILES ${CUE_DIR}/*.cpp)

add_executable(zhdk
   ${CUE_FILES}
   cvgl-zhdk/cvgl_main.cpp # << should not put this in the main src dir
   cvgl-zhdk/cvglMainProcess.cpp
)

target_include_directories(zhdk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
target_include_directories(zhdk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
target_include_directories(zhdk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cvgl-zhdk)

target_include_directories(zhdk PUBLIC ${CUE_DIR})

target_link_libraries(zhdk libcvgl)

add_custom_command(TARGET zhdk PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                      ${CMAKE_SOURCE_DIR}/src/shaders/ $<TARGET_FILE_DIR:cvgl>/shaders/)


# this is a funny idea, maybe useful someday, but for now too much work
#target_compile_definitions(cvgl PRIVATE "CUE_FILE_NAMES=\"yo\"")
#target_compile_definitions(main PRIVATE "MY_FILE_PATH=\"${MY_FILE_PATH}\"")

# also potentially to log file path for loading shaders / future lua files?
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
