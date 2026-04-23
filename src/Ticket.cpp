#include "Ticket.h"
#include "Exceptions.h"

#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

std::string tierToString(TicketTier tier) {
    switch (tier) {
        case TicketTier::VIP:      return "VIP";
        case TicketTier::GENERAL:  return "GENERAL";
        case TicketTier::STANDING: return "STANDING";
    }
    return "UNKNOWN";
}

double tierToPrice(TicketTier tier) {
    switch (tier) {
        case TicketTier::VIP:      return 5000.0;
        case TicketTier::GENERAL:  return 2000.0;
        case TicketTier::STANDING: return  800.0;
    }
    return 0.0;
}

TicketTier stringToTier(const std::string& s) {
    if (s == "VIP")      return TicketTier::VIP;
    if (s == "GENERAL")  return TicketTier::GENERAL;
    if (s == "STANDING") return TicketTier::STANDING;
    throw InvalidBookingException("Unknown tier: " + s);
}

Ticket::Ticket(const std::string& ticketId_,
               const std::string& customerName_,
               TicketTier tier_,
               int seatNumber_,
               const std::string& bookingTime_,
               bool isGroupBooking_,
               int groupSize_)
    : ticketId(ticketId_)
    , customerName(customerName_)
    , tier(tier_)
    , seatNumber(seatNumber_)
    , bookingTime(bookingTime_)
    , isGroupBooking(isGroupBooking_)
    , groupSize(groupSize_)
{}

std::string Ticket::toString() const {
    std::ostringstream oss;
    oss << "  ┌─────────────────────────────────────┐\n";
    oss << "  │  Ticket ID   : " << ticketId << "\n";
    oss << "  │  Customer    : " << customerName << "\n";
    oss << "  │  Tier        : " << tierToString(tier) << "\n";
    oss << "  │  Seat No.    : " << seatNumber << "\n";
    oss << "  │  Price       : \u20b9" << std::fixed << std::setprecision(2) << tierToPrice(tier) << "\n";
    oss << "  │  Group       : " << (isGroupBooking ? "Yes (" + std::to_string(groupSize) + " seats)" : "No") << "\n";
    oss << "  │  Booked At   : " << bookingTime << "\n";
    oss << "  └─────────────────────────────────────┘\n";
    return oss.str();
}

std::string Ticket::generateId(int counter) {
    std::ostringstream oss;
    oss << "TKT-" << std::setw(5) << std::setfill('0') << counter;
    return oss.str();
}

std::string Ticket::currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
