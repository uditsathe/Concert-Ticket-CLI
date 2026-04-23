# 🎵 Concert Ticket Booking System 

A C++ CLI application simulating real-world concert ticket sales with group bookings,
waitlists, cancellations with auto-rebooking, and persistent CSV storage.

This documentation for the project is AI generated, I recommend running the project first hand.

---

## Features

- **Individual and group ticket booking** — book up to 10 seats atomically (all-or-none)
- **Three ticket tiers** — VIP (₹5,000), GENERAL (₹2,000), STANDING (₹800)
- **Waitlist system** — customers are queued when a tier is sold out and auto-booked when seats free up
- **Cancellation with automatic waitlist processing** — cancelling a ticket triggers immediate waitlist resolution
- **CSV persistence** — all bookings are saved to `data/bookings.csv` and survive program restarts
- **ASCII seat map visualization** — see which seats are available (`O`) or booked (`X`) in a grid layout
- **Live inventory tracking** — view total, booked, and available seats per tier at any time
- **Input validation and custom exception handling** — robust error messages for invalid inputs

---

## Dependencies

```
No external dependencies.
Requires: C++17 compiler (g++ 8+ or clang++ 7+) and CMake 3.16+
```

---

## Build & Run

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

---

## Example Session (AI was used to generate this example after implmentation, actual view will differ a bit)

```
╔══════════════════════════════════════════════╗
║       🎵  NEXUS ARENA TICKET BOOKING  🎵      ║
║         Concert Ticket Booking System         ║
╚══════════════════════════════════════════════╝

  Tier        | Total | Booked | Available | Price (INR)
  ----------------------------------------------------------
  VIP         |    50 |      0 |        50 | ₹5000.00
  GENERAL     |   200 |      0 |       200 | ₹2000.00
  STANDING    |   500 |      0 |       500 |  ₹800.00

════════════════════ MAIN MENU ════════════════════
  1. Book Ticket(s)
  ...

# Step 1: Book 3 VIP tickets as a group
Enter your name: Alice
Select tier — 1. VIP (₹5000)  2. GENERAL (₹2000)  3. STANDING (₹800): 1
Number of tickets (1-10, enter >1 for group booking): 3

✅ Booking Confirmed!
  ┌─────────────────────────────────────┐
  │  Ticket ID   : TKT-00001
  │  Customer    : Alice
  │  Tier        : VIP
  │  Seat No.    : 1
  │  Price       : ₹5000.00
  │  Group       : Yes (3 seats)
  │  Booked At   : 2026-04-24 10:30:00
  └─────────────────────────────────────┘
  ... (TKT-00002, TKT-00003 printed similarly)

# Step 2: View the VIP seat map
Select tier — 1. VIP  2. GENERAL  3. STANDING: 1

  Seat Map — VIP (Nexus Arena)
  --------------------------------------------------
  Row  1   X  X  X  O  O  O  O  O  O  O
  Row  2   O  O  O  O  O  O  O  O  O  O
  ...
  Legend:  O = Available   X = Booked

# Step 3: Cancel one ticket — waitlist entry gets auto-booked
Enter Ticket ID to cancel (e.g. TKT-00001): TKT-00002
✅ Booking TKT-00002 cancelled successfully.
🎉 Waitlist notification: Bob's booking for 1 x VIP seat(s) has been confirmed! Ticket(s): TKT-00004

# Step 4: Look up a booking
Enter Ticket ID to look up: TKT-00001
  ┌─────────────────────────────────────┐
  │  Ticket ID   : TKT-00001
  │  Customer    : Alice
  │  Tier        : VIP
  │  Seat No.    : 1
  │  Price       : ₹5000.00
  │  Group       : Yes (3 seats)
  │  Booked At   : 2026-04-24 10:30:00
  └─────────────────────────────────────┘
```

---

## Project Structure

```
.
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

## Data Persistence

All bookings are automatically saved to `data/bookings.csv`.
