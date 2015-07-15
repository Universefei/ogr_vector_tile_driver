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

int GeoJSONVectorTile::deSerialize( unsigned char* pRawdata)
{
    int nFea = ParseOutFeatureCount(pRawData);
    nFeature_ = nFea;

    if(papoFeatures_) clearTile();
    papoFeatures_ = (OGRFeature**)malloc(sizeof(OGRFeature*) * nFea);
    if(!papoFeatures_)
    {
        nFeature_ = 0;
        return 0;
    }
    poFeatureCompatibleFlags_ = (int*)malloc(sizeof(int) * nFea);
    if(!poFeatureCompatibleFlags_ )
    {
        nFeature_ = 0;
        free(poFeatures_);
        return 0;
    }
    memset(poFeatures_, 0, nFeature_);
    memset(poFeatureCompatibleFlags_, 1, nFeature_);

    /* TODO: parse out every featuer */
    for(int i=0; i<nFeature_; ++i)
    {
        papoFeatures_[i] = new OGRFeature(poLayer_->GetLayerDefn());
        if(!poFeatures_[i]) 
        {
            --i; continue;
        }

        /* add geometry */
        unsigned char* pszGeomWkb =  pasrseOutGeom(pRawData);
        poFeatures_[i]->SetGeometryDirectly( 
                        OGRGeometryFactory::CreateFromWkb( pszGeomWkb ) );

        /* add fields */
        OGRFeatureDefn* poFDefn = poLayer_->GetLayerDefn();
        int nField = poFDefn->GetFieldCount();
        for(int i=0; i<nField; ++i)
        {
            OGRFieldType eFT = poFDefn->GetFieldDefn()->GetType();
            if( eFT == OFTInteger)
            {
            }
            else if(eFT == OFTReal)
            {
            }
            else if(eFT == OFTString)
            {
            }
            else 
            {
                SetField(i, );
            }
            
        }
    }

    return 0;
}

