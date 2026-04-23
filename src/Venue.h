#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Ticket.h"

class Venue {
public:
    std::string name;
    std::unordered_map<TicketTier, int> totalSeats;
    std::unordered_map<TicketTier, int> availableSeats;
    std::unordered_map<TicketTier, double> prices;
    std::unordered_map<TicketTier, std::vector<bool>> seatOccupied;

    Venue();

    bool hasAvailability(TicketTier tier, int count) const;
    std::vector<int> reserveSeats(TicketTier tier, int count);
    void releaseSeats(TicketTier tier, const std::vector<int>& seatNumbers);
    void displaySeatMap(TicketTier tier) const;
    void displayInventory() const;
};
