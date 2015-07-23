#include "ogr_vt.h"
#include "ogr_feature.h"


/* --------------------------------------------------------------------- */
/*                          VectorTile()                                 */
/* --------------------------------------------------------------------- */

VectorTile::VectorTile(OGRVTLayer* poLayer, TileID* poTileID):
    poTileID_( poTileID ? ( new TileID(*poTileID) ) : NULL ),
    poLayer_(poLayer),
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
    poTileID_ = new TileID(*ref.poTileID_);
    poLayer_ = ref.poLayer_;

    for(int i=0, int nMax = ref.papoFeatures_.size();
            i<nMax; ++i)
    {
        papoFeatures_.push_back( ref.papoFeatures_[i]->Clone() );
        poFeatureCompatibleFlags_.push_back(1);
    }
    
    nIterator = 0;
    bOriginal_ = ref.isOriginalTile();
}

/* --------------------------------------------------------------------- */
/*                          operator =                                   */
/* --------------------------------------------------------------------- */

/**
 * Notices for assignment operator
 * ===============================
 * 1] return type reference
 * 2] if(this == &rhs) check
 * 3] clear old memory before allocate new data
 * 4] return *this;
 */

VectorTile& VectorTile::operator= (const VectorTile& ref)
{

    if(this == &ref) return *this; /* effective c++ item 11 */

    clearTile();

    poTileID_ = new TileID(*ref.poTileID_);
    poLayer_ = ref.poLayer_;

    for(int i=0, int nMax = ref.papoFeatures_.size();
            i<nMax; ++i)
    {
        papoFeatures_.push_back( ref.papoFeatures_[i]->Clone() );
        poFeatureCompatibleFlags_.push_back(1);
    }
    
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

    
    for(int i=0, int nMax = papoFeatures_.size(); i < nMax ; ++i)
    {
        if(papoFeatures_[i])
        {
            free( papoFeatures_[i] );
            papoFeatures_[i] = NULL;
        }
    }

    papoFeatures_.size() = 0;
    nCompatibleFeature_.size() = 0;
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
    return papoFeatures_.size();
}

/* --------------------------------------------------------------------- */
/*                          getFirstFeature()                            */
/* --------------------------------------------------------------------- */

OGRFeature*  VectorTile::getFirstFeature()
{
    int nLen = papoFeatures_.size();
    if(nLen == 0) {return NULL};

    nIterator_ = 0;
    while( !poFeatureCompatibleFlags_[nIterator_] || 
                                            !papoFeatures_[nIterator_])
    {
        nIterator_ = (++nIterator_) % nLen;
        if(nIterator_ == 0) {return NULL};
    }

    return papoFeatures_[nIterator_];
}

/* --------------------------------------------------------------------- */
/*                          getNextFeature()                             */
/* --------------------------------------------------------------------- */

OGRFeature*  VectorTile::getNextFeature()
{
    int nLen = papoFeatures_.size();
    if(nLen == 0) {return NULL};

    do
    {
        nIterator_ = (++nIterator_) % nLen;
        if(nIterator_ == 0) return NULL;
    }
    while( !poFeatureCompatibleFlags_[nIterator_] || 
                                            !papoFeatures_[nIterator_])

    return papoFeatures_[nIterator_];
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

/**
 * return 0 if no error, else return 1
 */

int VectorTile::deSerialize(unsigned char* buf)
{
    printf("feilunzhou: base class's function, you should implemente \
            this by yourself\n");
    return 0;
}

/* --------------------------------------------------------------------- */
/*                             fetchTile()                               */
/* --------------------------------------------------------------------- */

/**
 * Return: if success, return 0; else return 1
 */

int VectorTile::fetchTile(TileID* poTileID)
{
    if(!poTileID) return 1;

    if(poTileID_ != poTileID)
    {
        clearTile();
        poTileID_ = new TileID(*poTileID); /* copy constructor */
    }

    CPLString key(poTileID_->toString());
    unsigned char* poRowData = poLayer_->GetKVStore()->getValue(key);

    int deSerializeFlag = 1;
    if(poRowData)
    {
        flag = deSerialize(poRowData);
        free(poRowData);
    }

    return deSerializeFlag;
}


int VectorTile::fetchTile(const char* pszLayerName, int x, int y, int z)
{
    TileID *poNewTileID = new TileID(pszLayerName, x, y, z);
    if(*poTileID_ == *poNewTileID)
    {
        delete poNewTileID;
        return 1;
    }

    clearTile();
    poTileID_ = poNewTileID;

    return fetchTile(poTileID_);
}


/* --------------------------------------------------------------------- */
/*                      deleteIncompatibleFeatures()                     */
/* --------------------------------------------------------------------- */

int VectorTile::deleteIncompatibleFeatures()
{
    int nMax = papoFeatures_.size();
    for(int i=0; i<nMax; ++i)
    {
        if(!poFeatureCompatibleFlags_[i] && papoFeatures_[i])
            { free(papoFeatures_[i]);}
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

    if(bFilteGeom) { geomFiltingFlag = filteGeometry();}
    if(bFilteAttr) { attrFiltingFlag = filteAttributes();}

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
    int nMax = papoFeatures_.size();
    for(int i=0; i<nMax; ++i)
    {
        if( ( poFeatureCompatibleFlags_[i] && 
                poLayer_->GetHash()->has( papoFeatures_[i]->GetFid()) ) ||
                !papoFeatures_[i] )
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
    int nMax = papoFeatures_.size();
    for(int i=0; i<nMax; ++i)
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
    int nMax = papoFeatures_.size();
    for(int i=0; i<nMax; ++i)
    {
        if( poFeatureCompatibleFlags_[i] && papoFeatures_[i] )
        {
            poLayer_->CreateFeature(papoFeatures_[i]); /* deep copy */
            poLayer_->GetHash()->insert(papoFeatures_[i]->GetFid());
        }
    }

    return 0;
}

/* --------------------------------------------------------------------- */
/*                              AddFeature()                             */
/* --------------------------------------------------------------------- */

bool VectorTile::AddFeature(OGRFeature* poFeature)

{
    if(!poFeature) return false;

    papoFeatures_.push_back( poFeature->Clone() );
    poFeatureCompatibleFlags_.push_back(1);

    return true;
}

