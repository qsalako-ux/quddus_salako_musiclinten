#include "RateLimiter.h"

RateLimiter::RateLimiter(std::string limiterId, int maxRequests, int windowSeconds,
                         Clock& clock, Logger& logger)
    : limiterId(std::move(limiterId)),
      maxRequests(maxRequests),
      windowSeconds(windowSeconds),
      usedTokens(0),
      windowStart(clock.nowSeconds()),
      clock(clock),
      logger(logger) {}

bool RateLimiter::allowRequest(const std::string& userId) {
    if (clock.nowSeconds() - windowStart >= windowSeconds) {
        resetWindow();
    }
    if (usedTokens >= maxRequests) {
        logger.log("Rate limit exceeded for user " + userId);
        return false;
    }
    ++usedTokens;
    return true;
}

void RateLimiter::resetWindow() {
    usedTokens = 0;
    windowStart = clock.nowSeconds();
}
