#include "FeedProtocol.h"
#include "MarketEvent.h"

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


    // Test complete MarketEvent encoding

    market::MarketEvent event{};

    event.sequence = 0x0102030405060708ULL;
    event.type = market::EventType::Add;
    event.orderId = 0x1112131415161718ULL;
    event.relatedOrderId = 0x2122232425262728ULL;
    event.side = market::Side::Buy;
    event.priceTicks = 0x3132333435363738LL;
    event.quantity = 0x41424344;

    market::protocol::EncodedMessage encoded{};

    bool encodeSuccess = market::protocol::encode(event, encoded);

    assert(encodeSuccess);

    const std::uint8_t expectedMessage[] = {
        // Sequence: bytes 0-7
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,

        // Event type: byte 8 (ADD = 1)
        0x01,

        // Side: byte 9 (BUY = 1)
        0x01,

        // Order ID: bytes 10-17
        0x11, 0x12, 0x13, 0x14,
        0x15, 0x16, 0x17, 0x18,

        // Related order ID: bytes 18-25
        0x21, 0x22, 0x23, 0x24,
        0x25, 0x26, 0x27, 0x28,

        // Price ticks: bytes 26-33
        0x31, 0x32, 0x33, 0x34,
        0x35, 0x36, 0x37, 0x38,

        // Quantity: bytes 34-37
        0x41, 0x42, 0x43, 0x44
    };

    static_assert(
        sizeof(expectedMessage) == market::protocol::MESSAGE_SIZE,
        "Expected message must be exactly 38 bytes"
    );

    for (std::size_t i = 0; i < market::protocol::MESSAGE_SIZE; ++i) {
        assert(encoded[i] == expectedMessage[i]);
    }

    std::cout << "Complete MarketEvent encoding test passed\n";


    // Test transactional output: failed encoding must not modify output

    market::protocol::EncodedMessage previousOutput{};
    previousOutput.fill(0xAA);

    market::MarketEvent invalidEvent = event;
    invalidEvent.sequence = 0; // Invalid: sequence must be nonzero.

    bool invalidEncodeResult =
        market::protocol::encode(invalidEvent, previousOutput);

    assert(!invalidEncodeResult);

    for (std::uint8_t byte : previousOutput) {
        assert(byte == 0xAA);
    }

    std::cout << "Transactional encoding test passed\n";


    // Test 64-bit big-endian decoding

    std::uint64_t decoded64 = 0;

    bool read64Success = market::protocol::readU64BE(
        message.data(),
        message.size(),
        0,
        decoded64
    );

    assert(read64Success);
    assert(decoded64 == 0x123456789ABCDEF0ULL);

    std::cout << "64-bit big-endian decoding test passed\n";


    // Test 32-bit big-endian decoding

    std::uint32_t decoded32 = 0;

    bool read32Success = market::protocol::readU32BE(
        message32.data(),
        message32.size(),
        0,
        decoded32
    );

    assert(read32Success);
    assert(decoded32 == 0x12345678);

    std::cout << "32-bit big-endian decoding test passed\n";


    // Test invalid offset for 64-bit reader

    decoded64 = 0xAAAAAAAAAAAAAAAAULL;

    bool invalidRead64 = market::protocol::readU64BE(
        message.data(),
        message.size(),
        31,  // Only 7 bytes remain.
        decoded64
    );

    assert(!invalidRead64);
    assert(decoded64 == 0xAAAAAAAAAAAAAAAAULL);

    std::cout << "64-bit reader bounds checking test passed\n";


    // Test invalid offset for 32-bit reader

    decoded32 = 0xAAAAAAAA;

    bool invalidRead32 = market::protocol::readU32BE(
        message32.data(),
        message32.size(),
        35,  // Only 3 bytes remain.
        decoded32
    );

    assert(!invalidRead32);
    assert(decoded32 == 0xAAAAAAAA);

    std::cout << "32-bit reader bounds checking test passed\n";


    // Test null pointer handling

    bool nullRead64 = market::protocol::readU64BE(
        nullptr, 38, 0, decoded64
    );

    bool nullRead32 = market::protocol::readU32BE(
        nullptr, 38, 0, decoded32
    );

    assert(!nullRead64);
    assert(!nullRead32);

    std::cout << "Null pointer handling tests passed\n";

    // Test complete encode -> decode round trip

    market::MarketEvent decoded{};

    bool decodeSuccess = market::protocol::decode(
        encoded.data(),
        encoded.size(),
        decoded
    );

    assert(decodeSuccess);

    assert(decoded.sequence == event.sequence);
    assert(decoded.type == event.type);
    assert(decoded.orderId == event.orderId);
    assert(decoded.relatedOrderId == event.relatedOrderId);
    assert(decoded.side == event.side);
    assert(decoded.priceTicks == event.priceTicks);
    assert(decoded.quantity == event.quantity);

    std::cout << "Encode/decode round-trip test passed\n";

    // Test decoder rejects invalid message lengths


    bool shortMessage = market::protocol::decode(
        encoded.data(), 37, decoded
    );

    bool longMessage = market::protocol::decode(
        encoded.data(), 39, decoded
    );

    assert(!shortMessage);
    assert(!longMessage);

    std::cout << "Invalid message length tests passed\n";

    // Test decoder rejects invalid event type

    auto invalidTypeMessage = encoded;
    invalidTypeMessage[8] = 99;

    bool invalidTypeSuccess = market::protocol::decode(
        invalidTypeMessage.data(),
        invalidTypeMessage.size(),
        decoded
    );

    assert(!invalidTypeSuccess);

    std::cout << "Invalid event type test passed\n";

    // Test decoder rejects invalid side

    auto invalidSideMessage = encoded;
    invalidSideMessage[9] = 99;

    bool invalidSideSuccess = market::protocol::decode(
        invalidSideMessage.data(),
        invalidSideMessage.size(),
        decoded
    );

    assert(!invalidSideSuccess);

    std::cout << "Invalid side test passed\n";

    // Test decoder rejects price greater than INT64_MAX

    auto invalidPriceMessage = encoded;

    market::protocol::writeU64BE(
        invalidPriceMessage,
        26,
        UINT64_MAX
    );

    bool invalidPriceSuccess = market::protocol::decode(
        invalidPriceMessage.data(),
        invalidPriceMessage.size(),
        decoded
    );

    assert(!invalidPriceSuccess);

    std::cout << "Out-of-range price test passed\n";

    // Test transactional decoding

    market::MarketEvent originalOutput{};
    originalOutput.sequence = 777;
    originalOutput.orderId = 888;

    invalidMessage[8] = 99;


        success = market::protocol::decode(
        invalidMessage.data(),
        invalidMessage.size(),
        originalOutput
    );

    assert(!success);
    assert(originalOutput.sequence == 777);
    assert(originalOutput.orderId == 888);

    std::cout << "Transactional decoding test passed\n";

    // Test decoder rejects zero sequence number

    auto invalidSequenceMessage = encoded;

    market::protocol::writeU64BE(
        invalidSequenceMessage,
        0,
        0
    );

    bool invalidSequenceSuccess = market::protocol::decode(
        invalidSequenceMessage.data(),
        invalidSequenceMessage.size(),
        decoded
    );

    assert(!invalidSequenceSuccess);

    std::cout << "Invalid sequence test passed\n";

    invalidSequenceMessage = encoded;

    market::protocol::writeU64BE(
        invalidSequenceMessage, 0, 0
    );

    assert(!market::protocol::decode(
        invalidSequenceMessage.data(),
        invalidSequenceMessage.size(),
        decoded
    ));

    std::cout << "Invalid sequence test passed\n";


    auto invalidOrderIdMessage = encoded;

    market::protocol::writeU64BE(
        invalidOrderIdMessage, 10, 0
    );

    assert(!market::protocol::decode(
        invalidOrderIdMessage.data(),
        invalidOrderIdMessage.size(),
        decoded
    ));

    std::cout << "Invalid order ID test passed\n";


    // Test invalid price and quantity combinations

    auto invalidAdd = encoded;
    market::protocol::writeU64BE(invalidAdd, 26, 0);
    assert(!market::protocol::decode(
        invalidAdd.data(), invalidAdd.size(), decoded
    ));

    auto invalidModify = encoded;
    invalidModify[8] = 3; // MODIFY
    market::protocol::writeU32BE(invalidModify, 34, 0);
    assert(!market::protocol::decode(
        invalidModify.data(), invalidModify.size(), decoded
    ));

    auto invalidTrade = encoded;
    invalidTrade[8] = 4; // TRADE
    market::protocol::writeU64BE(invalidTrade, 26, 0);
    assert(!market::protocol::decode(
        invalidTrade.data(), invalidTrade.size(), decoded
    ));

    std::cout << "Invalid price and quantity tests passed\n";
}