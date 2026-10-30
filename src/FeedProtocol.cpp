#include "FeedProtocol.h"

namespace market::protocol {

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

} // namespace market::protocol