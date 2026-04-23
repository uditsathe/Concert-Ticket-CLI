#include <iostream>
#include <string>
#include <limits>
#include <filesystem>
#include <regex>

#include "BookingManager.h"
#include "Exceptions.h"

static const std::string RED   = "\033[31m";
static const std::string GREEN = "\033[32m";
static const std::string RESET = "\033[0m";

void printBanner() {
    std::cout << "\n";
    std::cout << "" << std::string(46, '-') << "\n";
    std::cout << "         Coldplay Ahmedabad Narendra Modi Stadium Tickets Live!        \n";
    std::cout << "         BuyConcertTix         \n";
    std::cout << "" << std::string(46, '-') << "\n";
}

void printMenu() {
    std::cout << "--------------------------------------------------------Main Menu--------------------------------------------------------\n";
    std::cout << "  1. Book Ticket(s)\n";
    std::cout << "  2. Cancel Booking\n";
    std::cout << "  3. Look Up Booking\n";
    std::cout << "  4. View All Bookings\n";
    std::cout << "  5. View Seat Map\n";
    std::cout << "  6. View Inventory\n";
    std::cout << "  7. Exit\n";
    std::cout << "-------------------------------------------------------------------------------------------------------------------------\n";
    std::cout << "Enter choice: ";
}

TicketTier selectTier(bool showPrices) {
    while (true) {
        if (showPrices) {
            std::cout << "Select tier — 1. VIP (12000)  2. GENERAL (4500)  3. STANDING (6000): ";
        } else {
            std::cout << "Select tier — 1. VIP  2. GENERAL  3. STANDING: ";
        }
        int choice;
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > 3) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid selection. Please enter 1, 2, or 3.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
            case 1: return TicketTier::VIP;
            case 2: return TicketTier::GENERAL;
            case 3: return TicketTier::STANDING;
        }
    }
}

void handleBooking(BookingManager& mgr) {
    std::cout << "Enter your name: ";
    std::string name;
    std::getline(std::cin, name);

    while (name.empty()) {
        std::cout << "Name cannot be empty. Enter your name: ";
        std::getline(std::cin, name);
    }

    TicketTier tier = selectTier(true);

    int groupSize = 0;
    while (true) {
        std::cout << "Number of tickets (1-10, enter >1 for group booking): ";
        std::cin >> groupSize;
        if (std::cin.fail() || groupSize < 1 || groupSize > 10) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid. Please enter a number between 1 and 10.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
    }

    BookingRequest req;
    req.customerName = name;
    req.tier         = tier;
    req.groupSize    = groupSize;

    std::vector<Ticket> tickets = mgr.bookTickets(req);

    if (!tickets.empty()) {
        std::cout << GREEN << " Booking Confirmed!" << RESET << "\n";
        for (const auto& t : tickets) {
            std::cout << t.toString();
        }
    } else {
        int pos = mgr.getWaitlistSize(tier);
        std::cout << " You are on the waitlist at position " << pos << ".\n";
    }
}

void handleCancel(BookingManager& mgr) {
    std::cout << "Enter Ticket ID to cancel (e.g. TKT-00001): ";
    std::string id;
    std::getline(std::cin, id);

    std::regex pattern("^TKT-\\d+$");
    if (!std::regex_match(id, pattern)) {
        std::cout << RED << "Invalid Ticket ID format. Expected format: TKT-XXXXX" << RESET << "\n";
        return;
    }

    bool result = mgr.cancelBooking(id);
    if (result) {
        std::cout << GREEN << "\= Booking " << id << " cancelled successfully." << RESET << "\n";
    } else {
        std::cout << RED << "Booking not found: " << id << RESET << "\n";
    }
}

void handleLookup(BookingManager& mgr) {
    std::cout << "Enter Ticket ID to look up: ";
    std::string id;
    std::getline(std::cin, id);
    mgr.lookupBooking(id);
}

void handleSeatMap(BookingManager& mgr) {
    TicketTier tier = selectTier(false);
    mgr.venue.displaySeatMap(tier);
}

int main() {
    std::filesystem::create_directories("data");

    BookingManager bookingManager;

    printBanner();
    bookingManager.venue.displayInventory();

    bool running = true;
    while (running) {
        try {
            printMenu();

            int choice;
            std::cin >> choice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << RED << "Invalid input. Please enter a number." << RESET << "\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (choice) {
                case 1:
                    handleBooking(bookingManager);
                    break;
                case 2:
                    handleCancel(bookingManager);
                    break;
                case 3:
                    handleLookup(bookingManager);
                    break;
                case 4:
                    bookingManager.displayAllBookings();
                    break;
                case 5:
                    handleSeatMap(bookingManager);
                    break;
                case 6:
                    bookingManager.venue.displayInventory();
                    break;
                case 7:
                    std::cout << "\nThank you for using BuyConcertTix Booking System. Goodbye! \n\n";
                    running = false;
                    break;
                default:
                    std::cout << RED << "Invalid choice. Please select 1-7." << RESET << "\n";
                    break;
            }
        } catch (const SoldOutException& e) {
            std::cout << RED << "[Sold Out] " << e.what() << RESET << "\n";
        } catch (const InvalidBookingException& e) {
            std::cout << RED << "[Invalid Booking] " << e.what() << RESET << "\n";
        } catch (const BookingNotFoundException& e) {
            std::cout << RED << "[Not Found] " << e.what() << RESET << "\n";
        } catch (const std::exception& e) {
            std::cout << RED << "[Error] " << e.what() << RESET << "\n";
        }
    }

    return 0;
}
