

#include "GarbageCollect.h"

using namespace MadTomDev::Base;

void FreeList(list<char *> *list)
{
    if (!list->empty())
    {
        char *str;
        for (int i = list->size(); i > 0; --i)
        {
            str = list->back();
            list->pop_back();
            free(str);
        }
    }
}
void FreeVector(vector<char *> *list)
{
    if (!list->empty())
    {
        char *str;
        for (int i = list->size(); i > 0; --i)
        {
            str = (*list)[i];
            free(str);
        }
        list->clear();
    }
}