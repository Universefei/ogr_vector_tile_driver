/******************************************************************************
 * $Id: ogrmemdriver.cpp 10645 2007-01-18 02:22:39Z warmerdam $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRVTDriver class.
 * Author:   Fei Lunzhou
 *
 ******************************************************************************/

#include "ogr_vt.h"
#include "cpl_conv.h"
#include "cpl_string.h"


/************************************************************************/
/*                           OGRVTDriver()                              */
/************************************************************************/

GeoJSONVectorTile::GeoJSONVectorTile(OGRVTLayer* poLayer):
    VectorTile(poLayer)
{
}

/************************************************************************/
/*                          ~OGRVTDriver()                              */
/************************************************************************/

GeoJSONVectorTile::~GeoJSONVectorTile()

{
}

/************************************************************************/
/*                           deSerialize()                              */
/************************************************************************/

int GeoJSONVectorTile::deSerialize( unsigned char* pRowdata)
{
    return 0;
}

