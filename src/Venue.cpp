#include "Venue.h"
#include "Exceptions.h"

#include <iostream>
#include <iomanip>
#include <sstream>

Venue::Venue() : name("Nexus Arena") {
    totalSeats[TicketTier::VIP]      = 50;
    totalSeats[TicketTier::GENERAL]  = 200;
    totalSeats[TicketTier::STANDING] = 500;

    availableSeats[TicketTier::VIP]      = 50;
    availableSeats[TicketTier::GENERAL]  = 200;
    availableSeats[TicketTier::STANDING] = 500;

    prices[TicketTier::VIP]      = 5000.0;
    prices[TicketTier::GENERAL]  = 2000.0;
    prices[TicketTier::STANDING] =  800.0;

    seatOccupied[TicketTier::VIP]      .assign(50,  false);
    seatOccupied[TicketTier::GENERAL]  .assign(200, false);
    seatOccupied[TicketTier::STANDING] .assign(500, false);
}

bool Venue::hasAvailability(TicketTier tier, int count) const {
    auto it = availableSeats.find(tier);
    if (it == availableSeats.end()) return false;
    return it->second >= count;
}

std::vector<int> Venue::reserveSeats(TicketTier tier, int count) {
    if (!hasAvailability(tier, count)) {
        throw SoldOutException("Not enough seats available for tier: " + tierToString(tier));
    }

    std::vector<int> reserved;
    auto& occupied = seatOccupied.at(tier);
    for (int i = 0; i < static_cast<int>(occupied.size()) && static_cast<int>(reserved.size()) < count; ++i) {
        if (!occupied[i]) {
            occupied[i] = true;
            reserved.push_back(i + 1);  // 1-based
        }
    }
    availableSeats.at(tier) -= count;
    return reserved;
}

void Venue::releaseSeats(TicketTier tier, const std::vector<int>& seatNumbers) {
    auto& occupied = seatOccupied.at(tier);
    for (int seatNum : seatNumbers) {
        int idx = seatNum - 1;  // convert 1-based to 0-based
        if (idx >= 0 && idx < static_cast<int>(occupied.size())) {
            if (occupied[idx]) {
                occupied[idx] = false;
                availableSeats.at(tier)++;
            }
        }
    }
}

void Venue::displaySeatMap(TicketTier tier) const {
    const auto& occupied = seatOccupied.at(tier);
    int total = static_cast<int>(occupied.size());
    int seatsPerRow = 10;

    std::cout << "\n  Seat Map — " << tierToString(tier) << " (" << name << ")\n";
    std::cout << "  " << std::string(50, '-') << "\n";

    for (int i = 0; i < total; i += seatsPerRow) {
        int rowNum = i / seatsPerRow + 1;
        std::cout << "  Row " << std::setw(2) << rowNum << "  ";
        for (int j = i; j < i + seatsPerRow && j < total; ++j) {
            std::cout << (occupied[j] ? " X" : " O");
        }
        std::cout << "\n";
    }

    std::cout << "  " << std::string(50, '-') << "\n";
    std::cout << "  Legend:  O = Available   X = Booked\n\n";
}

void Venue::displayInventory() const {
    std::cout << "\n";
    std::cout << "  " << std::left << std::setw(12) << "Tier"
              << "| " << std::right << std::setw(5)  << "Total"
              << " | " << std::setw(6) << "Booked"
              << " | " << std::setw(9) << "Available"
              << " | Price (INR)\n";
    std::cout << "  " << std::string(58, '-') << "\n";

    auto printRow = [&](TicketTier t) {
        int total     = totalSeats.at(t);
        int available = availableSeats.at(t);
        int booked    = total - available;
        double price  = prices.at(t);

        std::ostringstream priceStr;
        priceStr << "\u20b9" << std::fixed << std::setprecision(2) << price;

        std::cout << "  " << std::left << std::setw(12) << tierToString(t)
                  << "| " << std::right << std::setw(5)  << total
                  << " | " << std::setw(6) << booked
                  << " | " << std::setw(9) << available
                  << " | " << priceStr.str() << "\n";
    };

    printRow(TicketTier::VIP);
    printRow(TicketTier::GENERAL);
    printRow(TicketTier::STANDING);
    std::cout << "\n";
}
