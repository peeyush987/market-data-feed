
#include "OrderTracker.h"

namespace market {

bool OrderTracker::apply(const MarketEvent& event) {
    // Reject events that are not next in the feed sequence.
    if (event.sequence != nextExpectedSequence_) {
        return false;
    }

    bool applied = false;

    switch (event.type) {
    case EventType::Add: {
        if (event.orderId == 0 ||
            event.priceTicks <= 0 ||
            event.quantity == 0) {
            break;
        }

        SimulatedOrder order;
        order.orderId = event.orderId;
        order.side = event.side;
        order.priceTicks = event.priceTicks;
        order.remainingQuantity = event.quantity;

        applied = activeOrders_.emplace(
            event.orderId, order
        ).second;
        break;
    }

    case EventType::Cancel: {
        if (event.orderId == 0) {
            break;
        }

        applied = activeOrders_.erase(event.orderId) == 1;
        break;
    }

    case EventType::Modify: {
        if (event.orderId == 0 ||
            event.priceTicks <= 0 ||
            event.quantity == 0) {
            break;
        }

        auto it = activeOrders_.find(event.orderId);

        if (it == activeOrders_.end()) {
            break;
        }

        it->second.remainingQuantity = event.quantity;
        applied = true;
        break;
    }

    case EventType::Trade: {
        if (event.orderId == 0 ||
            event.quantity == 0 ||
            event.priceTicks <= 0) {
            break;
        }

        auto restingIt = activeOrders_.find(event.orderId);

        if (restingIt == activeOrders_.end()) {
            break;
        }

        SimulatedOrder& resting = restingIt->second;

        if (event.side != resting.side) {
            break;
        }

        if (resting.side == Side::Buy &&
            event.priceTicks > resting.priceTicks) {
            break;
        }

        if (resting.side == Side::Sell &&
            event.priceTicks < resting.priceTicks) {
            break;
        }

        if (event.quantity > resting.remainingQuantity) {
            break;
        }

        resting.remainingQuantity -= event.quantity;

        if (resting.remainingQuantity == 0) {
            activeOrders_.erase(restingIt);
        }

        applied = true;
        break;
    }
    }

    // Consume the sequence only after a successful state update.
    if (applied) {
        ++nextExpectedSequence_;
    }

    return applied;
}


bool OrderTracker::contains(std::uint64_t orderId) const {
    return activeOrders_.find(orderId) != activeOrders_.end();
}

std::size_t OrderTracker::activeOrderCount() const {
    return activeOrders_.size();
}

} // namespace market
