#include "ogr_vt.h"

/* --------------------------------------------------------------------- */
/*                              FileKV()                                 */
/* --------------------------------------------------------------------- */

FileKV::FileKV():
    pszRootPath(NULL)
{
}

/* --------------------------------------------------------------------- */
/*                              FileKV()                                 */
/* --------------------------------------------------------------------- */

FileKV::FileKV(CPLString rootPath)
{
    pszRootPath = (char*) malloc(strlen(rootPath.c_str()) + 1);
    strcpy(pszRootPath, rootPath.c_str());
}

/* --------------------------------------------------------------------- */
/*                             ~FileKV()                                 */
/* --------------------------------------------------------------------- */

FileKV::~FileKV()
{
    if(pszRootPath)
    {
        free(pszRootPath);
        pszRootPath = NULL;
    }
}

/* --------------------------------------------------------------------- */
/*                             copy constructor                          */
/* --------------------------------------------------------------------- */

FileKV::FileKV(const FileKV& refp)
{

    pszRootPath = (char*) malloc(strlen(refp.pszRootPath) + 1);
    memcpy(pszRootPath, refp.pszRootPath, strlen(refp.pszRootPath) + 1);
}

/* --------------------------------------------------------------------- */
/*                             operator =                                */
/* --------------------------------------------------------------------- */

FileKV& FileKV::operator= (const FileKV& refp)
{
    if(this == &refp) return *this;
    if(pszRootPath) 
    {
        free(pszRootPath);
        pszRootPath = NULL;
    }

    pszRootPath = (char*) malloc(strlen(refp.pszRootPath) + 1);
    memcpy(pszRootPath, refp.pszRootPath, strlen(refp.pszRootPath) + 1);

    return *this;
}

/* --------------------------------------------------------------------- */
/*                              getName()                                */
/* --------------------------------------------------------------------- */

const char* FileKV::getName() const
{
    return "file";
}

/* --------------------------------------------------------------------- */
/*                                open()                                 */
/* --------------------------------------------------------------------- */

int FileKV::open(CPLString openInfo)
{
    if(pszRootPath)
    {
        free(pszRootPath);
        pszRootPath = NULL;
    }
    pszRootPath = (char*) malloc(strlen(openInfo.c_str()) + 1);
    strcpy(pszRootPath, openInfo.c_str());

    return 0;
}

/* --------------------------------------------------------------------- */
/*                              getValue()                               */
/* --------------------------------------------------------------------- */

unsigned char* FileKV::getValue(CPLString strKey) const
{
    char* pszAbsolutePath = (char*) malloc( strlen(pszRootPath) + 1 + 
            strlen(strKey.c_str()) + 1);
    strcpy(pszAbsolutePath, pszRootPath);
    strcat(pszAbsolutePath, strKey.c_str());

    FILE* fp = fopen( pszAbsolutePath, "rwb" );
    /* TODO */
    return NULL;

}
