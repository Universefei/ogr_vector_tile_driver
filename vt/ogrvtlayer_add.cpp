/******************************************************************************
 * $Id: ogrmemlayer.cpp 27044 2014-03-16 23:41:27Z rouault $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRVTLayer class.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ****************************************************************************/

#include "ogr_vt.h"
#include "cpl_conv.h"
#include "ogr_p.h"
#include "ogr_geometry.h"

/************************************************************************/
/*                           GetIntersectTiles()                        */
/************************************************************************/

/**
 * Get from Wang Meixin
 *
 * @param (i/o/b)
 * @param (i/o/b)
 * @param (i/o/b)
 *
 * @return 
 */

vector<TileID*> OGRVTLayer::GetIntersectTiles(double minx, double miny, 
                                 double max, double maxy, int srid)
{
    TileIDSet ret;
    return ret;
}

/************************************************************************/
/*                           PerformFilter()                            */
/************************************************************************/

/**
 *  feilunzhou
 *
 * @param (i/o/b)
 *
 * @return 
 */

void OGRVTLayer::PerformFilter()

{
    /**
     * work flow
     * ====================================
     * Three steps:
     * 1]  find out tile numbers through geometry filter.
     * 2]  fetch tiles through tile number one by one.
     * 3]  secondary precious filting. (class Vector Tile or mannually handle)
     * 4]  add authorized feature into layer.
     * 5]  update filterModified flag.
     */

    if(bFilterModified)
    {
        TileIDSet   tileIds;
        TileIDSet::iterator itTiles;
        tileIds = GetIntersectTiles(GetSpatialFilter());

        VectorTile* poVT = new GeoJSONVectorTile(this);

        for(itTiles=tileIds.begin(); itTiles!=tileIds.end(); itTiles++)
        {
            /* fetch and do filtering */
            poVT->fetchTile( *itTiles );
            poVT->performFilting();
            poVT->commitToLayer();
            poVT->clearTile(); /* fei: release tile occupied resource */
        }

        delete poVT;
    }

    bFilterModified = 0;
}

/************************************************************************/
/*                              GetTile()                               */
/************************************************************************/

int OGRVTLayer::GetTile(int x, int y, int z)

{
    TileID tmpTile(GetName(), x, y, z);
    return GetTile(&tmpTile);
}

int OGRVTLayer::GetTile(TileID* poTileID)

{
    /* TODO: check if error occured, may connection failed */
    VectorTile* poVTile = new GeoJSONVectorTile(this);
    poVTile->fetchTile(poTileID);
    poVTile->performFilting();
    poVTile->commitToLayer();
    delete poVTile; /* TODO; may add to Vecter cache */

    return 0;
}

/************************************************************************/
/*                            ResetReading()                            */
/************************************************************************/

void OGRVTLayer::ResetReading()

{
    if(bFilterModified)
    {
        PerformFilter();
    }

    iNextReadFID = 0;
}

