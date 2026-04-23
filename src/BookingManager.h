#pragma once

#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <mutex>

#include "Ticket.h"
#include "Booking.h"
#include "Venue.h"

class BookingManager {
public:
    Venue venue;

    BookingManager();

    std::vector<Ticket> bookTickets(const BookingRequest& req);
    bool cancelBooking(const std::string& ticketId);
    void lookupBooking(const std::string& ticketId) const;
    void displayAllBookings() const;
    int getWaitlistSize(TicketTier tier) const;

private:
    std::unordered_map<std::string, Ticket> confirmedBookings;
    std::unordered_map<int, std::queue<WaitlistEntry>> waitlists;
    int bookingCounter;
    mutable std::mutex bookingMutex;
    std::string csvFilePath;

    void processWaitlist(TicketTier tier);
    void appendToCSV(const Ticket& t);
    void rewriteCSV();
    void loadFromCSV();
};
