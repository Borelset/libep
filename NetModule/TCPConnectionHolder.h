//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_TCPCONNECTIONHOLDER_H
#define LIBEP_TCPCONNECTIONHOLDER_H

#include <memory>
#include "../Utils/noncopyable.h"
#include "TCPConnection.h"

namespace NetModule{
    class TCPConnetionHolder : Utils::noncopyable{
    public:
        TCPConnetionHolder(std::shared_ptr<TCPConnection> connPtr);
    };
}

#endif //LIBEP_TCPCONNECTIONHOLDER_H
