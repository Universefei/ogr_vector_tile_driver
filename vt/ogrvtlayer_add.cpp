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
/*                           PerformFilter()                            */
/************************************************************************/

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

/************************************************************************/
/*                           GetIntersectTiles()                        */
/************************************************************************/

vector<TileID*> OGRVTLayer::GetIntersectTiles(double minx, double miny, 
                            double maxx, double maxy, int srid, int level)
{
    OGRLinearString* polinear = new OGRLinearString();
    polinear->addPoint(minx, miny);
    polinear->addPoint(minx, maxy);
    polinear->addPoint(maxx, maxy);
    polinear->addPoint(maxx, miny);
    polinear->addPoint(minx, miny);

    OGRGeometry* poExtent =  new OGRPolygon();
    poExtent->addRingDirectly(polinear);
    
    return GetIntersectTiles(poExtent, level);
}


/************************************************************************/
/*                            GetIntersectTiles()                       */
/************************************************************************/

std::vector<TileID*> OGRVTLayer::GetIntersectTiles(OGRGeometry* poGeom, int level)

{
    std::string layerName(getLayerName());
    return GetIntersectTiles_row(poGeom, level, layerName);
}


//////////////////////////////////////////////////////////////////////////
/// 该函数具体属于哪个类？是不是不属于HadoopKV
/// 该函数默认知道：所在图层，所在图层坐标系，所在图层名等
//////////////////////////////////////////////////////////////////////////
std::vector<TileID*> GetIntersectTiles_row( OGRGeometry* poGeom, int level, std::string name )
{
	int geom_srid =	poGeom->getSpatialReference()->GetEPSGGeogCS();
	std::vector<TileID*> ret;
	if( 3785 == geom_srid )
	{
		//////////////////////////////////////////////////////////////////////////
		/// 待实现
		//////////////////////////////////////////////////////////////////////////
	}
	else if( 4326 == geom_srid )
	{
		OGREnvelope *ogrEnvelope = new OGREnvelope;
		poGeom->getEnvelope( ogrEnvelope );

		LatLonExtent latlon( ogrEnvelope->MaxX, ogrEnvelope->MinX, ogrEnvelope->MinY, ogrEnvelope->MaxY );
		/// int level = 14;
		Tile leftDown, rightUp;
		leftDown.level = level;
		rightUp.level = level;
		TileCalculator::GetTilesExtentOfLatLonGeo( latlon, level, leftDown, rightUp );

		Tile traversalTile;
		LatLonExtent traversalLatLonExtent;
		OGREnvelope traversalOGREnvelope;
		traversalTile.level = level;


		//////////////////////////////////////////////////////////////////////////
		/// 遍历Feature可能覆盖的瓦片
		/// 先遍历列，再遍历行
		//////////////////////////////////////////////////////////////////////////
		for( int i = leftDown.x; i <= rightUp.x; ++i )
		{
			traversalTile.x = i;
			for( int j= leftDown.y; j <= rightUp.y; ++j )
			{
				traversalTile.y = j;
				TileCalculator::GetExtentOfTileGeo( traversalTile, traversalLatLonExtent );
				traversalOGREnvelope.MinX = traversalLatLonExtent.west;
				traversalOGREnvelope.MinY = traversalLatLonExtent.south;
				traversalOGREnvelope.MaxX = traversalLatLonExtent.east;
				traversalOGREnvelope.MaxY = traversalLatLonExtent.north;

				if( ogrEnvelope->Intersects( traversalOGREnvelope ) )
				{
					/// has intersect
					TileID * tileId = new TileID( 
						name.c_str(), 
						traversalTile.x,
						traversalTile.y,
						traversalTile.level );
					ret.push_back(tileId);
					delete tileId;
					tileId = NULL;
				}
				else
				{
					/// No intersect
				}
			}
		}
	}
	return ret;
}
