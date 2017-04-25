WORKROOT=`stu`
HADOOP_HOME="/home/zz/hadoop"
JAVA_HOME="/home/zz/jdk1.8.0_131"
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${HADOOP_HOME}/lib/native
CC=clang++
LIBS="-lm"

#DEFS = -DPACKAGE_NAME=\"libtorrent-rasterbar\" -DPACKAGE_TARNAME=\"libtorrent-rasterbar\" -DPACKAGE_VERSION=\"0.16.1\" -DPACKAGE_STRING=\"libtorrent-rasterbar\ 0.16.1\" -DPACKAGE_BUGREPORT=\"arvid@cs.umu.se\" -DPACKAGE_URL=\"http://www.libtorrent.org\" -DPACKAGE=\"libtorrent-rasterbar\" -DVERSION=\"0.16.1\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DHAVE_PTHREAD=1 -DHAVE_BOOST=1 -DHAVE_BOOST_SYSTEM=1 -DHAVE_GETHOSTBYNAME=1 -DHAVE_GETHOSTBYNAME_R=1 -DGETHOSTBYNAME_R_RETURNS_INT=1 -DHAVE_LIBRT=1 -DNDEBUG=1 -DTORRENT_USE_OPENSSL=1 -DHAVE_LINUX_FIEMAP_H=1 -DWITH_SHIPPED_GEOIP_H=1 -DBOOST_ASIO_HASH_MAP_BUCKETS=1021 -DBOOST_EXCEPTION_DISABLE=1 -DBOOST_ASIO_ENABLE_CANCELIO=1 -DBOOST_ASIO_DYN_LINK=1 -DTORRENT_BUILDING_SHARED=1
#export CLASSPATH=/home/stu/hadoop-0.20.2/hadoop-0.20.2-ant.jar:/home/stu/hadoop-0.20.2/hadoop-0.20.2-core.jar:/home/stu/hadoop-0.20.2/hadoop-0.20.2-examples.jar:/home/stu/hadoop-0.20.2/hadoop-0.20.2-test.jar:/home/stu/hadoop-0.20.2/hadoop-0.20.2-tools.jar:/home/stu/hadoop-0.20.2/lib/commons-cli-1.2.jar:/home/stu/hadoop-0.20.2/lib/commons-codec-1.3.jar:/home/stu/hadoop-0.20.2/lib/commons-el-1.0.jar:/home/stu/hadoop-0.20.2/lib/commons-httpclient-3.0.1.jar:/home/stu/hadoop-0.20.2/lib/commons-logging-1.0.4.jar:/home/stu/hadoop-0.20.2/lib/commons-logging-api-1.0.4.jar:/home/stu/hadoop-0.20.2/lib/commons-net-1.4.1.jar:/home/stu/hadoop-0.20.2/lib/core-3.1.1.jar:/home/stu/hadoop-0.20.2/lib/hsqldb-1.8.0.10.jar:/home/stu/hadoop-0.20.2/lib/jasper-compiler-5.5.12.jar:/home/stu/hadoop-0.20.2/lib/jasper-compiler-5.5.12.jar:/home/stu/hadoop-0.20.2/lib/jasper-runtime-5.5.12.jar:/home/stu/hadoop-0.20.2/lib/jets3t-0.6.1.jar:/home/stu/hadoop-0.20.2/lib/jetty-6.1.14.jar:/home/stu/hadoop-0.20.2/lib/jetty-util-6.1.14.jar:/home/stu/hadoop-0.20.2/lib/junit-3.8.1.jar:/home/stu/hadoop-0.20.2/lib/kfs-0.2.2.jar:/home/stu/hadoop-0.20.2/lib/log4j-1.2.15.jar:/home/stu/hadoop-0.20.2/lib/mockito-all-1.8.0.jar:/home/stu/hadoop-0.20.2/lib/oro-2.0.8.jar:/home/stu/hadoop-0.20.2/lib/servlet-api-2.5-6.1.14.jar:/home/stu/hadoop-0.20.2/lib/slf4j-api-1.4.3.jar:/home/stu/hadoop-0.20.2/lib/slf4j-log4j12-1.4.3.jar:/home/stu/hadoop-0.20.2/lib/xmlenc-0.52.jar
#export LD_LIBRARY_PATH
all: test_example
test_example:
	g++ arithmetic.cpp Download.cpp BigFile.cpp makeTorrent.cpp \
	UpServer.cpp ModifyXML.cpp UpTorrentToTrack.cpp ThreadPool.cpp \
	DownloadPthread.cpp QueryXML.cpp -I/home/zz/hadoop/include /home/zz/hadoop/lib/native/libhdfs.so \
	-I/home/zz/CVE-2016-4449/include \
	-ltorrent-rasterbar -lboost_system \
	-lpthread -lxml2 -lxml2 -o server
.PHONY:clean
clean:
	rm test_example
