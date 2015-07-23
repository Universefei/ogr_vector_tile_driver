/******************************************************************************
 * $Id: ogr_vt.h 27044 2015-07-01 23:41:27Z rouault $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Private definitions within the OGR Vector Tile driver.
 * Author:   Fei Lunzhou, feilunzhou@126.com
 *
 ******************************************************************************/

#ifndef _OGRMEM_H_INCLUDED
#define _OGRMEM_H_INCLUDED

#include "ogrsf_frmts.h"
#include "hashtable.h"

#include <vector>
using namespace std;

class OGRVTLayer;
class OGRVTDataSource;

/* unsigned long myHashfn(const void*); */
/* int myEqualFn(const void* left, const void* right); */
/* void myFreeElementFn(void* ptr); */


/************************************************************************/
/*                              VectorTile                              */
/************************************************************************/
class TileID
{

public:
                TileID(const char* pszLayerName, int x, int y, int z=0);
                TileID(const TileID& cpy); /* TODO */
                TileID(CPLString cplString);
                ~TileID();
    TileID&     operator= (const TileID& duplica); /* TODO */

    int         x_;
    int         y_;
    int         z_;
    char*       pszLayerName_;

    int         setLayerName(const char* pszLayerName);
    const char* toString() const;
    int         setFromString(CPLString);
};


class VectorTile 
{
protected:

    TileID*             poTileID_;
    mutable OGRVTLayer*   poLayer_;
    /* OGRVTLayer*         poLayer_; */

    /* OGRFeature**        papoFeatures_; */ 
    /* int                 *poFeatureCompatibleFlags_; */

    vector<OGRFeature*> papoFeatures_;
    vector<int>         poFeatureCompatibleFlags_;

    /* int                 nFeature_; /1* allocated feature count *1/ */
    int                 nCompatibleFeature_; /* compatible feature count */
    int                 nIterator_;

    /* if this tile is original tile, if deleteIncompatibleFeature, set 0;*/
    int                 bOriginal_; 
    int                 bFIDFilted_;

public:
                        VectorTile(OGRVTLayer* poLayer, TileID* poTileID = NULL);
                        VectorTile(const VectorTile& ref_copy_constructor);
    virtual             ~VectorTile();
    VectorTile&         operator= (const VectorTile& assign_constructor);
    /* VectorTile*         clone(); */

    const TileID*       getTileID() const;
    OGRVTLayer*         getLayer() const;
    int                 isOriginalTile() const;
    virtual int         getFeatureCount() const;


    virtual OGRFeature* getFirstFeature();
    virtual OGRFeature* getNextFeature();
    virtual void        resetReading();

    virtual int         fetchTile(TileID* poTileID); /* XXX:call deSerialize */
    virtual int         fetchTile(const char* pszLayerName, 
                                    int x, int y, int z=0); /* XXX:call deSerialize */

    virtual int         clearTile(); /* XXX: purge all tile data */

    virtual int         commitToLayer() const; /* XXX:commit all compatible feature into poLayer */
    virtual int         deSerialize(unsigned char*) = 0; /* XXX: deserialize buff into features */
    int                 performFilting(int bFilteGeom=0, int bFilteAttr=0); /* XXX: template method pattern */

public:
    bool                AddFeature(OGRFeature* poFeature); /* need by geojson reader */

protected:

    virtual int         filteFID(); /* XXX */
    virtual int         filteGeometry();
    virtual int         filteAttributes();

    virtual int         deleteIncompatibleFeatures(); /* fei: delete all filter out features */
};


class GeoJSONVectorTile : public VectorTile
{
public:
                        GeoJSONVectorTile(OGRVTLayer* poLayer);
                        ~GeoJSONVectorTile();

    virtual int         deSerialize(unsigned char*);
};


/************************************************************************/
/*                              KVStore                                */
/************************************************************************/

class KVStore
{
public:
    virtual const char* getName() const = 0;
    virtual int         open(CPLString connInfo) = 0;
    virtual unsigned char* getValue(CPLString strKey) const = 0 ;
};


class FileKV : public KVStore
{
    char*               pszRootPath;
public:
                        FileKV();
                        FileKV(CPLString rootPaht);
                        FileKV(const FileKV& cpy);
                        ~FileKV();

    const FileKV&       operator= (const FileKV& assignment);
    const char*         getName();
    virtual int         open(CPLString openinfo);
    virtual unsigned char*       getValue(CPLString strKey);

};


class RedisKV : public KVStore
{
    /* HiredisConn        *poRedisConn; */
public:
                        RedisKV();
                        RedisKV(CPLString connInfo);

    const char*         getName();
    virtual int         open(CPLString connInfo);
    virtual unsigned char*       getValue(CPLString strKey);
};


class KVStoreFactory
{
    KVStore             **papoKVStores_;
    int                 nKVStores_;

public:
    KVStore*            getAndOpenKVStoreByName(CPLString kvName, CPLString openInfo);
    static KVStoreFactory*     GetInstance();

private:
                        KVStoreFactory();
    KVStore*            createKVStore(CPLString name);

};


/**
 * reference OGRSFDriverRegistrar
 */

static void *hDRMutex = NULL;
static KVStoreFactory* volatile poGKVFactorty = NULL; /* fei: why volatile */


/************************************************************************/
/*                             OGRVTLayer                              */
/************************************************************************/

class OGRVTLayer : public OGRLayer
{
    OGRFeatureDefn     *poFeatureDefn;
    
    int                 nFeatureCount;
    int                 nMaxFeatureCount;
    OGRFeature        **papoFeatures;

    int                 iNextReadFID;
    int                 iNextCreateFID;

    int                 bUpdatable;
    int                 bAdvertizeUTF8;

    int                 bHasHoles;


/* --------------------------------------------------------------------- */
/*                          Added properties                             */
/* --------------------------------------------------------------------- */

  private: /* by fei */

    /* XXX: VT caches should put into datasource or layer? */
    VectorTile**        poTiles; /*  */
    int                 nTiles;

    HashTable           *poHash; /* fei: hashSet to findout duplicated fid */
    OGRVTDataSource     *poDS; /* fei */

    int                 bFilterModified; /* fei */
    OGRGeometry         *poBBox; /* fei */

  public: /* by fei */

    /* fei: fetch all feature into layer according to filter */
    void                PerformFilter();  /* fei */
    HashTable*          GetHash() const; /* fei */

    typedef vector<TileID*> TileIDSet;
    vector<TileID*>     GetIntersectTiles(double minx, double miny, 
                                        double maxx, double maxy, int srid);
    vector<TileID*>     GetIntersectTiles(OGRGeometry* poGeom);

    /* XXX: get all data into papoFeatures */
    int                 GetTile(int x, int y, int z=0); 
    int                 GetTile(TileID*);

    const KVStore*      GetKVStore() const ; /* needed by class VectorTile */

/*---------------------------------------------------------------------------*/

  public:
                        OGRVTLayer( const char * pszName,
                                     OGRSpatialReference *poSRS,
                                     OGRwkbGeometryType eGeomType );
                        ~OGRVTLayer();


    void                ResetReading();
    OGRFeature *        GetNextFeature();
    virtual OGRErr      SetNextByIndex( long nIndex );

    OGRFeature         *GetFeature( long nFeatureId );
    OGRErr              SetFeature( OGRFeature *poFeature );
    OGRErr              CreateFeature( OGRFeature *poFeature );
    virtual OGRErr      DeleteFeature( long nFID );
    
    OGRFeatureDefn *    GetLayerDefn() { return poFeatureDefn; }

    int                 GetFeatureCount( int );

    virtual OGRErr      CreateField( OGRFieldDefn *poField,
                                     int bApproxOK = TRUE );
    virtual OGRErr      DeleteField( int iField );
    virtual OGRErr      ReorderFields( int* panMap );
    virtual OGRErr      AlterFieldDefn( int iField, OGRFieldDefn* poNewFieldDefn, int nFlags );
    virtual OGRErr      CreateGeomField( OGRGeomFieldDefn *poGeomField,
                                         int bApproxOK = TRUE );

    int                 TestCapability( const char * );

    void                SetUpdatable(int bUpdatableIn) { bUpdatable = bUpdatableIn; }
    void                SetAdvertizeUTF8(int bAdvertizeUTF8In) { bAdvertizeUTF8 = bAdvertizeUTF8In; }

    int                 GetNextReadFID() { return iNextReadFID; }

};

/************************************************************************/
/*                           OGRVTDataSource                           */
/************************************************************************/

class OGRVTDataSource : public OGRDataSource
{
    OGRVTLayer          **papoLayers;
    int                 nLayers;
    
    char                *pszName;
    KVStore             *poKV; /* fei: refer to mysql/sqlite/pg driver */


  public:

                        OGRVTDataSource( const char *, char ** );
                        ~OGRVTDataSource();

    int                 Open( const char *, int bUpdate, int bTestOpen ); /* fei */
    const KVStore*      GetKVStore(); /* fei */

    const char          *GetName() { return pszName; }
    int                 GetLayerCount() { return nLayers; }
    OGRLayer            *GetLayer( int );

    virtual OGRLayer    *CreateLayer( const char *, 
                                      OGRSpatialReference * = NULL,
                                      OGRwkbGeometryType = wkbUnknown,
                                      char ** = NULL );
    OGRErr              DeleteLayer( int iLayer );

    int                 TestCapability( const char * );
};

/************************************************************************/
/*                             OGRVTDriver                             */
/************************************************************************/

class OGRVTDriver : public OGRSFDriver
{
  public:
                ~OGRVTDriver();
                
    const char *GetName();
    OGRDataSource *Open( const char *, int );

    virtual OGRDataSource *CreateDataSource( const char *pszName,
                                             char ** = NULL );
    
    int                 TestCapability( const char * );
};


#endif /* ndef _OGRMEM_H_INCLUDED */
