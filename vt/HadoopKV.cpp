#include "HadoopKV.h"
#include <WinSock2.h>
#include <iostream>
#include "sqlite3.h"

#pragma comment( lib, "WS2_32" )




HadoopKV::HadoopKV()
{
	port = 8087;
	server_ip = "192.168.2.214";
	metaDataName = "E:/Trash/metadata.sqlite";

	//////////////////////////////////////////////////////////////////////////
	/// ͨ��TCP/IP���metadata.sqlite
	/// ��ʼ��Socket DLL
	//////////////////////////////////////////////////////////////////////////
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if(WSAStartup(socketVersion, &wsaData) != 0)
	{
		std::cout << "Init socket dll error!" << std::endl;
		exit(1);
	}

	/// ����socket
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == c_Socket)
	{
		std::cout << "Create Socket Error!" << std::endl;
		exit(1);
	}

	/// ָ������˵ĵ�ַ
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr( server_ip.c_str() );
	server_addr.sin_port = htons( port );

	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		std::cout << "Can Not Connect To Server IP!" << std::endl;
		exit(1);
	}

	//�����ļ���
	char file_name[FILE_NAME_MAX_SIZE+1];
	memset(file_name, 0, FILE_NAME_MAX_SIZE);
	memcpy( file_name, metaDataName.c_str(), metaDataName.length()+1 );

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE ? BUFFER_SIZE:strlen(file_name));

	/// ������������ļ���
	if(send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		std::cout << "Send File Name Failed" << std::endl;
		exit(1);
	}

	char buf[L_tmpnam];
	tmpnam_s( buf );
	metaDataName = buf;

	/// ���ļ���׼��д��
	FILE * fp = fopen(metaDataName.c_str(), "wb"); //windows����"wb",��ʾ��һ��ֻд�Ķ������ļ�
	if(NULL == fp)
	{
		std::cout << "File: " << metaDataName.c_str() << " Can Not Open To Write!" << std::endl;
		exit(1);
	}
	else
	{
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		while ((length = recv(c_Socket, buffer, BUFFER_SIZE, 0)) > 0)
		{
			if (fwrite(buffer, sizeof(char), length, fp) < length)
			{
				std::cout << "File: " << metaDataName.c_str() << " Write Failed" << std::endl;
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}
		std::cout << "Receive File: " << metaDataName.c_str() << " From Server Successful!" << std::endl;
	}

	fclose(fp);
	fp = NULL;
	closesocket(c_Socket);

	//////////////////////////////////////////////////////////////////////////
	///�ͷ�winsock��
	//////////////////////////////////////////////////////////////////////////
	WSACleanup();
}

HadoopKV::HadoopKV( std::string connInfo )
{
	//////////////////////////////////////////////////////////////////////////
	/// ɾ����ʱ���ݿ�
	//////////////////////////////////////////////////////////////////////////
	if( 0 != remove( metaDataName.c_str() ) )
	{
		/// can't delete
	}
	if( NULL != vectorTileStore )
	{
		vectorTileStore->disConnect();
		delete vectorTileStore;
		vectorTileStore = NULL;
	}
}

HadoopKV::~HadoopKV()
{
	//////////////////////////////////////////////////////////////////////////
	/// �Ͽ�����
	//////////////////////////////////////////////////////////////////////////
}

const char* HadoopKV::getName() const
{
	return NULL;
}

int HadoopKV::open( std::string connInfo )
{
	//////////////////////////////////////////////////////////////////////////
	/// �򿪱���metadata.sqlite
	/// ������ͼ������   ��ΪΨһ��ʶ
	/// ������
	//////////////////////////////////////////////////////////////////////////
	int ret = -1;
	
	/// std::string name = "buildings";

	sqlite3 *DB = NULL;
	sqlite3_stmt *stmt = NULL;
	char* SQL = NULL;
	ret = sqlite3_open( metaDataName.c_str(), &DB );
	if( SQLITE_OK != ret )
	{
		sqlite3_close( DB );
		DB = NULL;
		return 1;
	}
	ret = sqlite3_threadsafe();

	SQL = "SELECT * FROM metadata WHERE table_name = $table_name;";
	sqlite3_prepare( DB, SQL, -1, &stmt, NULL );
	sqlite3_bind_text( stmt, sqlite3_bind_parameter_index( stmt, "$table_name" ),
		connInfo.c_str(), connInfo.length(), SQLITE_STATIC );
	ret = sqlite3_step( stmt );
	if( SQLITE_ROW == ret )
	{
		pyramidMetaData.table_name = connInfo;
		pyramidMetaData.geo_column_name = (char*)sqlite3_column_text( stmt, 1 );
		pyramidMetaData.geom_type = (char*)sqlite3_column_text( stmt, 2 );
		pyramidMetaData.geom_srid = sqlite3_column_int( stmt, 3 );
		pyramidMetaData.level = sqlite3_column_int( stmt, 4 ) ;
	}

	if( NULL != stmt )
	{
		sqlite3_finalize( stmt );
		stmt = NULL;
	}

	SQL = "SELECT * FROM column_defn WHERE table_name=$table_name;";
	sqlite3_prepare( DB, SQL, -1, &stmt, NULL );
	sqlite3_bind_text( stmt, sqlite3_bind_parameter_index( stmt, "$table_name" ),
		connInfo.c_str(), connInfo.length(), SQLITE_STATIC );
	while( SQLITE_ROW == sqlite3_step( stmt ))
	{
		std::string column_name = (char*)sqlite3_column_text( stmt, 1 );
		std::string column_type = (char*)sqlite3_column_text( stmt, 2 );
		int cid = sqlite3_column_int( stmt, 3 );
		kvMap.insert( std::pair< int, std::string >( cid, column_name ) );
		valueTypeMap.insert( std::pair< std::string, std::string >( column_name, column_type ) );
	}

	if( NULL != stmt )
	{
		sqlite3_finalize( stmt );
		stmt = NULL;
	}
	if( NULL != DB )
	{
		sqlite3_close( DB );
		DB = NULL;
	}

	vectorTileStore = new VectorTileStore();

	char *path = "E:/C++projects/ImportDBToolAssistant/DBToolAssistant/conf/VectorTileStoreConf.xml";
	ret = vectorTileStore->connect(1,&path);
}

unsigned char* HadoopKV::getValue( std::string strKey ) const
{
	char *buf_get = NULL;
	char *ret = NULL;
	int length = vectorTileStore->getTile( strKey.c_str(), "may", "may", "may", &buf_get );
	if( length<0 )
	{
		/// ����
		return NULL;
	}
	else if( length == 0 )
	{
		/// û�и���Ƭ
		return NULL;
	}
	else
	{
		/// ����Ƭ����???
		/// ret = new char[length];
		/// memcpy( ret, buf_get, length );
		//////////////////////////////////////////////////////////////////////////
		/// �����Ⱑ
		//////////////////////////////////////////////////////////////////////////
		return (unsigned char*)buf_get;
	}

}
