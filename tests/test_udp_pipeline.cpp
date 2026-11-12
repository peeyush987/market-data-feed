
#include "MarketEvent.h"
#include "FeedProtocol.h"
#include "UdpPublisher.h"
#include "UdpReceiver.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

int main()
{
    // Initialize UDP receiver and publisher.
    UdpReceiver receiver;
    UdpPublisher publisher;

    assert(receiver.open(9000));
    assert(publisher.open());

    // Create a valid ADD market event.
    market::MarketEvent event{};

    event.sequence = 1;
    event.type = market::EventType::Add;
    event.orderId = 1001;
    event.side = market::Side::Buy;
    event.priceTicks = 10000;
    event.quantity = 5;

    // Encode the event into the 38-byte wire format.
    market::protocol::EncodedMessage encoded{};

    bool encodedSuccessfully =
        market::protocol::encode(event, encoded);

    assert(encodedSuccessfully);

    // Send the encoded datagram.
    bool sentSuccessfully = publisher.send(encoded);

    assert(sentSuccessfully);

    std::cout << "Market event encoded and sent successfully!\n";

    // Receive the UDP datagram.
    std::array<std::uint8_t, market::protocol::MESSAGE_SIZE> received{};

    std::ptrdiff_t bytesReceived =
        receiver.receive(received.data(), received.size());

    assert(bytesReceived ==
        static_cast<std::ptrdiff_t>(received.size()));

    std::cout << "UDP datagram received successfully!\n";

    market::MarketEvent decoded{};

    bool decodedSuccessfully = market::protocol::decode(
        received.data(),
        static_cast<std::size_t>(bytesReceived),
        decoded
    );

    assert(decodedSuccessfully);

    std::cout << "UDP datagram decoded successfully!\n";


    assert(decoded.sequence == event.sequence);
    assert(decoded.type == event.type);
    assert(decoded.orderId == event.orderId);
    assert(decoded.relatedOrderId == event.relatedOrderId);
    assert(decoded.side == event.side);
    assert(decoded.priceTicks == event.priceTicks);
    assert(decoded.quantity == event.quantity);

    std::cout << "All event fields match!\n";

    return 0;
}
