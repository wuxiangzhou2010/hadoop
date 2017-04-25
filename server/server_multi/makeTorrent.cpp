#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/file.hpp"
#include "libtorrent/storage.hpp"
#include "libtorrent/hasher.hpp"
#include "libtorrent/create_torrent.hpp"
#include "libtorrent/file.hpp"
#include "libtorrent/file_pool.hpp"
#include "makeTorrent.h"
#include "UpServer.h"
#include <pthread.h>
#include <boost/bind.hpp>
#include <string.h>
#include <vector>
#include <fstream>

#include "ModifyXML.h"
#include "UpTorrentToTrack.h"
using namespace libtorrent;
std::vector<char> load_file(std::string const& filename)
{
	std::vector<char> ret;
	std::fstream in;
	in.exceptions(std::ifstream::failbit);
	in.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
	in.seekg(0, std::ios_base::end);
	size_t const size = in.tellg();
	in.seekg(0, std::ios_base::beg);
	ret.resize(size);
	in.read(ret.data(), ret.size());
	return ret;
}

extern char* dec_file;
extern char* hdfs_path;
extern char Record[100000];

char *Path = new char[100];
int M = 0;

struct TorrentData
{
	char FilePath[200],TorrentPath[200];
};

void MakeTorrent::print_progress(int i, int num)
{
	fprintf(stderr, "\r%d/%d", i + 1, num);
}
//筛选器：文件大小满足自动作种
bool MakeTorrent::file_filter1(std::string const& f)
{
	bool flag = false;

	if(strstr(Path,f.c_str() ) != NULL)
		flag = true;

	return flag;
}
//筛选器：容器等待一起做种
bool MakeTorrent::file_filter2(std::string const& f)
{
	char name[100];
	strcpy(name,f.c_str() );
	strcat(name,"/");

	if(strstr(Record,name) != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int MakeTorrent::make(char* path,char* outFile,char *readPath,char* track)
{
	using namespace libtorrent;
	strcpy(Path,path);
	strcat(Path,"/\0");
	char const* creator_str = "";//写入文件目录
#ifndef BOOST_NO_EXCEPTIONS
	try
	{
#endif


	std::vector<std::string> web_seeds;			//种子（已有全部文件的机器）
	std::vector<std::string> trackers;			//服务器，负责管理seed目录等
	int pad_file_limit = -1;
	int piece_size = 0;
	int flags = 0;
	std::string root_cert;

	std::string outfile;		//输出的种子名
	std::string merklefile;			//做种子的文件名字
	outfile = outFile;
	trackers.push_back(track);
	//outfile = "/home/stu/文档/1.torrent";
#ifdef TORRENT_WINDOWS
	// don't ever write binary data to the console on windows
	// it will just be interpreted as text and corrupted
	outfile = "a.torrent";
#endif
	file_storage fs;		//目标文件
	file_pool fp;
	std::string full_path = libtorrent::complete(path);			//完全路径

	if(!strcmp(parent_path(path).c_str(),parent_path(dec_file).c_str() ) )
	{
		add_files(fs,full_path, file_filter1, flags);		//应该是遍历目录 把所有文件加到fs中
	}
	else
	{
		add_files(fs,dec_file, file_filter1, flags);		//应该是遍历目录 把所有文件加到fs中
	}

	if (fs.num_files() == 0)
	{
		fputs("no files specified.\n", stderr);
		return 1;
	}

	create_torrent t(fs, piece_size, pad_file_limit, flags);		//建立种子t

	for (std::vector<std::string>::iterator i = trackers.begin(), end(trackers.end() ); i != end; ++i)
	{
		t.add_tracker(*i);			//向t中加入tracker服务器信息
	}

	for (std::vector<std::string>::iterator i = web_seeds.begin(), end(web_seeds.end() ); i != end; ++i)
	{
		t.add_url_seed(*i);			//向t中加如资源种子信息
	}

	error_code ec;

	if(!strcmp(parent_path(path).c_str(),parent_path(dec_file).c_str() ) )
	{
		set_piece_hashes(t,parent_path(full_path), boost::bind(&MakeTorrent::print_progress, _1, t.num_pieces() ), ec);			//向种子加入哈系值
	}
	else
	{
		set_piece_hashes(t,parent_path(dec_file), boost::bind(&MakeTorrent::print_progress, _1, t.num_pieces() ), ec);			//向种子加入哈系值
	}

	if (ec)
	{
		fprintf(stderr, "%s\n", ec.message().c_str() );
		return 1;
	}

	fprintf(stderr, "\n");
	t.set_creator(creator_str);

	if (!root_cert.empty() )
	{
		std::vector<char> pem;
		load_file(root_cert);

		if (ec)
		{
			fprintf(stderr, "failed to load root certificate for tracker: %s\n", ec.message().c_str() );
		}
		else
		{
			t.set_root_cert(std::string(&pem[0], pem.size() ) );
		}
	}

	// create the torrent and print it to stdout
	std::vector<char> torrent;
	bencode(back_inserter(torrent), t.generate() );
	FILE* output = stdout;			//FILE*文件指针

	if (!outfile.empty() )
		output = fopen(outfile.c_str(), "wb+");

	fwrite(&torrent[0], 1, torrent.size(), output);

	if (output != stdout)
		fclose(output);

	if (!merklefile.empty() )
	{
		output = fopen(merklefile.c_str(), "wb+");
		int ret = fwrite(&t.merkle_tree()[0], 20, t.merkle_tree().size(), output);

		if (ret != t.merkle_tree().size() * 20)
		{
			fprintf(stderr, "failed to write %s: (%d) %s\n", merklefile.c_str(), errno, strerror(errno) );
		}

		fclose(output);
	}

	UpTorrentToTrack UpTorrent;
	char Torrent_URL[100];
	char* Torrent_url = UpTorrent.Send(outFile);
	strcpy(Torrent_URL,Torrent_url);
	strcat(Torrent_URL,"\0");
	ModifyXML ModifyXML;
	ModifyXML.ModXml(Torrent_URL);
	delete[] Torrent_url;
	pthread_t thread;
	struct TorrentData *Data = (TorrentData*)malloc(sizeof(TorrentData) );
	strcpy(Data->FilePath,parent_path(dec_file).c_str() );
	strcpy(Data->TorrentPath,outfile.c_str() );
	pthread_create(&thread,NULL,&UpServer::StartUp,(void*)(Data) );
#ifndef BOOST_NO_EXCEPTIONS
}
catch (std::exception& e)
{
	fprintf(stderr, "%s\n", e.what() );
}
#endif

	return 0;
}

int MakeTorrent::make(char* readPath,char* track)
{
	using namespace libtorrent;

	char const* creator_str = "";//写入文件目录
#ifndef BOOST_NO_EXCEPTIONS
	try
	{
#endif


	std::vector<std::string> web_seeds;			//种子（已有全部文件的机器）
	std::vector<std::string> trackers;			//服务器，负责管理seed目录等
	int pad_file_limit = -1;
	int piece_size = 0;
	int flags = 0;
	std::string root_cert;
	//name++;
	std::string outfile;		//输出的种子名
	std::string merklefile;			//做种子的文件名字
	char *outFile = new char[100];
	strcpy(outFile,readPath);
	strcat(outFile,".torrent");
	outfile = outFile;

	trackers.push_back(track);
	//outfile = "/home/stu/文档/1.torrent";
#ifdef TORRENT_WINDOWS
	// don't ever write binary data to the console on windows
	// it will just be interpreted as text and corrupted
	outfile = "a.torrent";
#endif
	file_storage fs;		//目标文件
	file_pool fp;
	std::string full_path = libtorrent::complete(parent_path(readPath) );		//完全路径
	add_files(fs, dec_file, file_filter2, flags);		//应该是遍历目录 把所有文件加到fs中

	if (fs.num_files() == 0)
	{
		fputs("no files specified.\n", stderr);
		return 1;
	}

	create_torrent t(fs, piece_size, pad_file_limit, flags);		//建立种子t

	for (std::vector<std::string>::iterator i = trackers.begin(), end(trackers.end() ); i != end; ++i)
	{
		t.add_tracker(*i);			//向t中加入tracker服务器信息
	}

	for (std::vector<std::string>::iterator i = web_seeds.begin(), end(web_seeds.end() ); i != end; ++i)
	{
		t.add_url_seed(*i);			//向t中加如资源种子信息
	}

	error_code ec;
	set_piece_hashes(t,parent_path(dec_file), boost::bind(&MakeTorrent::print_progress, _1, t.num_pieces() ), ec);			//向种子加入哈系值

	if (ec)
	{
		fprintf(stderr, "%s\n", ec.message().c_str() );
		return 1;
	}

	fprintf(stderr, "\n");
	t.set_creator(creator_str);

	if (!root_cert.empty() )
	{
		std::vector<char> pem;
		load_file(root_cert);

		if (ec)
		{
			fprintf(stderr, "failed to load root certificate for tracker: %s\n", ec.message().c_str() );
		}
		else
		{
			t.set_root_cert(std::string(&pem[0], pem.size() ) );
		}
	}

	// create the torrent and print it to stdout
	std::vector<char> torrent;
	bencode(back_inserter(torrent), t.generate() );
	FILE* output = stdout;			//FILE*文件指针

	if (!outfile.empty() )
		output = fopen(outfile.c_str(), "wb+");

	fwrite(&torrent[0], 1, torrent.size(), output);

	if (output != stdout)
		fclose(output);

	if (!merklefile.empty() )
	{
		output = fopen(merklefile.c_str(), "wb+");
		int ret = fwrite(&t.merkle_tree()[0], 20, t.merkle_tree().size(), output);

		if (ret != t.merkle_tree().size() * 20)
		{
			fprintf(stderr, "failed to write %s: (%d) %s\n", merklefile.c_str(), errno, strerror(errno) );
		}

		fclose(output);
	}

	UpTorrentToTrack UpTorrent;
	char Torrent_URL[100];
	char* Torrent_url = UpTorrent.Send(outFile);
	strcpy(Torrent_URL,Torrent_url);
	strcat(Torrent_URL,"\0");
	ModifyXML ModifyXML;
	ModifyXML.ModXml(Torrent_URL);
	delete[] Torrent_url;
	delete[] outFile;
	pthread_t thread;
	struct TorrentData *Data = (TorrentData*)malloc(sizeof(TorrentData) );
	strcpy(Data->FilePath,parent_path(dec_file).c_str() );
	strcpy(Data->TorrentPath,outfile.c_str() );
	pthread_create(&thread,NULL,&UpServer::StartUp,(void*)(Data) );
#ifndef BOOST_NO_EXCEPTIONS
}
catch (std::exception& e)
{
	fprintf(stderr, "%s\n", e.what() );
}
#endif

	return 0;
}
