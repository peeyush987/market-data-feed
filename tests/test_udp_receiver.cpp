#include "UdpReceiver.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    // Start our receiver.
    UdpReceiver receiver;
    assert(receiver.open(9000));

    // Create a UDP sender socket.
    int senderFd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(senderFd != -1);

    // Destination: 127.0.0.1:9000
    sockaddr_in destination{};
    destination.sin_family = AF_INET;
    destination.sin_port = htons(9000);
    destination.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Prepare a 38-byte test datagram.
    std::array<std::uint8_t, 38> sent{};

    for (std::size_t i = 0; i < sent.size(); ++i) {
        sent[i] = static_cast<std::uint8_t>(i);
    }

    // Send the datagram.
    ssize_t bytesSent = sendto(
        senderFd,
        sent.data(),
        sent.size(),
        0,
        reinterpret_cast<sockaddr*>(&destination),
        sizeof(destination)
    );

    assert(bytesSent == 38);


    // Receive the datagram.
    std::array<std::uint8_t, 38> received{};

    std::ptrdiff_t bytesReceived =
        receiver.receive(received.data(), received.size());

    assert(bytesReceived == 38);
    assert(received == sent);

    std::cout << "UDP send/receive test passed!\n";

    // Test a datagram smaller than our protocol message.
    std::array<std::uint8_t, 37> shortMessage{};

    bytesSent = sendto(
        senderFd,
        shortMessage.data(),
        shortMessage.size(),
        0,
        reinterpret_cast<sockaddr*>(&destination),
        sizeof(destination)
    );

    assert(bytesSent == 37);

    bytesReceived = receiver.receive(received.data(), received.size());

    assert(bytesReceived == 37);


    // Test a datagram larger than our receive buffer.
    std::array<std::uint8_t, 50> oversizedMessage{};

    bytesSent = sendto(
        senderFd,
        oversizedMessage.data(),
        oversizedMessage.size(),
        0,
        reinterpret_cast<sockaddr*>(&destination),
        sizeof(destination)
    );

    assert(bytesSent == 50);

    bytesReceived = receiver.receive(received.data(), received.size());

    assert(bytesReceived == -1);

    close(senderFd);

    return 0;
}