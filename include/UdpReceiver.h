#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

class UdpReceiver {
public:
    UdpReceiver();
    ~UdpReceiver();

    bool open(std::uint16_t port);

    std::ptrdiff_t receive(
        std::uint8_t* buffer,
        std::size_t bufferSize
    );


    UdpReceiver(const UdpReceiver&) = delete;

    UdpReceiver& operator=(const UdpReceiver&) = delete;

private:
    int socketFd_;
};

#endif