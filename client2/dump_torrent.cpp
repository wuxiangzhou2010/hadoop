#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/lazy_entry.hpp"
#include "libtorrent/magnet_uri.hpp"
#include "dump_torrent.h"
#include "Download.h"
#include "UpServer.h"

extern char* IP;
extern int Port;
extern int PER_LEN;
extern char* hdfs_path;

struct TorrentData
{
	char FilePath[200],TorrentPath[200];
};

int DumpTorrent::dump(char* TorrentPath)
{
	using namespace libtorrent;
	
	int item_limit = 1000000;
	int depth_limit = 1000;


	int size = file_size(TorrentPath);
	if (size > 40 * 1000000)
	{
		fprintf(stderr, "file too big (%d), aborting\n", size);
		return 1;
	}
	std::vector<char> buf(size);
	error_code ec;
	int ret = load_file(TorrentPath, buf, ec, 40 * 1000000);
	if (ret != 0)
	{
		fprintf(stderr, "failed to load file: %s\n", ec.message().c_str());
		return 1;
	}
	lazy_entry e;
	int pos;
	ret = lazy_bdecode(&buf[0], &buf[0] + buf.size(), e, ec, &pos
		, depth_limit, item_limit);


	torrent_info t(e, ec);
	// print info about torrent
	
	typedef std::vector<std::pair<std::string, int> > node_vec;
	node_vec const& nodes = t.nodes();

	char ih[41];
	to_hex((char const*)&t.info_hash()[0], 20, ih);
	printf("种子文件个数为: %d\n种子包含文件为\n", t.num_files());
	int index = 0;
	if(t.num_files()==1){
		bool flag=false;
		char *name=new char[100];
		for (torrent_info::file_iterator i = t.begin_files();i != t.end_files(); ++i, ++index){
			printf("%s\n",t.files().file_path(*i).c_str());
			strcpy(name,t.files().file_path(*i).c_str());
			printf("%s\n",name);
			int len=strlen(name);printf("%d\n",len);
			if(name[len-2]=='+'){
				if(name[len-1]=='1'){
					name[len-2]='\0';
					printf("123414234234234\n");
					flag=true;
				}
				else break;
			}
			printf("213423423423423@@%s\n",name);
			Download Down(IP,Port,PER_LEN);
			char *directory=new char[100];char *HDFSPath=new char[100];
			strcpy(directory,"./");
			strcat(directory,name);
			strcpy(HDFSPath,hdfs_path);
			strcat(HDFSPath,name);
			Down.down(directory,HDFSPath,flag);
			delete[] directory,HDFSPath,name;
		}
	}
	else{
		for (torrent_info::file_iterator i = t.begin_files();i != t.end_files(); ++i, ++index){
			Download Down(IP,Port,PER_LEN);
			char *directory=new char[100];char *HDFSPath=new char[100];
			strcpy(directory,"./");
			strcat(directory,t.files().file_path(*i).c_str());
			strcpy(HDFSPath,hdfs_path);
			strcat(HDFSPath,t.files().file_path(*i).c_str());
			Down.down(directory,HDFSPath,false);
			delete[] directory,HDFSPath;
		}
	}
	pthread_t thread;
	struct TorrentData *Data=(TorrentData*)malloc(sizeof(TorrentData));
	strcpy(Data->FilePath,"./");
	strcpy(Data->TorrentPath,TorrentPath);
	pthread_create(&thread,NULL,&UpServer::StartUp,(void*)(Data));
	

	return 0;
}

