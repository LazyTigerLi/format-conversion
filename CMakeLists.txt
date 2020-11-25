cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_C_COMPILER /usr/bin/gcc-8)
# set(CMAKE_CXX_COMPILER /usr/bin/g++-8)
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O3")
#set(CMAKE_GENERATOR "MinGW Makefiles")
project(format_conversion)

set(PROJECT_PATH /home/user/cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_PATH}/lib)

set(LIB_SRC ${PROJECT_PATH}/src/read_hdf5.cpp 
            ${PROJECT_PATH}/src/sac2hdf5.cpp 
            ${PROJECT_PATH}/src/spec2hdf5.cpp
            ${PROJECT_PATH}/src/file_tree.cpp)

find_package(HDF5 COMPONENTS CXX HL)
include_directories(${HDF5_INCLUDE_DIRS}
                    ${PROJECT_PATH}/include)

link_libraries(/usr/lib/libstdc++fs.a
                ${HDF5_LIBRARIES}
                ${HDF5_CXX_HL_LIBRARIES})

add_library(format_conversion SHARED ${LIB_SRC})
# add_executable(test ${PROJECT_PATH}/src/test.cpp 
#                     ${PROJECT_PATH}/src/sac2hdf5.cpp)
# target_link_libraries(test
#                     ${PROJECT_PATH}/lib/libformat_conversion.so)
# add_executable(spec2hdf5 ./src/spec2hdf5.cpp)
# add_executable(read_hdf5 ./src/read_hdf5.cpp)
# add_executable(sac2hdf5 ./src/sac2hdf5.cpp)

# target_link_libraries(spec2hdf5
#                     /usr/lib/libstdc++fs.a
#                     ${HDF5_LIBRARIES}
#                     ${HDF5_CXX_HL_LIBRARIES})
# target_link_libraries(sac2hdf5
#                     /usr/lib/libstdc++fs.a
#                     ${HDF5_LIBRARIES}
#                     ${HDF5_CXX_HL_LIBRARIES})
# target_link_libraries(read_hdf5
#                     ${HDF5_LIBRARIES}
#                     ${HDF5_CXX_HL_LIBRARIES})
