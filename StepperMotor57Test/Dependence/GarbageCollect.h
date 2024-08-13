#ifndef MADTOMDEV_BASE_GARBAGECOLLECT_H
#define MADTOMDEV_BASE_GARBAGECOLLECT_H

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>

using namespace std;

namespace MadTomDev
{
    namespace Base
    {        
        void FreeList(list<char *> * list);
        void FreeVector(vector<char *> * list);
    }
}

#endif