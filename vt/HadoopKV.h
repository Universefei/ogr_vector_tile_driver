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
	/// HadoopKVû��ʵ��
	//////////////////////////////////////////////////////////////////////////
    /* HadoopKV* poHadoop = new HadoopKV("host=192.168.1.2 port=78888 path=lll"); */
	/* HadoopKV( std::string connInfo ); */
	virtual ~HadoopKV();
	virtual const char* getName() const;

	//////////////////////////////////////////////////////////////////////////
	/// ��ĳ�����ݿ�
	/// û��close��
	//////////////////////////////////////////////////////////////////////////
	virtual int open( std::string connInfo );

	//////////////////////////////////////////////////////////////////////////
	/// Ϊʲô����ֵ��unsigned char*??
	/// �ڴ��ͷ�����
	//////////////////////////////////////////////////////////////////////////
	virtual unsigned char* getValue( std::string strKey ) const ;

	//////////////////////////////////////////////////////////////////////////
	/// ��ȡԪ�����費��Ҫʲô�ӿڣ�
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// ��ȡԪ���ݵĻ�һ��Ҫ�������ļ�������������
	/// �˶�����Σ�TCP/IP�����ļ���ַ���ܱ�ô��
	/// 
	/// �費��Ҫ���߳�֮��ģ�����
	/// ����Ҫ����Ϊ�Ƕ��ӿ�
	//////////////////////////////////////////////////////////////////////////

private:
	VectorTileStore *vectorTileStore;
	std::string server_ip;
	int port;
	static const int BUFFER_SIZE = 1024;
	static const int FILE_NAME_MAX_SIZE = 512;
	std::string metaDataName;

	//////////////////////////////////////////////////////////////////////////
	/// ��0��ʼ���
	/// Ԫ��������map
	//////////////////////////////////////////////////////////////////////////
	std::map< int, std::string > kvMap;
	std::map< std::string, std::string > valueTypeMap;

	PyramidMD pyramidMetaData;
};

#endif
