/******************************************************************************
 * $Id: ogr_vt.h 27044 2015-07-01 23:41:27Z rouault $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Private definitions within the OGR Vector Tile driver.
 * Author:   Fei Lunzhou, feilunzhou@126.com
 *
 ******************************************************************************
 * Copyright (c) 2003, Frank Warmerdam <warmerdam@pobox.com>
 * Copyright (c) 2011-2013, Even Rouault <even dot rouault at mines-paris dot org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef _OGRMEM_H_INCLUDED
#define _OGRMEM_H_INCLUDED

#include "ogrsf_frmts.h"


/************************************************************************/
/*                              VectorTile                              */
/************************************************************************/

class TileID
{
public:
            TileID(char* pszLayerName, int x, int y, int z=0);
            TileID(CPLString cplString);
            ~TileID();

    int     x_;
    int     y_;
    int     z_;
    char*   pszLayerName_;

    char*   toString();
    int     setFromString(CPLString);
}


class VectorTile 
{
    TileID*             poTileID_;
    OGRVTLayer*         poLayer_;

    OGRFeature**        poFeatures_; 
    int                 *poFeatureCompatibleFlags_;

    int                 nFeature_; /* allocated feature count */
    int                 nCompatibleFeature_; /* compatible feature count */
    int                 nIterator_;

    /* if this tile is original tile, if deleteIncompatibleFeature, set 0;*/
    int                 bOriginal_; 

public:
                        VectorTile(OGRVTLayer* poLayer);
                        VectorTile(const VectorTile& ref_copy_constructor);
    virtual             ~VectorTile();
    const VectorTile&   operator= (const VectorTile& assign_constructor);
    /* VectorTile*         clone(); */

    const TileID*       getTileID() const;
    const OGRVTLayer*   getLayer() const;
    int                 isOriginalTile() const;
    virtual int         getFeatureCount() const;

    virtual OGRFeature*         getFirstFeature();
    virtual OGRFeature*         getNextFeature();
    virtual void                resetReading();

    virtual int         fetchTile(CPLString tilekey);
    virtual int         clearTile(); /* fei: purge all tile data */
    virtual int         deleteIncompatibleFeatures(); /* fei: delete all filter out features */

    virtual int         filteFID();
    virtual int         filteGeometry();
    virtual int         filteAttributes();

private:
    virtual int         deSerialize(char*) = 0; /* deserialize buff into features */
}


class GeoJSONVectorTile : public VectorTile
{
public:
    virtual int         deSerialize(char*);
}


/************************************************************************/
/*                              KVStore                                */
/************************************************************************/

class KVStore
{
public:
    virtual const char* getName() = 0 const;
    virtual int         open(CPLString connInfo) = 0;
    virtual char*       getValue(CPLString strKey) = 0 ;
}


class FileKV : public KVStore
{
    char*               pszRootPath;
public:
                        FileKV();
                        FileKV(CPLString rootPaht);
                        FileKV(const FileKV& copy_constructor);
                        ~FileKV();

    const FileKV&       operator= (const FileKV& assignment);
    const char*         getName();
    virtual int         open(CPLString openinfo);
    virtual unsigned char*       getValue(CPLString strKey);

}


class RedisKV : public KVStore
{
    /* HiredisConn        *poRedisConn; */
public:
                        RedisKV();
                        RedisKV(CPLString connInfo);
                        RedisKV();

    const char*         getName();
    virtual int         open(CPLString connInfo);
    virtual unsigned char*       getValue(CPLString strKey);
}


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
}


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


  private: /* by fei */

    VectorTile**        poTiles;
    int                 nTiles;

    CPLHashSet          *poHash; /* fei: hashSet to findout duplicated fid */
    OGRVTDataSource     *poDS; /* fei */

    int                 bFilterModified; /* fei */
    OGRGeometry         *poBBox; /* fei */

  public: /* by fei */

    /* fei: fetch all feature into layer according to filter */
    void                PerformFilter();  /* fei */
    CPLHashSet          getHash(); /* fei */


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

  private:
    int                 GetIntersectTiles(OGRGeometry *poGeom, char** poKeys);
};

/************************************************************************/
/*                           OGRVTDataSource                           */
/************************************************************************/

class OGRVTDataSource : public OGRDataSource
{
    OGRVTLayer          **papoLayers;
    int                 nLayers;
    
    char                *pszName;


  public:
    KVStore             *poKV; /* fei: refer to mysql/sqlite/pg driver */

                        OGRVTDataSource( const char *, char ** );
                        ~OGRVTDataSource();

    int                 Open( const char *, int bUpdate, int bTestOpen ); /* fei */

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
