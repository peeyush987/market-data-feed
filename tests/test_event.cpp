#include "MarketEvent.h"
#include "OrderTracker.h"

#include <iostream>

int main() {
    market::OrderTracker tracker;

    // Test 1: Add an order with quantity 10.
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
        std::cerr << "Test 1 failed: ADD\n";
        return 1;
    }

    // Test 2: Modify the remaining quantity from 10 to 7.
    market::MarketEvent modify;
    modify.sequence = 2;
    modify.type = market::EventType::Modify;
    modify.orderId = 101;
    modify.side = market::Side::Buy;
    modify.priceTicks = 2005;
    modify.quantity = 7;

    if (!tracker.apply(modify) ||
        !tracker.contains(101) ||
        tracker.activeOrderCount() != 1) {
        std::cerr << "Test 2 failed: MODIFY\n";
        return 2;
    }

    // Test 3: Attempt to trade 8 units when only 7 remain.
    // The event must be rejected, leaving the order unchanged.
    market::MarketEvent oversizedTrade;
    oversizedTrade.sequence = 3;
    oversizedTrade.type = market::EventType::Trade;
    oversizedTrade.orderId = 101;
    oversizedTrade.side = market::Side::Buy;
    oversizedTrade.priceTicks = 1990;
    oversizedTrade.quantity = 8;

    if (tracker.apply(oversizedTrade) ||
        !tracker.contains(101) ||
        tracker.activeOrderCount() != 1) {
        std::cerr << "Test 3 failed: oversized TRADE\n";
        return 3;
    }

    // Test 4: Trade all 7 remaining units.
    // The order should be removed after the trade.
    market::MarketEvent finalTrade;
    finalTrade.sequence = 3;
    finalTrade.type = market::EventType::Trade;
    finalTrade.orderId = 101;
    finalTrade.side = market::Side::Buy;
    finalTrade.priceTicks = 1990;
    finalTrade.quantity = 7;

    if (!tracker.apply(finalTrade) ||
        tracker.contains(101) ||
        tracker.activeOrderCount() != 0) {
        std::cerr << "Test 4 failed: final TRADE\n";
        return 4;
    }


    // Test 5: A sequence gap must be rejected.
    market::MarketEvent gap;
    gap.sequence = 6; // The tracker expects sequence 5.
    gap.type = market::EventType::Add;
    gap.orderId = 303;
    gap.side = market::Side::Buy;
    gap.priceTicks = 2000;
    gap.quantity = 4;

    if (tracker.apply(gap) ||
        tracker.contains(303) ||
        tracker.activeOrderCount() != 0) {
        std::cerr << "Test 5 failed: sequence gap\n";
        return 5;
    }

    // Test 6: The correct sequence can still be accepted.
    market::MarketEvent validAdd;
    validAdd.sequence = 4;
    validAdd.type = market::EventType::Add;
    validAdd.orderId = 303;
    validAdd.side = market::Side::Buy;
    validAdd.priceTicks = 2000;
    validAdd.quantity = 4;

    if (!tracker.apply(validAdd) ||
        !tracker.contains(303)) {
        std::cerr << "Test 6 failed: retry expected sequence\n";
        return 6;
    }

    // Test 7: A duplicate sequence must be rejected.
    market::MarketEvent duplicate = validAdd;

    if (tracker.apply(duplicate) ||
        !tracker.contains(303) ||
        tracker.activeOrderCount() != 1) {
        std::cerr << "Test 7 failed: duplicate sequence\n";
        return 7;
    }

    // Test 8: An invalid event must not consume sequence 6.
    market::MarketEvent invalidCancel;
    invalidCancel.sequence = 5;
    invalidCancel.type = market::EventType::Cancel;
    invalidCancel.orderId = 999;

    if (tracker.apply(invalidCancel) ||
        !tracker.contains(303)) {
        std::cerr << "Test 8 failed: invalid event changed state\n";
        return 8;
    }

    // Test 9: A valid event can reuse sequence 6.
    market::MarketEvent validCancel;
    validCancel.sequence = 5;
    validCancel.type = market::EventType::Cancel;
    validCancel.orderId = 303;

    if (!tracker.apply(validCancel) ||
        tracker.contains(303) ||
        tracker.activeOrderCount() != 0) {
        std::cerr << "Test 9 failed: valid retry after rejection\n";
        return 9;
    }

    


    std::cout << "All OrderTracker tests passed.\n";
    return 0;
}