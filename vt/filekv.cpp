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
    strcpy(pszRootPath, rootPaht.cStr());
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

    pszRootPath = (char*) malloc(strlen(refp.rootPath.c_str()) + 1);
    pszRootPath = memcpy(pszRootPath, refp.pszRootPath.c_str(), 
            strlen(refp.rootPath.c_str()) + 1);
}

/* --------------------------------------------------------------------- */
/*                             operator =                                */
/* --------------------------------------------------------------------- */

const FileKV& operator= (const FileKV& rep)
{
    pszRootPath = (char*) malloc(strlen(refp.rootPath.c_str()) + 1);
    pszRootPath = memcpy(pszRootPath, refp.pszRootPath.c_str(), 
            strlen(refp.rootPath.c_str()) + 1);
    return *this;
}

/* --------------------------------------------------------------------- */
/*                              getName()                                */
/* --------------------------------------------------------------------- */

const char* FileKV::getName()
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
    pszRootPath = (char*) malloc(strlen(rootPath.c_str()) + 1);
    strcpy(pszRootPath, rootPaht.cStr());

    return 0;
}

/* --------------------------------------------------------------------- */
/*                              getValue()                               */
/* --------------------------------------------------------------------- */

unsigned char* FileKV::getValue(CPLString strKey)
{
    char* pszAbsolutePath = (char*) malloc( strlen(pszRootPath) + 1 + 
            strlen(strKey.c_str()) + 1);
    strcpy(pszAbsolutePath, pszRootPath);
    strcat(pszAbsolutePath, strKey.c_str());

    FILE* fp = fopen( pszAbsolutePath, "rwb" );

}
