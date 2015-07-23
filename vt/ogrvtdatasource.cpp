/******************************************************************************
 * $Id: ogrmemdatasource.cpp 26302 2013-08-11 16:24:14Z rouault $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRVTDataSource class.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ****************************************************************************/

#include "ogr_vt.h"
#include "cpl_conv.h"
#include "cpl_string.h"
#include "ogr_spatialref.h"

#include "rapidjson/document.h"

CPL_CVSID("$Id: ogrvtdatasource.cpp 26302 2013-08-11 16:24:14Z rouault $");

/************************************************************************/
/*                          OGRVTDataSource()                          */
/************************************************************************/

OGRVTDataSource::OGRVTDataSource( const char *pszFilename, 
                                    char **papszOptions)

{
    pszName = CPLStrdup(pszFilename);
    papoLayers = NULL;
    nLayers = 0;
}

/************************************************************************/
/*                         ~OGRVTDataSource()                          */
/************************************************************************/

OGRVTDataSource::~OGRVTDataSource()

{
    CPLFree( pszName );

    for( int i = 0; i < nLayers; i++ )
        delete papoLayers[i];
    
    CPLFree( papoLayers );
}

/************************************************************************/
/*                             Open()                                  */
/************************************************************************/

/**
 * fei added
 * ====================================
 * Open datasource to read metadata, and construct layers in datasource obj.
 *
 */
int OGRVTDataSource::Open( const char * pszNewName, int bUpdate,
                              int bTestOpen )

{
    CPLAssert( nLayers == 0 );

    /**
     * eample conninfo:
     * "VT: kv_store=file dir=/home/gdal/vt" default kv_store is file
     * "VT: kv_store=Redis host=192.168.33.10 ip=5432 user=redis passwd=jj"
     */
    

/* -------------------------------------------------------------------- */
/*      Verify vector tile prefix.                                      */
/* -------------------------------------------------------------------- */
    if( !EQUALN(pszNewName,"VT:",3) )
    {
        if( !bTestOpen )
            CPLError( CE_Failure, CPLE_AppDefined,
                      "%s does not conform to Vector Tile naming convention,"
                      " VT:*\n", pszNewName );
        return FALSE;
    }

    pszName = CPLStrdup( pszNewName );
    char* pszConnectionName = CPLStrdup(pszName);

/* -------------------------------------------------------------------- */
/*      Parse out kv_store                                              */
/* -------------------------------------------------------------------- */
    char             *pszKvStoreStart;
    CPLString         osKvStore;
    pszKvStoreStart = strstr(pszConnectionName, "kv_store=");
    if (pszKvStoreStart == NULL)
        pszKvStoreStart = strstr(pszConnectionName, "KV_STORE=");
    if (pszKvStoreStart != NULL)
    {
        char           *pszKvStore;
        const char     *pszEnd = NULL;

        pszKvStore = CPLStrdup( pszKvStoreStart + strlen("kv_store=") );

        pszEnd = strchr(pszKvStore, ' ');
        if( pszEnd == NULL )
            pszEnd = pszConnectionName + strlen(pszConnectionName);

        // Remove KV_STORE=xxxxx from pszConnectionName string
        memmove( pszKvStoreStart, pszEnd, strlen(pszEnd) + 1 );

        pszKvStore[pszEnd - pszKvStoreStart - strlen("kv_store=")] = '\0';

        osKvStore = pszKvStore;
        CPLFree(pszKvStore);
    }
    else
    {
        osKvStore = "file";
    }
        CPLString otherConnInfo = ;

/* -------------------------------------------------------------------- */
/*      Construct KVStore instance via given conninfo                   */
/* -------------------------------------------------------------------- */
    KVStore *poGenKV = KVStoreFactory::GetKVStoreByName(osKvStore);
    if(poGenKV)
    {
        poKV = poGenKV;
    }
    else
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "%s construct kvstore failed"
                  " fei-VT:*\n", pszNewName );
    }

/* -------------------------------------------------------------------- */
/*      Try to establish connection.                                    */
/* -------------------------------------------------------------------- */
    poKV->open(otherConnInfo);

/* -------------------------------------------------------------------- */
/*      Try to get layers metadata in kv store                          */
/* -------------------------------------------------------------------- */
    char* pszBuffer = poKV->getValue("metadata");
    rapidjson::Document datasource;
    datasource.Parse(pszBuffer);

    const rapidjson::Value& ary = datasource["layers"];

    pszName = datasource["name"].GetString(); /* fei: datasource name */
    nLayers = ary.Size(); /* fei: get layer amount */
    
    papoLayers = (OGRVTLayer*)CPLMalloc(sizeof(OGRVTLayer*) * nLayers);
    if(!papoLayers)
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "malloc layer error"
                  " VT:*\n" );
    }

    /* rapidjson::Document layer; */

    OGRGeoJSONReader* poReader = new OGRGeoJSONReader();
    CPLAssert(poReader);

    for(int i, i<nLayers, ++i)
    {
        /* fei: 获取对应图层的元数据 */
        unsigned char* pszLayerDescBuf = poKV->getValue(ary[i].GetString());
        poReader->Parse(( const char*)pszLayerDescBuf);
        poReader->GenerateLayerDefn(papoLayers[i]); /* TODO */

        /* layer.Parse(pszLayerDescBuf); */
        /* int srid = layer["srid"]; */
        /* char* pszGeomType = layer["geometry_type"].GetString(); */

        /* /1* fei: need to parse more parameters to pass to Layer constructor *1/ */
        /* OGRSpatialReference *poSRS = new OGRSpatialReference(); */
        /* poSRS->ImportFromEPSG(srid); */
        /* papoLayers[i] = new OGRVTLayer(layer["name"].GetString(), */
        /*         poSRS, pszGeomType ); */

        /* /1* fei: add Attributtes define *1/ */
        /* if(papoLayers[i]) */
        /* { */
        /*     /1* fei: parse out more field define from json *1/ */
        /*     for() */
        /*     { */
        /*         /1* TODO *1/ */
        /*         papoLayers[i]->CreateField(); */
        /*     } */
        /* } */

        /* delete  poSRS; */
    }

    return bRet;
}

/************************************************************************/
/*                            CreateLayer()                             */
/************************************************************************/

OGRLayer *
OGRVTDataSource::CreateLayer( const char * pszLayerName,
                                 OGRSpatialReference *poSRS,
                                 OGRwkbGeometryType eType,
                                 char ** papszOptions )

{
/* -------------------------------------------------------------------- */
/*      Create the layer object.                                        */
/* -------------------------------------------------------------------- */
    OGRVTLayer *poLayer;

    poLayer = new OGRVTLayer( pszLayerName, poSRS, eType );

/* -------------------------------------------------------------------- */
/*      Add layer to data source layer list.                            */
/* -------------------------------------------------------------------- */
    papoLayers = (OGRVTLayer **)
        CPLRealloc( papoLayers,  sizeof(OGRVTLayer *) * (nLayers+1) );
    
    papoLayers[nLayers++] = poLayer;

    return poLayer;
}

/************************************************************************/
/*                            DeleteLayer()                             */
/************************************************************************/

OGRErr OGRVTDataSource::DeleteLayer( int iLayer )

{
    if( iLayer >= 0 && iLayer < nLayers )
    {
        delete papoLayers[iLayer];

        for( int i = iLayer+1; i < nLayers; i++ )
            papoLayers[i-1] = papoLayers[i];
        
        nLayers--;
        
        return OGRERR_NONE;
    }
    else
        return OGRERR_FAILURE;
}

/************************************************************************/
/*                           TestCapability()                           */
/************************************************************************/

int OGRVTDataSource::TestCapability( const char * pszCap )

{
    if( EQUAL(pszCap,ODsCCreateLayer) )
        return TRUE;
    else if( EQUAL(pszCap,ODsCDeleteLayer) )
        return TRUE;
    else if( EQUAL(pszCap,ODsCCreateGeomFieldAfterCreateLayer) )
        return TRUE;
    else
        return FALSE;
}

/************************************************************************/
/*                              GetLayer()                              */
/************************************************************************/

OGRLayer *OGRVTDataSource::GetLayer( int iLayer )

{
    if( iLayer < 0 || iLayer >= nLayers )
        return NULL;
    else
        return papoLayers[iLayer];
}

