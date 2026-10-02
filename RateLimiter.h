#ifndef RATELIMITER_H
#define RATELIMITER_H

#include <string>

// Abstractions injected into RateLimiter so tests can replace them.
class Clock {
public:
    virtual ~Clock() = default;
    virtual long nowSeconds() const = 0;
};

class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& message) = 0;
};

class RateLimiter {
private:
    std::string limiterId;
    int maxRequests;
    int windowSeconds;
    int usedTokens;
    long windowStart;
    Clock& clock;
    Logger& logger;

public:
    RateLimiter(std::string limiterId, int maxRequests, int windowSeconds,
                Clock& clock, Logger& logger);

    bool allowRequest(const std::string& userId);
    void resetWindow();
};

#endif
