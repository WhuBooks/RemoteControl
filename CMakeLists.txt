cmake_minimum_required(VERSION 3.8)
project(VOLib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)

#MESSAGE(STATUS "Project Source Dir : ${PROJECT_SOURCE_DIR}")
#MESSAGE(STATUS "Project Binary Dir : ${PROJECT_BINARY_DIR}")
#MESSAGE(STATUS "Current Source Dir : ${CMAKE_CURRENT_SOURCE_DIR}")
#MESSAGE(STATUS "Current Binary Dir : ${CMAKE_CURRENT_BINARY_DIR}")

# set cmake module path to where find the third party lib
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
MESSAGE(STATUS "CMake Module Path : ${CMAKE_MODULE_PATH}")

# set postfix of different build-type
set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "postfix applied to debug build of libraries")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "postfic applied to release build of libraries")
set(CMAKE_RELWITHDEBINFO_POSTFIX "_rd" CACHE STRING "postfix applied to relwithdebinfo build of libraries")
set(CMAKE_MINSIZEREL_POSTFIX "_s" CACHE STRING "postfix applied to minsizerel build of libraries")

# set exe postfix
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(EXE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    set(EXE_POSTFIX ${CMAKE_DEBUG_POSTFIX})
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    set(EXE_POSTFIX ${CMAKE_RELWITHDEBINFO})
elseif(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set(EXE_POSTFIX ${CMAKE_MINSIZEREL_POSTFIX})
endif(CMAKE_BUILD_TYPE MATCHES Release)

# set binary and library directories
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
MESSAGE(STATUS "Executable Output Path : ${EXECUTABLE_OUTPUT_PATH}")
MESSAGE(STATUS "Library Output Path : ${LIBRARY_OUTPUT_PATH}")

# ouput compiler platform
if(WIN32)
    MESSAGE(STATUS "Compling for Windows")
elseif(APPLE)
    MESSAGE(STATUS "Compling for OSX")
elseif(ANDROID)
    MESSAGE(STATUS "Compling for Android")
elseif(UNIX)
    MESSAGE(STATUS "Compling for Unix")
else()
    MESSAGE(STATUS "Can't find compiler platform")
endif(WIN32)

# add third party lib to this variable
set(Third_Party_Libs)

# Eigen
include_directories("/usr/include/eigen3")

# OpenCV
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
list(APPEND Third_Party_Libs ${OpenCV_LIBS})

# Sophus
find_package(Sophus REQUIRED)
INCLUDE_DIRECTORIES(${Sophus_INCLUDE_DIR})
list(APPEND Third_Party_Libs ${Sophus_LIBRARY})

# CSparse
find_package(CSparse REQUIRED)
INCLUDE_DIRECTORIES(${CSPARSE_INCLUDE_DIR})
list(APPEND Third_Party_Libs ${CSPARSE_LIBRARY})

# g2o
find_package(G2O REQUIRED)
INCLUDE_DIRECTORIES(${G2O_INCLUDE_DIR})
set(G2O_LIBS g2o_core g2o_stuff g2o_cli
        g2o_solver_cholmod g2o_solver_csparse g2o_csparse_extension
        g2o_solver_dense g2o_solver_pcg g2o_solver_slam2d_linear
        g2o_solver_structure_only g2o_solver_eigen g2o_types_data
        g2o_types_icp g2o_types_sba g2o_types_sclam2d
        g2o_types_sim3 g2o_types_slam2d g2o_types_slam3d)
list(APPEND Third_Party_Libs ${G2O_LIBS})

# pangolin
find_package(Pangolin REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})
list(APPEND Third_Party_Libs ${Pangolin_LIBRARIES})

# glog
find_package(Glog REQUIRED)
include_directories(${Glog_INCLUDE_DIR})
list(APPEND Third_Party_Libs ${GLOG_LIBRARY})

# Ceres-solver
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})
list(APPEND Third_Party_Libs ${CERES_LIBRARIES})

MESSAGE(STATUS "Third Party Libs : ${Third_Party_Libs}")

set(Dataset_Dir "~/rgbd_dataset_freiburg2_large_with_loop")
add_definitions(-DDataset_Dir)
include_directories(TestLib)
include_directories(src)
add_subdirectory(src)
add_subdirectory(TestLib)
add_subdirectory(TestApp)

