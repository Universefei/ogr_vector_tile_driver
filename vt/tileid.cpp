#include "ogr_vt.h"
#include <string.h>
/* #include <stdio.h> */

/* --------------------------------------------------------------------- */
/*                             TileID()                                  */
/* --------------------------------------------------------------------- */

TileID::TileID(const char* pszLayerName, int x, int y, int z):
    x_(x), y_(y), z_(z)
{
    pszLayerName_ = (char*)malloc(strlen(pszLayerName) + 1);
    strcpy(pszLayerName_, pszLayerName);
}


TileID::TileID(CPLString cplstr)
{
    this->setFromString(cplstr);
}


TileID::TileID(const TileID& cpy)
{
    x_ = cpy.x_;
    y_ = cpy.y_;
    z_ = cpy.z_;
    pszLayerName_ = (char*)malloc(strlen(cpy.pszLayerName_) + 1);
    strcpy(pszLayerName_, cpy.pszLayerName_);
}

/* --------------------------------------------------------------------- */
/*                             ~TileID()                                 */
/* --------------------------------------------------------------------- */

TileID::~TileID()
{
    if(pszLayerName_)
        free(pszLayerName_);
}

/* --------------------------------------------------------------------- */
/*                             operator =                                */
/* --------------------------------------------------------------------- */

TileID& TileID::operator=(const TileID& duplica)

{
    if(this == &duplica) return *this; /* effective C++ item 11 */

    x_ = duplica.x_;
    y_ = duplica.y_;
    z_ = duplica.z_;
    free(pszLayerName_); pszLayerName_ = NULL;
    pszLayerName_ = (char*)malloc(strlen(duplica.pszLayerName_) + 1);
    strcpy(pszLayerName_, duplica.pszLayerName_);
    return *this;
}

/* --------------------------------------------------------------------- */
/*                             operator ==                               */
/* --------------------------------------------------------------------- */

bool TileID::operator==(const TileID& duplica)

{
    if(this == &duplica) {return true;} 
    return ( x_ == duplica.x_ && y_ == duplica.y_ && z_ == duplica.z_ &&
            !strcmp(duplica.pszLayerName_, pszLayerName_) );
}

/* --------------------------------------------------------------------- */
/*                             setLayerName()                            */
/* --------------------------------------------------------------------- */

int TileID::setLayerName(const char* pszLayerName)
{
    if(pszLayerName_) free(pszLayerName_);
    pszLayerName_ = (char*)malloc(strlen(pszLayerName) + 1);
    strcpy(pszLayerName_, pszLayerName);
    return 0;
}

/* --------------------------------------------------------------------- */
/*                             toString()                                */
/* --------------------------------------------------------------------- */

/**
 *  Notice: User is responsible to free returned memory
 */

const char* TileID::toString() const
{
    char *buf = (char*) malloc(1024);
    char tmp[1024];
    memset(buf, 0, 1024);
    memset(tmp, 0, 200);

    strcpy(buf, pszLayerName_);
    sprintf(tmp, "#%d#%d#%d", x_, y_, z_);
    strcat(buf, tmp);

    return buf;
}

/* --------------------------------------------------------------------- */
/*                             setFromString()                           */
/* --------------------------------------------------------------------- */

int TileID::setFromString(CPLString cplstring)
{
    char layerName[200];
    char *buf = (char*) malloc(1024);
    strcpy(buf, cplstring.c_str());
    sscanf(buf, "%s#%d#%d#%d", layerName, &x_, &y_, &z_);
    if(pszLayerName_)
    {
        free(pszLayerName_);
        pszLayerName_ = NULL;
    }
    pszLayerName_ = (char*)malloc(strlen(layerName) + 1);
    strcpy(pszLayerName_, layerName);

    free(buf);
    return 0;
}

