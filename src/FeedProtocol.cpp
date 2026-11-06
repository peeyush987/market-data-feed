#include "FeedProtocol.h"
#include <limits>

namespace market::protocol {

    namespace
    {
        bool toWireEventType(
        EventType type,
        WireEventType& wireType
        )
        {
            switch (type)
            {
                case EventType::Add:
                    wireType = WireEventType::Add;
                    return true;

                case EventType::Cancel:
                    wireType = WireEventType::Cancel;
                    return true;

                case EventType::Modify:
                    wireType = WireEventType::Modify;
                    return true;

                case EventType::Trade:
                    wireType = WireEventType::Trade;
                    return true;
            }

            return false;
        }

        bool toWireSide(Side side, WireSide& wireSide)
        {
            switch (side)
            {
                case Side::Buy:
                    wireSide = WireSide::Buy;
                    return true;

                case Side::Sell:
                    wireSide = WireSide::Sell;
                    return true;
            }

            return false;
        }

        bool isValidEvent(const MarketEvent& event)
        {
            // Basic validation
            if (event.sequence == 0) return false;

            if (event.orderId == 0) return false;

            if (event.priceTicks < 0) return false;

            // Event-specific validation
            switch (event.type)
            {
                case EventType::Add:
                case EventType::Modify:
                case EventType::Trade:
                    if (event.priceTicks == 0 || event.quantity == 0)
                        return false;
                    break;

                case EventType::Cancel:
                    // Zero price and quantity are allowed.
                    break;

                default:
                    return false;
            }

            return true;
        }


        bool fromWireEventType(
        WireEventType wireType,
        EventType& type
        )
        {
            switch (wireType)
            {
                case WireEventType::Add:
                    type = EventType::Add;
                    return true;

                case WireEventType::Cancel:
                    type = EventType::Cancel;
                    return true;

                case WireEventType::Modify:
                    type = EventType::Modify;
                    return true;

                case WireEventType::Trade:
                    type = EventType::Trade;
                    return true;
            }

            return false;
        }

        bool fromWireSide(
        WireSide wireSide,
        Side& side
        )
        {
            switch (wireSide)
            {
                case WireSide::Buy:
                    side = Side::Buy;
                    return true;

                case WireSide::Sell:
                    side = Side::Sell;
                    return true;
            }

            return false;
        }
    }


    bool writeU64BE(
        EncodedMessage& message,
        std::size_t offset,
        std::uint64_t value
    )
    {
        if (offset > MESSAGE_SIZE - 8) {
            return false;
        }
        for (std::size_t i = 0; i < 8; ++i) {
            int shift = 56 - 8 * i;
            message[offset + i] = (value >> shift) & 0xFF;
        }
        return true;
    }

    bool writeU32BE(
    EncodedMessage& message,
    std::size_t offset,
    std::uint32_t value
    )
    {
        if(offset > MESSAGE_SIZE - 4)
        {
            return false;
        }
        for (std::size_t i = 0; i < 4; ++i)
        {
            int shift = 24 - 8 * i;
            message[offset + i] = (value >> shift) & 0xFF;
        }
        return true;
    }

    bool encode(
    const MarketEvent& event,
    EncodedMessage& output
    )
    {
        if (!isValidEvent(event))
        {
            return false;
        }

        WireEventType wireType;
        WireSide wireSide;

        if (!toWireEventType(event.type, wireType))
        {
            return false;
        }

        if (!toWireSide(event.side, wireSide))
        {
            return false;
        }

        EncodedMessage temp{};

        // Sequence: 8 bytes
        if (!writeU64BE(temp, 0, event.sequence))
            return false;

        // Event type: 1 byte
        temp[8] = static_cast<std::uint8_t>(wireType);

        // Side: 1 byte
        temp[9] = static_cast<std::uint8_t>(wireSide);

        // Order ID: 8 bytes
        if (!writeU64BE(temp, 10, event.orderId))
            return false;

        // Related order ID: 8 bytes
        if (!writeU64BE(temp, 18, event.relatedOrderId))
            return false;

        // Price ticks: 8 bytes
        if (!writeU64BE(
                temp,
                26,
                static_cast<std::uint64_t>(event.priceTicks)))
            return false;

        // Quantity: 4 bytes
        if (!writeU32BE(temp, 34, event.quantity))
            return false;

        // Commit only after every field is encoded successfully
        output = temp;

        return true;


    }
    bool readU64BE(
    const std::uint8_t* data,
    std::size_t length,
    std::size_t offset,
    std::uint64_t& value
    )
    {
        if (data == nullptr)
            return false;

        if (offset > length || length - offset < 8)
            return false;

        std::uint64_t result = 0;

        for (std::size_t i = 0; i < 8; ++i)
        {
            result = (result << 8) | data[offset + i];
        }

        value = result;
        return true;
    }
    bool readU32BE(
    const std::uint8_t* data,
    std::size_t length,
    std::size_t offset,
    std::uint32_t& value
    )
    {
        if (data == nullptr)
            return false;

        if (offset > length || length - offset < 4)
            return false;

        std::uint32_t result = 0;

        for (std::size_t i = 0; i < 4; ++i)
        {
            result = (result << 8) | data[offset + i];
        }

        value = result;
        return true;
    }

    bool decode(
    const std::uint8_t* data,
    std::size_t length,
    MarketEvent& output
    )
    {
        // Reject null input.
        if (data == nullptr)
            return false;

        // Our protocol requires exactly 38 bytes.
        if (length != MESSAGE_SIZE)
            return false;

        std::uint64_t sequence = 0;

        if (!readU64BE(data, length, 0, sequence))
            return false;

        std::uint8_t rawEventType = data[8];

        EventType eventType;

        if (!fromWireEventType(
                static_cast<WireEventType>(rawEventType),
                eventType))
        {
            return false;
        }

        std::uint8_t rawSide = data[9];

        Side side;

        if (!fromWireSide(
                static_cast<WireSide>(rawSide),
                side))
        {
            return false;
        }

        std::uint64_t orderId = 0;

        if (!readU64BE(data, length, 10, orderId))
            return false;


        std::uint64_t relatedOrderId = 0;

        if (!readU64BE(data, length, 18, relatedOrderId))
            return false;


        std::uint64_t rawPrice = 0;

        if (!readU64BE(data, length, 26, rawPrice))
            return false;

        if (rawPrice > static_cast<std::uint64_t>(
        std::numeric_limits<std::int64_t>::max()))
        {
            return false;
        }

        std::int64_t priceTicks = static_cast<std::int64_t>(rawPrice);

        std::uint32_t quantity = 0;

        if (!readU32BE(data, length, 34, quantity))
            return false;

        MarketEvent temp{};

        temp.sequence = sequence;
        temp.type = eventType;
        temp.side = side;
        temp.orderId = orderId;
        temp.relatedOrderId = relatedOrderId;
        temp.priceTicks = priceTicks;
        temp.quantity = quantity;

        if (!isValidEvent(temp))
            return false;

        output = temp;

        return true;
    }

} // namespace market::protocol
