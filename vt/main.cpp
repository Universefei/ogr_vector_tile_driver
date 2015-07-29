#include <iostream>
#include <cstdlib>
#include <vector>
#include "HadoopKV.h"
#include "ogrsf_frmts.h"
#include "TileCalculator.h"

std::vector<TileID*> GetIntersectTiles( OGRGeometry* poGeom, int level, std::string name );


int main()
{
	HadoopKV *hadoopKV = new HadoopKV();
	hadoopKV->open( "buildings" );
	


	std::cout << " Key: buildings#14#26301#11313, Value: " << hadoopKV->getValue( "buildings#14#26301#11313" ) << std::endl;
	std::cout << " Key: buildings#14#26307#11317, Value: " << hadoopKV->getValue( "buildings#14#26307#11317" ) << std::endl;


	if( NULL != hadoopKV )
	{
		delete hadoopKV;
		hadoopKV = NULL;
	}
	system( "pause" );
	return 0;
}


//////////////////////////////////////////////////////////////////////////
/// 该函数具体属于哪个类？是不是不属于HadoopKV
/// 该函数默认知道：所在图层，所在图层坐标系，所在图层名等
//////////////////////////////////////////////////////////////////////////
std::vector<TileID*> GetIntersectTiles( OGRGeometry* poGeom, int level, std::string name )
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