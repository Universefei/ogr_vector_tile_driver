#include "ogr_vt.h"
#include "ogr_feature.h"


/* --------------------------------------------------------------------- */
/*                          VectorTile()                                 */
/* --------------------------------------------------------------------- */

VectorTile::VectorTile(OGRVTLayer* poLayer, TileID* poTileID):
    poTileID_( poTileID ? ( new TileID(*poTileID) ) : NULL ),
    poLayer_(poLayer),
    poFeatures_(NULL),
    poFeatureCompatibleFlags_(NULL),
    nFeature_(0),
    nCompatibleFeature_(0),
    nIterator_(0),
    bOriginal_(1),
    bFIDFilted_(0)
{
}

/* --------------------------------------------------------------------- */
/*                          copy constructor                             */
/* --------------------------------------------------------------------- */

VectorTile::VectorTile(const VectorTile& ref)
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

VectorTile& VectorTile::operator= (const VectorTile& ref)
{

    if(this == &ref) return *this; /* effective c++ item 11 */

    clearTile();

    *poTileID_ = *(ref.getTileID());
    poLayer_ = ref.getLayer();

    int nFeature = ref.getFeatureCount();
    papoFeatures_ = (OGRFeature**) malloc(sizeof(OGRFeature*)nFeature);
    poFeatureCompatibleFlags_ = (int*) malloc(sizeof(int) * nFeature);

    papoFeatures_[0] = ref.getFirstFeature()->Clone();
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
        if(papoFeatures_[i])
        {
            free( papoFeatures_[i] );
            papoFeatures_[i] = NULL;
        }
    }
    free( papoFeatures_ );
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

const TileID* VectorTile::getTileID() const
{
    return poTileID_;
}

/* --------------------------------------------------------------------- */
/*                              getLayer()                               */
/* --------------------------------------------------------------------- */

const OGRVTLayer* VectorTile::getLayer() const
{
    return poLayer_;
}

/* --------------------------------------------------------------------- */
/*                              isOriginalTile()                         */
/* --------------------------------------------------------------------- */

int VectorTile::isOriginalTile() const
{
    return bOriginal_;
}

/* --------------------------------------------------------------------- */
/*                           getFeatureCount()                           */
/* --------------------------------------------------------------------- */

int VectorTile::getFeatureCount() const
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
            poFeatureCompatibleFlags_[nIterator_])
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

int VectorTile::deSerialize(unsigned char* buf)
{
    printf("feilunzhou: base class's function, you should implemente \
            this by yourself\n");
    return 0;
}

/* --------------------------------------------------------------------- */
/*                             fetchTile()                               */
/* --------------------------------------------------------------------- */

int VectorTile::fetchTile(TileID* poTileID)
{
    clearTile();
    poTileID_ = new TileID(*poTileID);

    CPLString key(poTileID_->toString());
    unsigned char* poRowData = poLayer_->GetKVStore()->getValue(key);

    if(deSerialize(poRowData))
    {
        free(poRowData);
        return 0;
    }
    free(poRowData);
    return 1;
}

int VectorTile::fetchTile(const char* pszLayerName, int x, int y, int z)
{

    clearTile();
    poTileID_ = new TileID(pszLayerName, x, y, z);

    CPLString key(poTileID_->toString());
    unsigned char* poRowData = poLayer_->GetKVStore()->getValue(key);
    if(deSerialize(poRowData))
    {
        free(poRowData);
        performFilting();
        return 0;
    }
    free(poRowData);
    return 1;
}

/* --------------------------------------------------------------------- */
/*                      deleteIncompatibleFeatures()                     */
/* --------------------------------------------------------------------- */

int VectorTile::deleteIncompatibleFeatures()
{
    for(int i=0; i<nFeature_; ++i)
    {
        if(poFeatureCompatibleFlags_[i] && papoFeatures_[i])
            free(papoFeatures_[i]);
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              performFilting()                         */
/* --------------------------------------------------------------------- */

/**
 * if no error return 0, else return 1
 */

int VectorTile::performFilting(int bFilteGeom, int bFilteAttr)
{
    int fidFiltingFlag = filteFID();
    int geomFiltingFlag = 1;
    int attrFiltingFlag = 1;

    if(bFilteGeom)
        geomFiltingFlag = filteGeometry();
    if(bFilteAttr)
        attrFiltingFlag = filteAttributes();

    return  !((!fidFiltingFlag) && 
              (bFilteGeom ? (!geomFiltingFlag) : 1) && 
              (bFilteAttr ? (!attrFiltingFlag) : 1) );
}

/* --------------------------------------------------------------------- */
/*                              filteFID()                               */
/* --------------------------------------------------------------------- */

int VectorTile::filteFID()
{
    if(bFIDFilted_) return 0;

    /* if not filted */
    for(int i=0; i<nFeature_; ++i)
    {
        if( poFeatureCompatibleFlags_[i] && papoFeatures_[i] && 
                poLayer_->GetHash()->has( poFeatures_[i]->GetFid()) )
        {
            poFeatureCompatibleFlags_[i] = 0;
            nCompatibleFeature_--;
        }
    }
    bFIDFilted_ = 1;
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              filteGeometry()                          */
/* --------------------------------------------------------------------- */

int VectorTile::filteGeometry()
{
    for(int i=0; i<nFeature_; ++i)
    {
        if(poFeatureCompatibleFlags_[i] && papoFeatures_[i] && 
                /* TODO: add geometry filtering judgement */
                poLayer_->GetHash()->has( papoFeatures_[i]->GetFid()) )
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

/* --------------------------------------------------------------------- */
/*                              commitToLayer()                          */
/* --------------------------------------------------------------------- */

int VectorTile::commitToLayer() const
{
    if(!bFIDFilted_) filteFID();

    /* copy feature into layer */
    for(int i=0; i<nFeature_; ++i)
    {
        if( poFeatureCompatibleFlags_[i] && papoFeatures_[i] )
        {
            poLayer_->CreateFeature(papoFeatures_[i]); /* deep copy */
            poLayer_->GetHash()->insert(papoFeatures_[i]->GetFid());
        }
    }

    return 0;
}
