cmake_minimum_required(VERSION 2.8.4)
project(clock)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -L/usr/local/lib -lglfw3 -lglew")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -L/usr/local/lib -lglfw3 -lglew")

#########################################################
# Include Files
#########################################################

set(GL_ROOT /System/Library/Frameworks/OpenGL.framework)
set(GL_INC ${GL_ROOT}/Headers)
set(GL_LIB ${GL_ROOT}/Libraries/libGL.dylib)

set(GLEW_VER 1.13.0)
set(GLEW_ROOT /usr/local/Cellar/glew/${GLEW_VER})
set(GLEW_INC ${GLEW_ROOT}/include/GL)
set(GLEW_LIB ${GLEW_ROOT}/lib/libGLEW.dylib)

set(GLUT_VER 2.8.1)
set(GLUT_ROOT /usr/local/Cellar/freeglut/${GLUT_VER})
set(GLUT_INC ${GLUT_ROOT}/include/GL)
set(GLUT_LIB ${GLUT_ROOT}/lib/libGLUT.dylib)

include_directories(${GL_INC} ${GLEW_INC} ${GLUT_INC} ${CMAKE_CURRENT_SOURCE_DIR}/src)

include(src/CMakeLists.txt)

add_executable (clock
        main.cpp
        ${SOURCE_FILES})

IF (APPLE)
    include_directories (/System/Library/Frameworks)
    find_library (GLUT_LIBRARY GLUT)
    find_library (OpenGL_LIBRARY OpenGL)
    find_library (GLM_LIBRARY GLM)
    mark_as_advanced (GLUT_LIBRARY
            OpenGL_LIBRARY
            GLM_LIBRARY)
    set (EXTRA_LIBS ${GLUT_LIBRARY} ${OpenGL_LIBRARY})

    find_package(GLM REQUIRED)
    include_directories(${GLM_INCLUDE_DIRS})
    link_directories(${GLM_LIBRARY_DIRS})
    add_definitions(${GLM_DEFINITIONS})

    if(NOT GLM_FOUND)
        message(Error "GLM not found")
    endif(NOT GLM_FOUND)
ENDIF (APPLE)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()


target_link_libraries(clock ${EXTRA_LIBS})
