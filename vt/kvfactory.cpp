#include "ogr_vt.h"
#include "cpl_multiproc.h"

/**
 * reference OGRSFDriverRegistrar
 */

static void *hDRMutex = NULL;
static KVStoreFactory* volatile poGKVFactory = KVStoreFactory::GetInstance(); /* fei: why volatile */


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
    for(int i=0; i<nKVStores_; ++i)
    {
        if( !strcmp( papoKVStores_[i]->getName(), kvName.c_str() ) )
        {
            return papoKVStores_[i];
        } 
        
    }

    /* If not have, construct one */
    papoKVStores_ = (KVStore**) realloc(papoKVStores_, ++nKVStores_);
    papoKVStores_[nKVStores_ - 1] = createKVStore(kvName);
    if(papoKVStores_[nKVStores_ - 1])
        papoKVStores_[nKVStores_ - 1]->open(openInfo);

    return papoKVStores_[nKVStores_ - 1];
}

/* --------------------------------------------------------------------- */
/*                            createKVStore()                            */
/* --------------------------------------------------------------------- */

KVStore* KVStoreFactory::createKVStore(CPLString kvName)
{
    KVStore *poRet = NULL;
    if( !strcmp( kvName.c_str(), "file" ) )
    {
        poRet = new FileKV();
    }
    else if( !strcmp( kvName.c_str(), "redis" ) )
    {
        poRet = new RedisKV();
    }

    return poRet;
}

