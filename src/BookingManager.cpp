#include "BookingManager.h"
#include "Exceptions.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <filesystem>

BookingManager::BookingManager()
    : bookingCounter(0)
    , csvFilePath("data/bookings.csv")
{
    loadFromCSV();
}

std::vector<Ticket> BookingManager::bookTickets(const BookingRequest& req) {
    std::lock_guard<std::mutex> lock(bookingMutex);

    if (req.groupSize < 1 || req.groupSize > 10) {
        throw InvalidBookingException("Group size must be between 1 and 10.");
    }
    if (req.customerName.empty()) {
        throw InvalidBookingException("Customer name must not be empty.");
    }

    if (!venue.hasAvailability(req.tier, req.groupSize)) {
        int tierKey = static_cast<int>(req.tier);
        int position = static_cast<int>(waitlists[tierKey].size()) + 1;
        WaitlistEntry entry;
        entry.request          = req;
        entry.enqueuedAt       = std::chrono::system_clock::now();
        entry.waitlistPosition = position;
        waitlists[tierKey].push(entry);
        std::cout << "Added to waitlist at position " << position
                  << " for " << tierToString(req.tier) << " tier.\n";
        return {};
    }

    std::vector<int> seats = venue.reserveSeats(req.tier, req.groupSize);
    bool isGroup = (req.groupSize > 1);
    std::string timestamp = Ticket::currentTimestamp();
    std::vector<Ticket> tickets;

    for (int seat : seats) {
        ++bookingCounter;
        std::string id = Ticket::generateId(bookingCounter);
        Ticket t(id, req.customerName, req.tier, seat, timestamp, isGroup, req.groupSize);
        confirmedBookings.emplace(id, t);
        appendToCSV(t);
        tickets.push_back(t);
    }

    return tickets;
}

bool BookingManager::cancelBooking(const std::string& ticketId) {
    std::lock_guard<std::mutex> lock(bookingMutex);

    auto it = confirmedBookings.find(ticketId);
    if (it == confirmedBookings.end()) {
        return false;
    }

    Ticket copy = it->second;
    venue.releaseSeats(copy.tier, {copy.seatNumber});
    confirmedBookings.erase(it);
    rewriteCSV();
    processWaitlist(copy.tier);
    return true;
}

void BookingManager::lookupBooking(const std::string& ticketId) const {
    auto it = confirmedBookings.find(ticketId);
    if (it == confirmedBookings.end()) {
        std::cout << "Booking not found: " << ticketId << "\n";
    } else {
        std::cout << it->second.toString();
    }
}

void BookingManager::displayAllBookings() const {
    if (confirmedBookings.empty()) {
        std::cout << "\n  No bookings found. Start booking tickets!\n\n";
        return;
    }

    std::vector<std::string> ids;
    ids.reserve(confirmedBookings.size());
    for (const auto& kv : confirmedBookings) {
        ids.push_back(kv.first);
    }
    std::sort(ids.begin(), ids.end());

    std::cout << "\n";
    std::cout << "  " << std::left
              << std::setw(12) << "Ticket ID"
              << "| " << std::setw(20) << "Customer"
              << "| " << std::setw(9)  << "Tier"
              << "| " << std::setw(5)  << "Seat"
              << "| " << std::setw(6)  << "Group"
              << "| Time\n";
    std::cout << "  " << std::string(72, '-') << "\n";

    for (const auto& id : ids) {
        const Ticket& t = confirmedBookings.at(id);
        std::cout << "  " << std::left
                  << std::setw(12) << t.ticketId
                  << "| " << std::setw(20) << t.customerName
                  << "| " << std::setw(9)  << tierToString(t.tier)
                  << "| " << std::right << std::setw(4) << t.seatNumber << " "
                  << "| " << std::setw(6) << (t.isGroupBooking ? "Yes" : "No")
                  << "| " << t.bookingTime << "\n";
    }
    std::cout << "\n";
}

int BookingManager::getWaitlistSize(TicketTier tier) const {
    int key = static_cast<int>(tier);
    auto it = waitlists.find(key);
    if (it == waitlists.end()) return 0;
    return static_cast<int>(it->second.size());
}

void BookingManager::processWaitlist(TicketTier tier) {
    int key = static_cast<int>(tier);
    auto& q = waitlists[key];

    while (!q.empty()) {
        const WaitlistEntry& front = q.front();
        if (!venue.hasAvailability(tier, front.request.groupSize)) {
            break;
        }

        BookingRequest req = front.request;
        q.pop();

        std::vector<int> seats = venue.reserveSeats(req.tier, req.groupSize);
        bool isGroup = (req.groupSize > 1);
        std::string timestamp = Ticket::currentTimestamp();
        std::vector<std::string> newIds;

        for (int seat : seats) {
            ++bookingCounter;
            std::string id = Ticket::generateId(bookingCounter);
            Ticket t(id, req.customerName, req.tier, seat, timestamp, isGroup, req.groupSize);
            confirmedBookings.emplace(id, t);
            appendToCSV(t);
            newIds.push_back(id);
        }

        std::string idList;
        for (size_t i = 0; i < newIds.size(); ++i) {
            if (i > 0) idList += ", ";
            idList += newIds[i];
        }

        std::cout << "\U0001F389 Waitlist notification: " << req.customerName
                  << "'s booking for " << req.groupSize
                  << " x " << tierToString(req.tier)
                  << " seat(s) has been confirmed! Ticket(s): " << idList << "\n";
    }
}

void BookingManager::appendToCSV(const Ticket& t) {
    std::filesystem::create_directories("data");
    std::ofstream file(csvFilePath, std::ios::app);
    if (!file) return;
    file << t.ticketId << ","
         << t.customerName << ","
         << tierToString(t.tier) << ","
         << t.seatNumber << ","
         << t.bookingTime << ","
         << (t.isGroupBooking ? "1" : "0") << ","
         << t.groupSize << "\n";
}

void BookingManager::rewriteCSV() {
    std::filesystem::create_directories("data");
    std::ofstream file(csvFilePath, std::ios::trunc);
    if (!file) return;
    for (const auto& kv : confirmedBookings) {
        const Ticket& t = kv.second;
        file << t.ticketId << ","
             << t.customerName << ","
             << tierToString(t.tier) << ","
             << t.seatNumber << ","
             << t.bookingTime << ","
             << (t.isGroupBooking ? "1" : "0") << ","
             << t.groupSize << "\n";
    }
}

void BookingManager::loadFromCSV() {
    std::ifstream file(csvFilePath);
    if (!file) return;

    std::string line;
    int maxCounter = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> fields;
        std::istringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            fields.push_back(token);
        }

        if (fields.size() < 7) {
            std::cerr << "Warning: skipping malformed CSV line: " << line << "\n";
            continue;
        }

        try {
            std::string ticketId    = fields[0];
            std::string customer    = fields[1];
            TicketTier tier         = stringToTier(fields[2]);
            int seatNumber          = std::stoi(fields[3]);
            std::string bookingTime = fields[4];
            bool isGroup            = (fields[5] == "1");
            int groupSize           = std::stoi(fields[6]);

            Ticket t(ticketId, customer, tier, seatNumber, bookingTime, isGroup, groupSize);
            confirmedBookings.emplace(ticketId, t);

            // Restore seat occupancy directly (1-based → 0-based index)
            int idx = seatNumber - 1;
            auto& occupied = venue.seatOccupied.at(tier);
            if (idx >= 0 && idx < static_cast<int>(occupied.size()) && !occupied[idx]) {
                occupied[idx] = true;
                venue.availableSeats.at(tier)--;
            }

            // Track highest ticket number to restore bookingCounter
            if (ticketId.size() > 4 && ticketId.substr(0, 4) == "TKT-") {
                try {
                    int num = std::stoi(ticketId.substr(4));
                    if (num > maxCounter) maxCounter = num;
                } catch (...) {}
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: skipping malformed CSV line (" << e.what() << "): " << line << "\n";
        }
    }

    bookingCounter = maxCounter;
}
