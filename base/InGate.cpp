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

#include <InGate.hpp>
#include <Block.hpp>

namespace bm { 

    int InGate::receive(std::shared_ptr<const Msg>&& inm, std::pair<Block*, int> /*key*/ )
    {
        if(!m_owner->is_active())
        {
            return m_owner->receive_msg(std::move(inm),m_index);
        }
        else
        {
            m_queue.push(std::move(inm));
            return 0;
        }
    }

    void InGate::connect_to(OutGate& out)
    {
        m_peers.push_back(&out);
    }

    void InGate::disconnect_gate(OutGate* in)
    {
        auto rm=std::remove(m_peers.begin(),m_peers.end(),in);

        auto end=m_peers.end();
        if(rm==end)
            throw std::runtime_error("trying to disconect a non-connected gate");

        m_peers.erase(rm,end);

    }
    void InGate::disconnect_all()
    {
        for (auto it=m_peers.begin(); it!=m_peers.end(); ++it)
        {
            (*it)->disconnect_gate(this);
        }
        m_peers.clear();
        m_queue.reset();

    }


} // namespace bm






