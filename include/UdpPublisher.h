#ifndef UDP_PUBLISHER_H
#define UDP_PUBLISHER_H

#include "FeedProtocol.h"

#include <cstdint>

class UdpPublisher {
public:
    UdpPublisher();
    ~UdpPublisher();

    UdpPublisher(const UdpPublisher&) = delete;
    UdpPublisher& operator=(const UdpPublisher&) = delete;

    bool open();

    bool send(
        const market::protocol::EncodedMessage& message
    );

private:
    int socketFd_;
};

#endif