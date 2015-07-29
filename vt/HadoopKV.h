#ifndef __HADOOPKV_H_
#define __HADOOPKV_H_

#include <string>
#include <map>
#include "VectorTileStore.h"
#include "ogr_vt.h"

/* class TileID */
/* { */
/* public: */
/* 	TileID( const char* pszLayerName, int x, int y, int z=0 ) */
/* 		: x_(x), y_(y), z_(z) */
/* 	{ */
/* 		pszLayerName_ = new char[strlen(pszLayerName)+1]; */
/* 		memcpy( pszLayerName_, pszLayerName, strlen(pszLayerName)+1 ); */
/* 	} */
/* 	TileID( const TileID& cpy ){} */
/* 	TileID( std::string cplString ){} */
/* 	~TileID(){} */
/* 	TileID& operator=( const TileID& duplica ){} */

/* 	int x_; */
/* 	int y_; */
/* 	int z_; */
/* 	char* pszLayerName_; */

/* 	int setLayerName( const char* pszLayerName ){ return 0; } */
/* 	const char* toString() const{ return NULL; } */
/* 	int setFromString( std::string ){ return 0; } */
/* }; */

struct PyramidMD
{
	std::string table_name;
	std::string geo_column_name;
	std::string geom_type;
	int geom_srid;
	int level;
	PyramidMD(){}
};


class HadoopKV : public KVStore
{
public:
	HadoopKV();

	//////////////////////////////////////////////////////////////////////////
	/// HadoopKV没有实现
	//////////////////////////////////////////////////////////////////////////
    /* HadoopKV* poHadoop = new HadoopKV("host=192.168.1.2 port=78888 path=lll"); */
	/* HadoopKV( std::string connInfo ); */
	virtual ~HadoopKV();
	virtual const char* getName() const;

	//////////////////////////////////////////////////////////////////////////
	/// 打开某个数据库
	/// 没有close吗？
	//////////////////////////////////////////////////////////////////////////
	virtual int open( std::string connInfo );

	//////////////////////////////////////////////////////////////////////////
	/// 为什么返回值是unsigned char*??
	/// 内存释放问题
	//////////////////////////////////////////////////////////////////////////
	virtual unsigned char* getValue( std::string strKey ) const ;

	//////////////////////////////////////////////////////////////////////////
	/// 获取元数据需不需要什么接口？
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// 获取元数据的话一定要把整个文件都保存下来？
	/// 退而求其次，TCP/IP保存文件地址不能变么？
	/// 
	/// 需不需要多线程之类的？？？
	/// 不需要，因为是读接口
	//////////////////////////////////////////////////////////////////////////

private:
	VectorTileStore *vectorTileStore;
	std::string server_ip;
	int port;
	static const int BUFFER_SIZE = 1024;
	static const int FILE_NAME_MAX_SIZE = 512;
	std::string metaDataName;

	//////////////////////////////////////////////////////////////////////////
	/// 从0开始编号
	/// 元数据名字map
	//////////////////////////////////////////////////////////////////////////
	std::map< int, std::string > kvMap;
	std::map< std::string, std::string > valueTypeMap;

	PyramidMD pyramidMetaData;
};

#endif
