#include <iostream>
using namespace std;

typedef struct _ListNode HashBucket;

class HashTable
{
    HashBucket**        papoHash_;
    mutable int         nSlot_;
    int                 nNode_;

    mutable int         iPrimeIndex_;

    typedef int (*HashFunc)(int);
    HashFunc            fnHash_;

public:
                        HashTable();
                        HashTable(HashFunc);
                        ~HashTable();

    int                 has(int) const;
    int                 insert(int);
    int                 remove(int);
    int                 rehash() ;

private:
    int                 init(int );
    int                 clear();
};
