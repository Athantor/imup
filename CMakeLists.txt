cmake_minimum_required(VERSION 2.8)
project(imup)

include(FindPkgConfig)

FIND_PACKAGE(Qt4 REQUIRED)
pkg_check_modules(EXIV2 REQUIRED exiv2)

SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "-pipe -std=c++0x -Wall -Wextra -march=native ${EXIV2_CFLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3 -Werror -fstack-protector-all -fstrict-aliasing -fstrict-overflow -fipa-pure-const -ftree-vrp -Warray-bounds")

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
include_directories(${EXIV2_INCLUDE_DIRS})

SET(imup_SOURCES 
	main.cpp imupwin.cpp 
    imagefile.cpp 
    commonsimgobject.cpp 
    commonsimgwidget.cpp
    uploadproject.cpp
    imageloader.cpp
 )

SET(imup_HEADERS  imupwin.h 
    imagefile.h 
    commonsimgobject.h 
    commonsimgwidget.h
    uploadproject.h
    imageloader.h
)

SET(imup_UIS  imupwin.ui 
    commonsimgwidget.ui
)

QT4_WRAP_UI(imup_UIS_H ${imup_UIS})
QT4_WRAP_CPP(imup_HEADERS_MOC ${imup_HEADERS})


ADD_EXECUTABLE(imup ${imup_SOURCES} 
	${imup_HEADERS_MOC} ${imup_UIS_H})
TARGET_LINK_LIBRARIES(imup ${QT_LIBRARIES} ${EXIV2_LIBRARIES})

