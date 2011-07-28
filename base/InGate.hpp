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

/*
@ authors
Andrea Di Pietro <andrea.dipietro@for.unipi.it> 
Nicola Bonelli <bonelli@cnit.it>
*/


#ifndef _INGATE_HPP_
#define _INGATE_HPP_ 


#include<Msg.hpp>
#include <vector>
#include<algorithm>
#include<dbqueue.hpp>

namespace bm { 

    class OutGate;
    class Block;
    
    class InGate
    {
    public:

        InGate(Block& own,int id)
        : m_owner(&own), m_queue(), m_peers(), m_index(id)
        {}

        ~InGate()
        {}

        InGate(const InGate&) = delete;
        InGate& operator=(const InGate&) = delete;

        void swap(InGate &other)
        {
            std::swap(m_owner, other.m_owner);
            std::swap(m_queue, other.m_queue);
            std::swap(m_peers, other.m_peers);
            std::swap(m_index, other.m_index);
        }

        InGate(InGate&& other)
        : m_owner(std::move(other.m_owner)),
        m_queue(std::move(other.m_queue)), 
        m_peers(std::move(other.m_peers)), 
        m_index(std::move(other.m_index))
        {}

        InGate& operator=(InGate&& other)
        { 
            InGate tmp(std::move(other));
            tmp.swap(*this);
            return *this;
        }


        void connect_to(OutGate& out);

        std::shared_ptr<const Msg> dequeue()
        {
            std::shared_ptr<const Msg> tmp;
            m_queue.pop(tmp);
            return tmp;
        }

        void disconnect_gate(OutGate* in);


        int receive(std::shared_ptr<const Msg>&& inm, std::pair<Block*, int> /*key*/ );
        void disconnect_all();



    private:
        Block *m_owner;
        DBQueue<std::shared_ptr<const Msg>,1024> m_queue;
        std::vector<OutGate*> m_peers;
        int m_index;
    };

} // namespace bm

#endif /* _INGATE_HPP_ */
