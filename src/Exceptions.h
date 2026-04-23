#pragma once

#include <stdexcept>
#include <string>

class SoldOutException : public std::runtime_error {
public:
    explicit SoldOutException(const std::string& message)
        : std::runtime_error(message) {}
};

class InvalidBookingException : public std::runtime_error {
public:
    explicit InvalidBookingException(const std::string& message)
        : std::runtime_error(message) {}
};

class BookingNotFoundException : public std::runtime_error {
public:
    explicit BookingNotFoundException(const std::string& message)
        : std::runtime_error(message) {}
};
