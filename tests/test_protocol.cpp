#include "FeedProtocol.h"

#include <cassert>
#include <iostream>

int main() {
    market::protocol::EncodedMessage message{};

    bool success = market::protocol::writeU64BE(
        message, 0, 0x123456789ABCDEF0ULL
    );

    assert(success);

    const std::uint8_t expected[] = {
        0x12, 0x34, 0x56, 0x78,
        0x9A, 0xBC, 0xDE, 0xF0
    };

    for (std::size_t i = 0; i < 8; ++i) {
        assert(message[i] == expected[i]);
    }

    std::cout << "Big-endian encoding test passed\n";
}