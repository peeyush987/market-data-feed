#include "UdpPublisher.h"

#include <unistd.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

UdpPublisher::UdpPublisher()
    : socketFd_(-1)
{
}

UdpPublisher::~UdpPublisher()
{
    if (socketFd_ >= 0) {
        close(socketFd_);
    }
}

bool UdpPublisher::open()
{
    socketFd_ = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketFd_ == -1) {
        return false;
    }

    return true;
}

bool UdpPublisher::send(
    const market::protocol::EncodedMessage& message)
{
    if (socketFd_ == -1) {
        return false;
    }

    sockaddr_in destination{};

    destination.sin_family = AF_INET;
    destination.sin_port = htons(9000);
    destination.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    ssize_t bytesSent = sendto(
        socketFd_,
        message.data(),
        message.size(),
        0,
        reinterpret_cast<sockaddr*>(&destination),
        sizeof(destination)
    );

    if (bytesSent != static_cast<ssize_t>(message.size())) {
        return false;
    }

    return true;
}
