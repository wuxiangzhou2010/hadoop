project(sample)
cmake_minimum_required(VERSION 2.6)
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Add extra include and library search directories so examples can optionally
# be built without a prior "make install" of libtorrent.
# list(INSERT CMAKE_INCLUDE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/../include")
# list(INSERT CMAKE_LIBRARY_PATH 0 "${CMAKE_CURRENT_BINARY_DIR}/..")

# Also use the generated pkg-config file prior to "make install".
# In an independent project, these lines would simply not exist.
# set(PKG_CONFIG_CHANGED_PATH "${CMAKE_CURRENT_BINARY_DIR}/..;$ENV{PKG_CONFIG_PATH}")
# if(UNIX)
    # string(REPLACE ";" ":" PKG_CONFIG_CHANGED_PATH "${PKG_CONFIG_CHANGED_PATH}")
# endif ()
# set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_CHANGED_PATH}")

set(XML_ROOT /home/test/old_vm/libxml2-2.9.4)
set(XML_HEADER ${XML_ROOT}/include)
set(TORRENT_ROOT  /home/test/old_vm/libtorrent-rasterbar-1.1.3)
set(TORRENT_HEADER ${TORRENT_ROOT}/include)
set(HADOOP_ROOT /home/test/old_vm/hh)
set(HADOOP_HEADER ${HADOOP_ROOT}/include)
set(HADOOP_LIB ${HADOOP_ROOT}/lib/native)
set(BOOST_ROOT /home/test/old_vm/boost_1_64_0)
set(BOOST_LIB ${BOOST_ROOT}/stage/lib)
set(JAVA_HOME /home/test/old_vm/jdk1.8.0_131)
set(JVM_HOME ${JAVA_HOME}/jre/lib/amd64/server)
# find_package(LibtorrentRasterbar REQUIRED)
# find_package(Boost REQUIRED COMPONENTS system)
link_directories(${JVM_HOME} ${HADOOP_LIB})
include_directories(${JAVA_HOME} ${HADOOP_HEADER} ${TORRENT_HEADER} ${BOOST_ROOT} ${XML_HEADER})
# add_definitions(${LibtorrentRasterbar_DEFINITIONS})

set(src_list
    arithmetic.cpp
    BigFile.cpp
    BigFile.h
    CMakeLists.txt
    Download.cpp
    Download.h
    DownloadPthread.cpp
    DownloadPthread.h
    makeTorrent.cpp
    makeTorrent.h
    ModifyXML.cpp
    ModifyXML.h
    QueryXML.cpp
    QueryXML.h
    ThreadPool.cpp
    ThreadPool.h
    UpServer.cpp
    UpServer.h
    UpTorrentToTrack.cpp
    UpTorrentToTrack.h)

set(link_flag 
    pthread
    xml2
    hdfs
    jvm)
# foreach(example ${single_file_examples})
# add_executable(${example} "${example}.cpp")
# target_link_libraries(${example} ${LibtorrentRasterbar_LIBRARIES} ${Boost_LIBRARIES})
# endforeach(example)

add_executable(sample ${src_list})

target_link_libraries(sample ${link_flag})
