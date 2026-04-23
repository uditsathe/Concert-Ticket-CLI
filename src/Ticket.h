#pragma once

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

enum class TicketTier {
    VIP,
    GENERAL,
    STANDING
};

std::string tierToString(TicketTier tier);
double tierToPrice(TicketTier tier);
TicketTier stringToTier(const std::string& s);

class Ticket {
public:
    std::string ticketId;
    std::string customerName;
    TicketTier tier;
    int seatNumber;
    std::string bookingTime;
    bool isGroupBooking;
    int groupSize;

    Ticket(const std::string& ticketId,
           const std::string& customerName,
           TicketTier tier,
           int seatNumber,
           const std::string& bookingTime,
           bool isGroupBooking,
           int groupSize);

    std::string toString() const;

    static std::string generateId(int counter);
    static std::string currentTimestamp();
};
