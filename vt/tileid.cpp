#include "ogr_vt.h"
#include <string.h>

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
/*                             toString()                                */
/* --------------------------------------------------------------------- */

/**
 *  Notice: User is responsible to free returned memory
 */

char* TileID::toString()
{
    char *buf = (char*) malloc(1024);
    char tmp[1024];
    memset(buf, 0, 1024);
    memset(tmp, 0, 200);

    strcpy(buf, pszLayerName_);
    ssprintf(tmp, "#%d#%d#%d", x, y, z);
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
    sscanf(buf, "%s#%d#%d#%d", layerName, &x_, &y_, &z);
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

