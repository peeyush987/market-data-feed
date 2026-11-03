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


    // Test invalid offset
    market::protocol::EncodedMessage invalidMessage{};

    bool invalidResult = market::protocol::writeU64BE(
        invalidMessage, 31, 0x123456789ABCDEF0ULL
    );

    assert(!invalidResult);

    // Verify that no bytes were modified.
    for (std::size_t i = 0; i < invalidMessage.size(); ++i) {
        assert(invalidMessage[i] == 0);
    }

    std::cout << "Bounds checking test passed\n";


    // Test 32-bit big-endian encoding
    market::protocol::EncodedMessage message32{};

    bool success32 = market::protocol::writeU32BE(
        message32, 0, 0x12345678
    );

    assert(success32);

    const std::uint8_t expected32[] = {
        0x12, 0x34, 0x56, 0x78
    };

    for (std::size_t i = 0; i < 4; ++i) {
        assert(message32[i] == expected32[i]);
    }

    std::cout << "32-bit big-endian encoding test passed\n";

    // Test invalid offset
    market::protocol::EncodedMessage invalidMessage32{};

    bool invalidResult32 = market::protocol::writeU32BE(
        invalidMessage32, 35, 0x12345678
    );

    assert(!invalidResult32);

    // Verify no bytes were modified
    for (std::size_t i = 0; i < invalidMessage32.size(); ++i) {
        assert(invalidMessage32[i] == 0);
    }

    std::cout << "32-bit bounds checking test passed\n";
}