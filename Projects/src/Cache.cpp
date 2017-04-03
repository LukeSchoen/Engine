#include "Cache.h"
#include "FilePath.h"
#include <string.h>

struct CacheElement
{
  CacheElement(const char *_key, void *_data)
  {
    key = _key;
    data = _data;
  }
  const char *key;
  void *data;
};

void Cache::AddItem(const char *key, void *data)
{
  if(cacheData == nullptr)
    cacheData = new Vector(sizeof(CacheElement));

  if (GetItem(key) == nullptr) // don't add the same key twice!
  {
    CacheElement e(key, data);
    cacheData->Add(&e);
  }
}

void *Cache::GetItem(const char *key)
{
  if (cacheData != nullptr)
    for (int i = 0; i < cacheData->count; i++)
    {
      CacheElement *e = (CacheElement*)cacheData->Get(i);
      if (strcmp(e->key, key) == 0)
        return e->data;
    }
 return nullptr;
}

void Cache::DelItem(const char *key)
{

}
