# Concert Ticket Booking System — Cursor Execution Plan

> **Instructions for Cursor:** Read this entire file and execute all steps in order. Create every file listed. Do not ask clarifying questions. Do not leave any TODOs or placeholders. Produce a fully working, compilable C++17 CLI application.

---

## Project Overview

| Field | Value |
|---|---|
| Project Name | Concert Ticket Booking System |
| Language | C++17 |
| Build System | CMake 3.16+ |
| External Dependencies | **None** — C++ Standard Library only |
| Target Platforms | Linux, macOS, Windows (MinGW) |

---

## STEP 1 — Create Project Structure

Create the following directory and file layout:

```
concert-booking/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── Exceptions.h
│   ├── Ticket.h
│   ├── Ticket.cpp
│   ├── Booking.h
│   ├── Booking.cpp
│   ├── Venue.h
│   ├── Venue.cpp
│   ├── BookingManager.h
│   └── BookingManager.cpp
└── data/
    └── .gitkeep
```

---

## STEP 2 — CMakeLists.txt

Create `CMakeLists.txt` at the project root with the following:

- `cmake_minimum_required`: 3.16
- Project name: `ConcertBooking`
- C++ standard: 17, required: true
- Add executable `concert_booking` from all `.cpp` files in `src/`
- On Linux/GCC, link `stdc++fs` for `std::filesystem` support on older GCC versions
- Set executable output directory to the project root

---

## STEP 3 — src/Exceptions.h

Define three custom exception classes. Use `#pragma once`.

```
- class SoldOutException       : public std::runtime_error
- class InvalidBookingException : public std::runtime_error
- class BookingNotFoundException : public std::runtime_error
```

Each takes a `std::string message` in its constructor passed to `std::runtime_error`.

---

## STEP 4 — src/Ticket.h and src/Ticket.cpp

Use `#pragma once` on the header.

**TicketTier enum class:**
- VIP
- GENERAL
- STANDING

**Free functions:**
- `std::string tierToString(TicketTier tier)` → returns "VIP", "GENERAL", or "STANDING"
- `double tierToPrice(TicketTier tier)` → returns 5000.0, 2000.0, or 800.0
- `TicketTier stringToTier(const std::string& s)` → parses string to enum, throws `InvalidBookingException` on unknown value

**Ticket class fields (all public for simplicity):**
- `std::string ticketId`
- `std::string customerName`
- `TicketTier tier`
- `int seatNumber`
- `std::string bookingTime`   ← formatted as "YYYY-MM-DD HH:MM:SS"
- `bool isGroupBooking`
- `int groupSize`             ← 1 for individual

**Ticket class methods:**
- Constructor taking all fields
- `std::string toString() const` → returns a neatly formatted multi-line string of all ticket details including price
- `static std::string generateId(int counter)` → returns `"TKT-"` + zero-padded 5-digit number (e.g. `"TKT-00042"`)
- `static std::string currentTimestamp()` → returns current local time as `"YYYY-MM-DD HH:MM:SS"` using `<chrono>` and `<iomanip>`

---

## STEP 5 — src/Booking.h and src/Booking.cpp

Use `#pragma once` on the header. Include `<chrono>`.

**BookingRequest struct:**
- `std::string customerName`
- `TicketTier tier`
- `int groupSize`   ← 1 for individual, 2–10 for group

**WaitlistEntry struct:**
- `BookingRequest request`
- `std::chrono::system_clock::time_point enqueuedAt`
- `int waitlistPosition`

No implementation needed in the .cpp beyond includes.

---

## STEP 6 — src/Venue.h and src/Venue.cpp

Use `#pragma once` on the header.

**Venue class:**

Fields:
- `std::string name`  ← "Nexus Arena"
- `std::unordered_map<TicketTier, int> totalSeats`      ← VIP: 50, GENERAL: 200, STANDING: 500
- `std::unordered_map<TicketTier, int> availableSeats`  ← same initial values
- `std::unordered_map<TicketTier, double> prices`       ← VIP: 5000, GENERAL: 2000, STANDING: 800
- `std::unordered_map<TicketTier, std::vector<bool>> seatOccupied` ← indexed by seat number (0-based), true = occupied

Constructor:
- Initializes all maps and resizes seatOccupied vectors to match totalSeats per tier (all false initially)

Methods:

1. `bool hasAvailability(TicketTier tier, int count) const`
   - Returns true if the tier has at least `count` free seats available

2. `std::vector<int> reserveSeats(TicketTier tier, int count)`
   - Finds `count` free seats (first-fit scan of seatOccupied vector)
   - Marks them as occupied, decrements availableSeats
   - Returns the 1-based seat numbers reserved
   - Throws `SoldOutException` if not enough seats

3. `void releaseSeats(TicketTier tier, const std::vector<int>& seatNumbers)`
   - Marks those seats as unoccupied (convert from 1-based to 0-based index)
   - Increments availableSeats by count

4. `void displaySeatMap(TicketTier tier) const`
   - Prints an ASCII grid: `O` = available, `X` = booked
   - 10 seats per row, with row numbers on the left
   - Print a legend below the grid

5. `void displayInventory() const`
   - Prints a formatted table:
     ```
     Tier        | Total | Booked | Available | Price (INR)
     ----------------------------------------------------------
     VIP         |    50 |      0 |        50 | ₹5,000.00
     GENERAL     |   200 |      0 |       200 | ₹2,000.00
     STANDING    |   500 |      0 |       500 |   ₹800.00
     ```

---

## STEP 7 — src/BookingManager.h and src/BookingManager.cpp

Use `#pragma once` on the header. This is the core engine.

**BookingManager class:**

Fields:
- `Venue venue`
- `std::unordered_map<std::string, Ticket> confirmedBookings`  ← ticketId → Ticket
- `std::unordered_map<int, std::queue<WaitlistEntry>> waitlists`  ← use `static_cast<int>(TicketTier)` as key
- `int bookingCounter`   ← starts at 0, increments per ticket (not per booking request)
- `std::mutex bookingMutex`
- `std::string csvFilePath`  ← `"data/bookings.csv"`

**Public Methods:**

1. `BookingManager()`
   - Initializes venue, bookingCounter = 0, csvFilePath
   - Calls `loadFromCSV()` on construction

2. `std::vector<Ticket> bookTickets(const BookingRequest& req)`
   - Acquires `std::lock_guard` on `bookingMutex`
   - Validates: groupSize must be between 1 and 10 inclusive; throws `InvalidBookingException` otherwise
   - Validates: customerName must be non-empty; throws `InvalidBookingException` otherwise
   - Checks `venue.hasAvailability(req.tier, req.groupSize)`
   - **If available (GROUP ATOMICITY RULE):** Reserve ALL seats at once via `venue.reserveSeats(req.tier, req.groupSize)`. Create one `Ticket` object per seat. All tickets in the group share the same customer name, tier, isGroupBooking=true (false if groupSize==1), and groupSize. Each gets a unique ticketId. Add all to `confirmedBookings`. Append each to CSV. Return the vector of tickets.
   - **If NOT available:** Create a `WaitlistEntry`, compute waitlist position as current queue size + 1, push to the appropriate waitlist queue. Print: `"Added to waitlist at position X for [TIER] tier."` Return empty vector.

3. `bool cancelBooking(const std::string& ticketId)`
   - Acquires `std::lock_guard` on `bookingMutex`
   - Finds ticket in `confirmedBookings`; returns false if not found
   - Calls `venue.releaseSeats()` for the ticket's seat
   - Removes from `confirmedBookings`
   - Calls `rewriteCSV()`
   - Calls `processWaitlist(ticket.tier)` to check if anyone on the waitlist can now be served
   - Returns true

4. `void lookupBooking(const std::string& ticketId) const`
   - Prints ticket details via `ticket.toString()` or prints `"Booking not found: [ticketId]"`

5. `void displayAllBookings() const`
   - If no bookings, print a friendly empty state message
   - Otherwise print a formatted table of all confirmed bookings sorted by ticketId:
     ```
     Ticket ID   | Customer           | Tier     | Seat | Group | Time
     -----------------------------------------------------------------------
     TKT-00001   | Alice              | VIP      |    3 |   Yes | 2026-04-24 00:15:00
     ```

6. `int getWaitlistSize(TicketTier tier) const`
   - Returns current size of the waitlist queue for that tier

**Private Methods:**

7. `void processWaitlist(TicketTier tier)`
   - Peeks at the front of the waitlist queue for this tier
   - If queue is non-empty AND `venue.hasAvailability(tier, front.groupSize)`:
     - Pop the entry
     - Call `bookTickets()` logic directly (without re-locking mutex — already locked) to confirm their booking
     - Print: `"🎉 Waitlist notification: [customerName]'s booking for [groupSize] x [TIER] seat(s) has been confirmed! Ticket(s): [IDs]"`
     - Recurse to process next waitlist entry if seats still available

8. `void appendToCSV(const Ticket& t)`
   - Appends one line to CSV:
     `ticketId,customerName,tier,seatNumber,bookingTime,isGroupBooking,groupSize`

9. `void rewriteCSV()`
   - Rewrites the entire CSV from `confirmedBookings` map

10. `void loadFromCSV()`
    - If CSV file doesn't exist, return early
    - Parse each line, reconstruct `Ticket` objects, insert into `confirmedBookings`
    - Set `bookingCounter` to the highest ticket number found + 1
    - Restore seat occupancy in `venue` by calling `venue.reserveSeats()` for each loaded ticket
    - Handle malformed lines gracefully (skip with a warning)

---

## STEP 8 — src/main.cpp

**On startup:**
- Use `std::filesystem::create_directories("data")` to ensure the data folder exists
- Instantiate `BookingManager bookingManager`
- Print a welcome banner:

```
╔══════════════════════════════════════════════╗
║       🎵  NEXUS ARENA TICKET BOOKING  🎵      ║
║         Concert Ticket Booking System         ║
╚══════════════════════════════════════════════╝
```

- Print current inventory via `bookingManager.venue.displayInventory()`

**Main menu loop** — runs until user selects Exit:

```
════════════════════ MAIN MENU ════════════════════
  1. Book Ticket(s)
  2. Cancel Booking
  3. Look Up Booking
  4. View All Bookings
  5. View Seat Map
  6. View Inventory
  7. Exit
═══════════════════════════════════════════════════
```

**Option 1 — Book Ticket(s):**
- Prompt: `"Enter your name: "`
- Prompt: `"Select tier — 1. VIP (₹5000)  2. GENERAL (₹2000)  3. STANDING (₹800): "`
- Prompt: `"Number of tickets (1-10, enter >1 for group booking): "`
- Validate all inputs; re-prompt on invalid
- Call `bookingManager.bookTickets(req)`
- If confirmed: print `"✅ Booking Confirmed!"` followed by each ticket's `toString()`
- If waitlisted: print `"⏳ You are on the waitlist at position X."`

**Option 2 — Cancel Booking:**
- Prompt: `"Enter Ticket ID to cancel (e.g. TKT-00001): "`
- Validate format matches `TKT-` followed by digits
- Call `bookingManager.cancelBooking(id)`
- Print success or failure message

**Option 3 — Look Up Booking:**
- Prompt for Ticket ID
- Call `bookingManager.lookupBooking(id)`

**Option 4 — View All Bookings:**
- Call `bookingManager.displayAllBookings()`

**Option 5 — View Seat Map:**
- Prompt: `"Select tier — 1. VIP  2. GENERAL  3. STANDING: "`
- Call `bookingManager.venue.displaySeatMap(tier)`

**Option 6 — View Inventory:**
- Call `bookingManager.venue.displayInventory()`

**Option 7 — Exit:**
- Print: `"Thank you for using Nexus Arena Booking System. Goodbye! 🎶"`
- `return 0`

**Error handling in main:**
- Wrap the entire menu loop in a `try/catch` block
- Catch `SoldOutException`, `InvalidBookingException`, `BookingNotFoundException` separately and print their `.what()` messages in red (use ANSI escape codes: `\033[31m` for red, `\033[0m` to reset)
- Catch `std::exception` as a fallback

---

## STEP 9 — README.md

Write a complete README with these exact sections:

### 1. Title & Description
```
# 🎵 Concert Ticket Booking System
A C++17 CLI application simulating real-world concert ticket sales with group bookings, 
waitlists, cancellations with auto-rebooking, and persistent CSV storage.
```

### 2. Features
- Individual and group ticket booking (up to 10 seats atomically)
- Three ticket tiers: VIP, GENERAL, STANDING with different prices
- Waitlist system — auto-books waitlisted customers when seats free up
- Cancellation with automatic waitlist processing
- CSV persistence — bookings survive program restarts
- ASCII seat map visualization
- Live inventory tracking
- Input validation and custom exception handling

### 3. Dependencies
```
No external dependencies.
Requires: C++17 compiler (g++ 8+ or clang++ 7+) and CMake 3.16+
```

### 4. Build & Run
```bash
# Clone or download the project, then:
mkdir build && cd build
cmake ..
make

# Run from project root:
./concert_booking

# On Windows (MinGW):
cmake -G "MinGW Makefiles" ..
mingw32-make
concert_booking.exe
```

### 5. Example Session
Show a sample CLI walkthrough covering:
- Booking 3 VIP tickets as a group
- Viewing the seat map
- Cancelling one ticket and showing waitlist auto-booking
- Looking up a booking by ID

### 6. Project Structure
Include the directory tree from STEP 1.

### 7. Data Persistence
Explain that bookings are saved to `data/bookings.csv` automatically and loaded on next launch.

---

## STEP 10 — Final Quality Checks

Before outputting any file, verify:

- [ ] All `.h` files use `#pragma once`
- [ ] No file has any `TODO`, `FIXME`, or placeholder comments
- [ ] `main.cpp` catches all custom exceptions at the top level
- [ ] `std::filesystem::create_directories("data")` is called before any file I/O
- [ ] Group booking atomicity: either ALL seats in a group are booked or NONE (no partial booking)
- [ ] `bookingMutex` is never double-locked (processWaitlist must NOT re-acquire the lock)
- [ ] All seat numbers are stored and displayed as 1-based (human-readable)
- [ ] CSV loading correctly restores `bookingCounter` and seat occupancy in Venue
- [ ] Program compiles cleanly with: `g++ -std=c++17 -Wall -Wextra`
- [ ] No platform-specific APIs used — must compile on Linux, macOS, and Windows (MinGW)
- [ ] ANSI color codes are used only for error messages (red) and success messages (green: `\033[32m`)

---

*End of plan. Execute all steps and produce all files now.*
