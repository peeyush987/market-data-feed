#include "UdpReceiver.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>

UdpReceiver::UdpReceiver()
    : socketFd_(-1)
{
}

UdpReceiver::~UdpReceiver()
{
    if (socketFd_ >= 0) {
        close(socketFd_);
    }
}

bool UdpReceiver::open(std::uint16_t port)
{
    socketFd_ = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketFd_ == -1) {
        return false;
    }

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(
            socketFd_,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)) == -1) {

        close(socketFd_);
        socketFd_ = -1;

        return false;
    }

    return true;
}

std::ptrdiff_t UdpReceiver::receive(
    std::uint8_t* buffer,
    std::size_t bufferSize)
{
    if (buffer == nullptr || bufferSize == 0 || socketFd_ == -1) {
        return -1;
    }

    iovec bufferInfo{};
    bufferInfo.iov_base = buffer;
    bufferInfo.iov_len = bufferSize;

    msghdr message{};

    message.msg_name = nullptr;
    message.msg_namelen = 0;
    message.msg_iov = &bufferInfo;
    message.msg_iovlen = 1;
    message.msg_control = nullptr;
    message.msg_controllen = 0;
    message.msg_flags = 0;

    ssize_t bytesReceived = recvmsg(socketFd_, &message, 0);

    if (bytesReceived == -1) {
        return -1;
    }

    if (message.msg_flags & MSG_TRUNC) {
        return -1;
    }

    return static_cast<std::ptrdiff_t>(bytesReceived);
}