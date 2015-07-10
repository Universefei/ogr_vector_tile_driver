#include "ogr_vt.h"
#include "ogr_feature.h"


/* --------------------------------------------------------------------- */
/*                          VectorTile()                                 */
/* --------------------------------------------------------------------- */

VectorTile::VectorTile(OGRVTLayer* poLayer):
    poTileID_(NULL),
    poLayer_(poLayer),
    poFeatures_(NULL),
    poFeatureCompatibleFlags_(NULL),
    nFeature_(0),
    nCompatibleFeature_(0),
    nIterator_(0),
    bOriginal_(1)
{
}

/* --------------------------------------------------------------------- */
/*                          copy constructor                             */
/* --------------------------------------------------------------------- */

VectorTile::VectorTile(const OGRVTLayer& ref)
{
    *poTileID_ = *(ref.getTileID());
    poLayer_ = ref.getLayer();

    int nFeature = ref.getFeatureCount();
    poFeatures_ = (OGRFeature**) malloc(sizeof(OGRFeature*)nFeature);
    poFeatureCompatibleFlags_ = (int*) malloc(sizeof(int) * nFeature);

    poFeatures_[0] = ref.getFirstFeature()->Clone();
    for(int i = 1; i<nFeature; ++i)
    {
        poFeatures_[i] = ref.getNextFeature()->Clone();
    }
    
    nFeature_ = nCompatibleFeature_ = nFeature;
    nIterator = 0;
    bOriginal_ = ref.isOriginalTile();
}

/* --------------------------------------------------------------------- */
/*                          operator =                                   */
/* --------------------------------------------------------------------- */

const VectorTile& operator= (const OGRVTLayer& ref)
{
    clearTile();
    poLayer_ = NULL;

    *poTileID_ = *(ref.getTileID());
    poLayer_ = ref.getLayer();

    int nFeature = ref.getFeatureCount();
    poFeatures_ = (OGRFeature**) malloc(sizeof(OGRFeature*)nFeature);
    poFeatureCompatibleFlags_ = (int*) malloc(sizeof(int) * nFeature);

    poFeatures_[0] = ref.getFirstFeature()->Clone();
    for(int i = 1; i<nFeature; ++i)
    {
        poFeatures_[i] = ref.getNextFeature()->Clone();
    }
    
    nFeature_ = nCompatibleFeature_ = nFeature;
    nIterator = 0;
    bOriginal_ = ref.isOriginalTile();
    return *this;
}

/* --------------------------------------------------------------------- */
/*                          ～VectorTile()                               */
/* --------------------------------------------------------------------- */

VectorTile::~VectorTile()
{
    clearTile();
    poLayer_ = NULL;
}

/* --------------------------------------------------------------------- */
/*                           clearTile()                                 */
/* --------------------------------------------------------------------- */

int VectorTile::clearTile()
{
    if(poTileID_)
    {
        free( poTileID_ );
        poTileID_ = NULL;
    }

    for(int i=0; i<nFeature_; ++i)
    {
        if(poFeatures_[i])
        {
            free( poFeatures_[i] );
            poFeatures_[i] = NULL;
        }
    }
    free( poFeatures_ );
    free( poFeatureCompatibleFlags_ );

    nFeature_ = 0;
    nCompatibleFeature_ = 0;
    nIterator_ = 0;
    bOriginal_ = 1;

    return 0;
}

/* --------------------------------------------------------------------- */
/*                              getTileID()                              */
/* --------------------------------------------------------------------- */

const TileID* VectorTile::getTileID()
{
    return poTileID_;
}

/* --------------------------------------------------------------------- */
/*                              getLayer()                               */
/* --------------------------------------------------------------------- */

const OGRVTLayer* VectorTile::getLayer()
{
    return poLayer_;
}

/* --------------------------------------------------------------------- */
/*                              isOriginalTile()                         */
/* --------------------------------------------------------------------- */

int VectorTile::isOriginalTile()
{
    return bOriginal_;
}

/* --------------------------------------------------------------------- */
/*                           getFeatureCount()                           */
/* --------------------------------------------------------------------- */

int VectorTile::getFeatureCount()
{
    return nFeature_;
}

/* --------------------------------------------------------------------- */
/*                          getFirstFeature()                            */
/* --------------------------------------------------------------------- */

OGRFeature*  VectorTile::getFirstFeature()
{
    nIterator_ = 0;
    if(papoFeatures_ && papoFeatures_[0] && poFeatureCompatibleFlags_[0])
        return papoFeatures_[0];
    return NULL;
}

/* --------------------------------------------------------------------- */
/*                          getNextFeature()                             */
/* --------------------------------------------------------------------- */

OGRFeature*  VectorTile::getNextFeature()
{
    if(++nIterator_ >= nFeature_)
    {
        return NULL;
    }
    if(papoFeatures_ && papoFeatures_[nIterator_] && 
            poFeatureCompatibleFlags[nIterator_])
    {
        return papoFeatures_[0];
    }
    return NULL;
}

/* --------------------------------------------------------------------- */
/*                             resetReading()                            */
/* --------------------------------------------------------------------- */

void VectorTile::resetReading()
{
    nIterator_ = 0;
}

/* --------------------------------------------------------------------- */
/*                            deSerialize()                              */
/* --------------------------------------------------------------------- */

int VectorTile::deSerialize(char* buf)
{
    printf("feilunzhou: base class's function, you should implemente \
            this by yourself\n");
    return 0;
}

/* --------------------------------------------------------------------- */
/*                             fetchTile()                               */
/* --------------------------------------------------------------------- */

int VectorTile::fetchTile(CPLString tilekey)
{
    clearTile();
    poTileID_->setFromString(tileKey);
    if( deSerialize(poLayer_->getValue(poTileID_->toString())) )
    {
        return 0;
    }
    return 1;
}

/* --------------------------------------------------------------------- */
/*                      deleteIncompatibleFeatures()                     */
/* --------------------------------------------------------------------- */

int VectorTile::deleteIncompatibleFeatures()
{
    for(int i=0; i<nFeature; ++i)
    {
        if(poFeatureCompatibleFlags_[i] && poFeatures_[i])
            free(poFeatures_[i]);
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              filteFID()                               */
/* --------------------------------------------------------------------- */

int VectorTile::filteFID()
{
    for(int i=0; i<nFeature; ++i)
    {
        if(poFeatureCompatibleFlags_[i] && poFeatures_[i] && 
                poLayer_->getHash()->hasKey( poFeatures_[i]->GetFid()) )
        {
            poFeatureCompatibleFlags_[i] = 0;
            nCompatibleFeature_--;
        }
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              filteGeometry()                          */
/* --------------------------------------------------------------------- */

int VectorTile::filteGeometry()
{
    for(int i=0; i<nFeature; ++i)
    {
        if(poFeatureCompatibleFlags_[i] && poFeatures_[i] && 
                /* TODO: add geometry filtering judgement */
                poLayer_->getHash()->hasKey( poFeatures_[i]->GetFid()) )
        {
            poFeatureCompatibleFlags_[i] = 0;
            nCompatibleFeature_--;
        }
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              filteAttributes()                        */
/* --------------------------------------------------------------------- */

int VectorTile::filteAttributes()
{
    printf("not implemented yet");
    return 0;
}
