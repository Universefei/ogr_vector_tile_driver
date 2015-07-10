#include "ogr_vt.h"

/* --------------------------------------------------------------------- */
/*                            KVStoreFactory()                           */
/* --------------------------------------------------------------------- */

KVStoreFactory::KVStoreFactory():
    papoKVStores_(NULL),
    nKVStores_(0)
{
}

/* --------------------------------------------------------------------- */
/*                            GetInstance()                              */
/* --------------------------------------------------------------------- */

KVStoreFactory* KVStoreFactory::GetInstance()
{
    if(!poGKVFactory)
    {
        CPLMutexHolderD( &hDRMutex );
        if(!poGKVFactory)
        {
            poGKVFactory = new KVStoreFactory();
        }
    }
    CPLAssert( NULL != poGKVFactory );
    return poGKVFactory;
}

/* --------------------------------------------------------------------- */
/*                            getAndOpenKVStore()                        */
/* --------------------------------------------------------------------- */

KVStore* KVStoreFactory::getAndOpenKVStore(CPLString kvName, CPLString openInfo)
{
    /* Check if already have specific KVStore in object */
    for(int i=0; i<n; ++i)
    {
        if( !strcmp( papoKVStores_[i], kvName.c_str() ) )
        {
            return papoKVStores_[i];
        } 
        
    }

    /* If not have, construct one */
    papoKVStores_ = (KVStore**) realloc(papoKVStores_, n+1);
    papoKVStores_[n] = createKVStore(kvName);
    if(papoKVStores_[n])
        papoKVStores_[n]->open(openInfo);
    ++n;

    return papoKVStores_[n - 1];
}

/* --------------------------------------------------------------------- */
/*                            createKVStore()                            */
/* --------------------------------------------------------------------- */

KVStore* KVStoreFactory::createKVStore(CPLString kvName)
{
    KVStore *poRet = NULL;
    if(kvName.c_str() == "file" )
    {
        poRet = new FileKV();
    }
    else if(kvName.c_str() == "redis")
    {
        poRet = new RedisKV();
    }

    return poRet;
}

