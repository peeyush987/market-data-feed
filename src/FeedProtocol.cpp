#include "FeedProtocol.h"

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

    

} // namespace market::protocol