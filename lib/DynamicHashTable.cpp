/* Copyright (c) 2011, NEC Europe Ltd, Consorzio Nazionale 
 * Interuniversitario per le Telecomunicazioni, Institut 
 * Telecom/Telecom Bretagne, ETH Zürich, INVEA-TECH a.s. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of NEC Europe Ltd, Consorzio Nazionale 
 *      Interuniversitario per le Telecomunicazioni, Institut Telecom/Telecom 
 *      Bretagne, ETH Zürich, INVEA-TECH a.s. nor the names of its contributors 
 *      may be used to endorse or promote products derived from this software 
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT 
 * HOLDERBE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 */

#ifdef DYNAMICHASHTABLE_HPP
#include <cstring>
#include <iostream>
#include <vector>


#include "HashTable.hpp"


namespace bm
{

template<typename T >
bool DynamicHashTable<T>::check(unsigned int i, Key& key)
{
    unsigned int k=0;
    for (unsigned int j = 0; k < realcolumnsize[i]; ++j)
    {
        if (nodes[i][j].getKey().keylength!=0)
        {
            if (nodes[i][j].getKey()==key)
                return true;
            ++k;
        }
    }
    return false;
}

template<typename T >
T* DynamicHashTable<T>::get(unsigned int i,Key& key)
{
    unsigned int k=0;
    for (unsigned int j = 0; k < realcolumnsize[i]; ++j)
    {
        if (nodes[i][j].getKey().keylength!=0)
        {
            if (nodes[i][j].getKey()==key)
                return &(nodes[i][j].getState());
            ++k;
        }
    }
    return NULL;
}

template<typename T >
std::vector<Key*> DynamicHashTable<T>::get(unsigned int i)
{
    std::vector<Key*> nodeskey;
    if (realcolumnsize[i])
    {
        unsigned int k=0;
        for (unsigned int j = 0; k < realcolumnsize[i]; ++j)
        {
            if (nodes[i][j].getKey().keylength!=0)
            {
                nodeskey.push_back(&(nodes[i][j].getKey()));
                ++k;
            }
        }
        
    }
    return nodeskey;
}

template<typename T >
bool DynamicHashTable<T>::remove(unsigned int i, Key& key)
{
    unsigned int k=0;
    for (unsigned int j = 0; k < realcolumnsize[i]; ++j)
    {
        if (nodes[i][j].getKey().keylength!=0)
        {
            if (nodes[i][j].getKey()==key)
            {
                nodes[i][j].reset();
                --realcolumnsize[i];
                return true;
            }
            ++k;
        }
    }
    return false;
}

template<typename T >
UpdateStatus DynamicHashTable<T>::update_add(unsigned int i, Key& key, T s)
{
    unsigned int k=0;
    bool nodefree = false;
    unsigned int nnode=0;
    for (unsigned int j = 0; k < realcolumnsize[i]; ++j)
    {
        if (nodes[i][j].getKey().keylength!=0)
        {
            if (nodes[i][j].getKey()==key)
            {
                nodes[i][j].updateState(s);
                return UPDATED;
            }
            ++k;
        }
        else if(!nodefree)
        {
            nodefree = true;
            nnode=j;
        }
    }
    
    if(realcolumnsize[i]<columnsize)
    {
        if (!nodefree)
            nnode = realcolumnsize[i];
        nodes[i][nnode].setKey(key);
        nodes[i][nnode].setState(s);
        ++realcolumnsize[i];
        return ADDED;
    }
    return NOTFREESPACE;
}

template<typename T >
bool DynamicHashTable<T>::update(unsigned int i,Key& key,T s)
{
    unsigned int k=0;
    for (unsigned int j = 0; k < realcolumnsize[i]; ++j)
    {
        if (nodes[i][j].getKey()==key)
        {
            nodes[i][j].updateState(s);
            return true;
        }
        k++;
    }
    return false;
}

template<typename T >
bool DynamicHashTable<T>::add(unsigned int i,Key& key,T s)
{
    if (realcolumnsize[i]<columnsize)
        for (unsigned int j = 0; j < columnsize; ++j)
            if (nodes[i][j].getKey().keylength==0)
            {
                nodes[i][j].setKey(key);
                nodes[i][j].setState(s);
                ++realcolumnsize[i];
                return true;
            }
    return false;
}

template<typename T >
bool DynamicHashTable<T>::grow(unsigned int rowtoadd)
{
    
    for(unsigned int i = this->hashsize; i < this->hashsize + rowtoadd && (i<maxrows || maxrows==0); i++)
    {
        nodes.push_back(new HashNode<T>[columnsize]);
        
        for (unsigned int j = 0; j < columnsize; ++j)
        {
            //trick to initialize without storing update and clear functions
            nodes[i][j]=nodes[0][0];
            nodes[i][j].reset();
        }
                    
        realcolumnsize.push_back(0);
    }
    this->hashsize+=rowtoadd;
    return true;
}

template<typename T >
bool DynamicHashTable<T>::reduce(unsigned int rowtorem)
{
    for(unsigned int i = 0; i < rowtorem && nodes.size()>1; i++)
    {
        delete [] nodes.back();
        nodes.pop_back();
        realcolumnsize.pop_back();
    }
    this->hashsize-=rowtorem;
    return true;
}

template<typename T >
bool DynamicHashTable<T>::move(unsigned int from, unsigned int to, Key* key)
{
    unsigned int k=0;
    for(unsigned int i = 0; k<realcolumnsize[from]; i++)
    {
        if(nodes[from][i].getKey().keylength!=0)
        {
            if(nodes[from][i].getKey()==(*key))
            {
                unsigned int j;
                for(j=0;j<columnsize;j++)
                    if(nodes[to][j].getKey().keylength==0)
                        break;
                nodes[to][j]=nodes[from][i];
                nodes[from][i].reset();
                realcolumnsize[from]--;
                realcolumnsize[to]++;
                return true;
            }
        k++;
        }
    }
    return false;
}

template<typename T >
void DynamicHashTable<T>::print()
{
    for (unsigned int i = 0; i < this->hashsize; ++i)
    {
        std::cout << "\n" << i << "elementi " << realcolumnsize[i] << "\n";
        for(unsigned int j = 0; j < columnsize; ++j)
        {
            std::cout <<  "\t" << nodes[i][j].getKey().keylength << " - ";
            for (unsigned int z = 0; z < nodes[i][j].getKey().keylength; ++z)
                std::cout << (int) nodes[i][j].getKey().keyvalue[z];
            
            std::cout << "\t { " << nodes[i][j].getState().to_str() << " }\n";
        }
        
    }

}

}

#endif
