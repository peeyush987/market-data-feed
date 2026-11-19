# Market Data Feed Handler

A C++17 market-data feed handler project implementing a fixed-size binary message protocol, UDP transport, message decoding, and order-state tracking.

The current end-to-end pipeline sends a market event through a UDP publisher, receives it through a UDP receiver, decodes the binary message, and verifies that the decoded fields match the original event.

## Architecture

```text
MarketEvent
    |
    v
FeedProtocol::encode()
    |
    v
UdpPublisher
    |
    | UDP datagram
    v
UdpReceiver
    |
    v
FeedProtocol::decode()
    |
    v
MarketEvent
```

`OrderTracker` maintains simulated active-order state and validates incoming events, including sequence numbers.

## Binary Feed Protocol

Every message has a fixed size of **38 bytes**. Multi-byte integer fields use big-endian byte order.

| Field | Offset | Size | Description |
|---|---:|---:|---|
| Sequence | 0 | 8 bytes | Message sequence number |
| Event type | 8 | 1 byte | ADD, CANCEL, MODIFY, or TRADE |
| Side | 9 | 1 byte | BUY or SELL |
| Order ID | 10 | 8 bytes | Primary order identifier |
| Related order ID | 18 | 8 bytes | Related order identifier |
| Price ticks | 26 | 8 bytes | Price represented in integer ticks |
| Quantity | 34 | 4 bytes | Order quantity |
| **Total** | | **38 bytes** | |

Wire event codes:

| Event | Code |
|---|---:|
| ADD | 1 |
| CANCEL | 2 |
| MODIFY | 3 |
| TRADE | 4 |

Wire side codes:

| Side | Code |
|---|---:|
| BUY | 1 |
| SELL | 2 |

The protocol implementation validates message length, field values, and encoding/decoding bounds. It does not serialize C++ structs directly; fields are explicitly encoded and decoded.

## UDP Transport

- Uses IPv4 UDP sockets.
- The receiver binds to `127.0.0.1:9000`.
- The publisher sends datagrams to `127.0.0.1:9000`.
- The receiver uses `recvmsg()` and checks for truncated datagrams.
- The publisher uses `sendto()` to transmit encoded messages.

The current transport test runs over the local loopback interface. It does not yet implement application-level missing-message detection and recovery in the UDP pipeline.

## Build and Run the End-to-End Test

The end-to-end test can currently be compiled directly with a C++17 compiler:

```bash
c++ -std=c++17 -Wall -Wextra -Wpedantic \
-Iinclude tests/test_udp_pipeline.cpp \
src/UdpPublisher.cpp \
src/UdpReceiver.cpp \
src/FeedProtocol.cpp \
-o test_udp_pipeline
```

Run it:

```bash
./test_udp_pipeline
```

The test verifies that an event can be encoded, sent over UDP, received, decoded, and compared field by field against the original event.

## Current Implementation

- Fixed 38-byte binary message format.
- Big-endian integer serialization and deserialization.
- Validation of encoded and decoded messages.
- UDP publisher using `sendto()`.
- UDP receiver using `recvmsg()`.
- Detection of truncated UDP datagrams.
- Order-state tracking with sequence validation.
- Unit tests for protocol, order tracking, and UDP receiving.
- Single-event end-to-end UDP pipeline test.

## Next Development Steps

- Multi-event integration through the publisher, receiver, decoder, and order tracker.
- End-to-end sequence-gap and duplicate-message handling, including recovery behavior.
- Missing-message recovery and replay.
- Recorded feed replay.
- Throughput and latency measurement.
- CMake build configuration, automated CI testing, and Docker support.
