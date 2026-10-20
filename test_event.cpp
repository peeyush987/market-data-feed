#include "MarketEvent.h"
#include "OrderState.h"
#include "OrderTracker.h"

int main() {
    market::MarketEvent event;

    event.sequence = 1;
    event.type = market::EventType::Add;
    event.orderId = 101;
    event.priceTicks = 2005;
    event.quantity = 10;

    return 0;

    #include "OrderState.h"

    // Inside main():
    market::SimulatedOrder order;
    order.orderId = event.orderId;
    order.side = event.side;
    order.priceTicks = event.priceTicks;
    order.remainingQuantity = event.quantity;


    market::OrderTracker tracker;

    market::MarketEvent add;
    add.sequence = 1;
    add.type = market::EventType::Add;
    add.orderId = 101;
    add.side = market::Side::Buy;
    add.priceTicks = 2005;
    add.quantity = 10;

    if (!tracker.apply(add) ||
        !tracker.contains(101) ||
        tracker.activeOrderCount() != 1) {
        return 1;
    }

    market::MarketEvent cancel;
    cancel.sequence = 2;
    cancel.type = market::EventType::Cancel;
    cancel.orderId = 101;

    if (!tracker.apply(cancel) ||
        tracker.contains(101) ||
        tracker.activeOrderCount() != 0) {
        return 2;
    }

    return 0;
}