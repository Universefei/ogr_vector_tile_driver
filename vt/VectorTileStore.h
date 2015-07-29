#ifndef __VectorTileStore_H_
#define __VectorTileStore_H_



#define	JCLASS_NAME "vectorDataStorge/VectorDataStorge"
#define DLL_API __declspec(dllexport) 
 
#include <string.h>  
#include <stdio.h>
#include <string>
#include <fstream>
#include <io.h>
class DLL_API VectorTileStore
{
public:
	/*
 * @Method:    connect
 * @FullName:  vectorTileStore::connect
 * @Access:    public 
 * @Returns:   bool falseʧ�� true�ɹ�
 * @purpose:	  
 */
	bool connect(int argc, char ** argv);
	bool connect();
/*
 * @Method:    disconnect
 * @FullName:  vectorTileStore::disconnect
 * @Access:    public 
 * @Returns:   bool falseʧ�� true�ɹ�
 * @purpose:	  
 */
	bool disConnect();
/*
 * @Method:    getTile
 * @FullName:  vectorTileStore::getTile
 * @Access:    public 
 * @Returns:   int > 0��Ƭ�ĳ��� = 0 û�и���Ƭ  <0 ���ִ���
 * @Parameter: const char * key
 * @Parameter: const char * type
 * @Parameter: const char * scale
 * @Parameter: const char * dataStandard
 * @Parameter: char * * tileValue
 * @purpose:	  
 */
	int getTile( const char* key, const char*type, const char* scale, const char* dataStandard, char** tileValue );
/*
 * @Method:    putTile
 * @FullName:  vectorTileStore::putTile
 * @Access:    public 
 * @Returns:   int -1ʧ�� 0�ɹ�
 * @Parameter: const char * key
 * @Parameter: const char * type
 * @Parameter: const char * scale
 * @Parameter: const char * dataStandard
 * @Parameter: const char * tileValue
 * @Parameter: int tileLength 
 * @purpose:	  
 */
	int putTile( const char* key, const char* type, const char* scale, const char* dataStandard, const char* tileValue, int tileLength );
/*
 * @Method:    deleteTile
 * @FullName:  vectorTileStore::deleteTile
 * @Access:    public -1ʧ�� 0�ɹ�
 * @Returns:   int
 * @Parameter: const char * key
 * @Parameter: const char * type
 * @Parameter: const char * scale
 * @Parameter: const char * dataStandard
 * @purpose:	  
 */
	int deleteTile( const char* key, const char*type, const char* scale, const char* dataStandard );
/*
 * @Method:    flush
 * @FullName:  vectorTileStore::flush
 * @Access:    public 
 * @Returns:   void
 * @purpose:	  
 */
	void flush(void);
/*
 * @Method:    freeTileMemory
 * @FullName:  vectorTileStore::freeTileMemory
 * @Access:    public 
 * @Returns:   void
 * @Parameter: char * tileValue
 * @purpose:	�ͷ� vectorTileStore::getTile ��������ڴ�ռ� 
 */
	void freeTileMemory( char* tileValue );
	VectorTileStore(void);
	~VectorTileStore(void);
};






#endif