#include "hashtable.h"
#include <iostream>
#include "cpl_conv.h"

using namespace std;

typedef struct _ListNode {
    int             key;
    _ListNode*      poNext;
} ListNode;

int ListPreInsert(ListNode*& poH, int key)
{
    ListNode* poNew = (ListNode*)malloc(sizeof(ListNode));
    poNew->key = key;
    poNew->poNext = poH;
    poH = poNew;
    return 0;
};

int ListFind(ListNode* poH, int key)
{
    ListNode* itr = poH;
    while(!itr)
    {
        if( itr->key == key ) return 1;
        itr = itr->poNext;
    }
    return 0;
};

int ListDestroy(ListNode*& poH)
{
    ListNode* tmp = NULL;
    while(poH)
    {
        tmp = poH;
        poH = poH->poNext;
        free(tmp);
    }
    return 1;
};

int ListRemove(ListNode*& poH, int key)
{
    int flag = 0;
    if(NULL == poH) return 0;
    CPLAssert(poH);

    ListNode* itr = poH;
    if(poH->key == key) 
    {
        free(itr);
        poH = poH->poNext;
        flag = 1;
        return flag;
    }

    CPLAssert(itr = poH->poNext);
    ListNode* preItr = poH;
    
    while(!itr)
    {
        if( itr->key == key )
        {
            preItr->poNext = itr->poNext;
            free(itr);
            flag = 1;
            break;
        } 
        preItr = itr;
        itr = itr->poNext;
    }; 
    return flag;
};

static const int anPrimes[] =  
{ 53, 97, 193, 389, 769, 1543, 3079, 6151,
  12289, 24593, 49157, 98317, 196613, 393241,
  786433, 1572869, 3145739, 6291469, 12582917,
  25165843, 50331653, 100663319, 201326611, 
  402653189, 805306457, 1610612741 };

int fnDefaultHash(int key)
{
    return key;
}

HashTable::HashTable():
    iPrimeIndex_(0),
    nNode_(0),
    fnHash_(fnDefaultHash)
{
    nSlot_ =  anPrimes[iPrimeIndex_];
    papoHash_ = (ListNode**)malloc(sizeof(ListNode*) * nSlot_);
}

HashTable::~HashTable()
{
}

int HashTable::has(int key) const
{
    int hashValue = fnHash_(key) % nSlot_;
    int isFound = ListFind(papoHash_[hashValue], key);
    return isFound;
};

int HashTable::insert(int key)
{
    if(has(key)) return 0;
    if(nNode_ >= nSlot_ * 4) rehash();
    int hashValue = fnHash_(key) % nSlot_;
    ListPreInsert(papoHash_[hashValue], key);
    return 1;
}

int HashTable::remove(int key)
{
    int hashValue = fnHash_(key) % nSlot_;
    ListNode* poList = papoHash_[hashValue];
    int existFlag = 0;
    if(!poList) return 0;
    if(poList->key == key)
    {
        free(poList);
        poList = NULL;
        return 1;
    }

    ListNode* poPre=poList; poList = poList->poNext;
    while(poList)
    {
        if(poList->key == key)
        {
            poPre->poNext = poList->poNext;
            existFlag = 1;
            free(poList);
            break;
        }
        poPre = poList;
        poList = poList->poNext;
    }
    return existFlag;
}

int HashTable::rehash() 
{
    int nextIndex = iPrimeIndex_ + 1;
    int oldSlot = nSlot_;
    HashBucket** oldHash = papoHash_;
    
    papoHash_ = NULL;
    init(anPrimes[nextIndex]);

    ListNode *poTmp, *poList = NULL;
    for(int i=0; i<oldSlot; ++i)
    {
        poList = oldHash[i];
        while(poList)
        {
            insert(poList->key);
            poTmp = poList;
            poList = poList->poNext;
            free(poTmp);
        }
    }

    free(oldHash);
    return 1;
}

int HashTable::init(int nSlot)
{
    if(papoHash_ && nSlot_) clear();
    for(iPrimeIndex_=0; nSlot>=anPrimes[iPrimeIndex_]; ++iPrimeIndex_);
    nSlot_ = anPrimes[iPrimeIndex_];
    papoHash_ = (HashBucket**)malloc(sizeof(HashBucket*) * nSlot_);
    memset(papoHash_, 0, nSlot_);
    return 1;
}

int HashTable::clear()
{
    ListNode *poTmp, *poList = NULL;
    for(int i=0; i<nSlot_; ++i)
    {
        poList = papoHash_[i];
        while(poList)
        {
            poTmp = poList;
            poList = poList->poNext;
            free(poTmp);
        }
    }
    free(papoHash_); papoHash_ = NULL;
    return 1;
}
