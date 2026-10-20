
#include "OrderTracker.h"

namespace market {

bool OrderTracker::apply(const MarketEvent& event) {
    switch (event.type) {
    case EventType::Add: {
        // Reject invalid IDs, prices, and quantities.
        if (event.orderId == 0 ||
            event.priceTicks <= 0 ||
            event.quantity == 0) {
            return false;
        }

        SimulatedOrder order;
        order.orderId = event.orderId;
        order.side = event.side;
        order.priceTicks = event.priceTicks;
        order.remainingQuantity = event.quantity;

        // emplace does not replace an existing order.
        return activeOrders_.emplace(
            event.orderId, order
        ).second;
    }

    case EventType::Cancel: {
        if (event.orderId == 0) {
            return false;
        }

        // erase returns 1 if an order was removed, 0 otherwise.
        return activeOrders_.erase(event.orderId) == 1;
    }

    case EventType::Modify: {
        if (event.orderId == 0 ||
            event.priceTicks <= 0 ||
            event.quantity == 0) {
            return false;
        }

        auto it = activeOrders_.find(event.orderId);

        if (it == activeOrders_.end()) {
            return false;
        }

        // Update only after validation succeeds.
        it->second.priceTicks = event.priceTicks;
        it->second.remainingQuantity = event.quantity;
        it->second.side = event.side;

        return true;
    }

    case EventType::Trade: {
        if (event.orderId == 0 ||
            event.relatedOrderId == 0 ||
            event.orderId == event.relatedOrderId ||
            event.quantity == 0) {
            return false;
        }

        auto restingIt = activeOrders_.find(event.orderId);
        auto incomingIt =
            activeOrders_.find(event.relatedOrderId);

        if (restingIt == activeOrders_.end() ||
            incomingIt == activeOrders_.end()) {
            return false;
        }

        SimulatedOrder& resting = restingIt->second;
        SimulatedOrder& incoming = incomingIt->second;

        // A trade must involve opposite sides.
        if (resting.side == incoming.side) {
            return false;
        }

        // Neither order may trade more than its remaining quantity.
        if (event.quantity > resting.remainingQuantity ||
            event.quantity > incoming.remainingQuantity) {
            return false;
        }

        // Both orders have been validated; now apply the trade.
        resting.remainingQuantity -= event.quantity;
        incoming.remainingQuantity -= event.quantity;

        // Remove fully filled orders.
        if (resting.remainingQuantity == 0) {
            activeOrders_.erase(restingIt);
        }

        if (incoming.remainingQuantity == 0) {
            activeOrders_.erase(incomingIt);
        }

        return true;
    }
    }

    // Defensive fallback for an invalid EventType value.
    return false;
}

bool OrderTracker::contains(std::uint64_t orderId) const {
    return activeOrders_.find(orderId) != activeOrders_.end();
}

std::size_t OrderTracker::activeOrderCount() const {
    return activeOrders_.size();
}

} // namespace market
