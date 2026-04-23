#pragma once

#include <string>
#include <chrono>
#include "Ticket.h"

struct BookingRequest {
    std::string customerName;
    TicketTier tier;
    int groupSize;
};

struct WaitlistEntry {
    BookingRequest request;
    std::chrono::system_clock::time_point enqueuedAt;
    int waitlistPosition;
};
