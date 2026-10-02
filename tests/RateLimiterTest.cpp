#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../RateLimiter.h"

using ::testing::HasSubstr;
using ::testing::NiceMock;
using ::testing::StrictMock;

// STUB: a hand-written Clock with canned, controllable time.
// It returns whatever we tell it to; we never verify calls on it.
class StubClock : public Clock {
public:
    long now = 1000;
    long nowSeconds() const override { return now; }
};

// MOCK: a Logger whose calls we set expectations on and verify.
class MockLogger : public Logger {
public:
    MOCK_METHOD(void, log, (const std::string& message), (override));
};

TEST(RateLimiterTest, AllowsRequestsUpToTheLimit) {
    StubClock clock;
    NiceMock<MockLogger> logger;
    RateLimiter limiter("api", 3, 60, clock, logger);

    EXPECT_TRUE(limiter.allowRequest("alice"));
    EXPECT_TRUE(limiter.allowRequest("alice"));
    EXPECT_TRUE(limiter.allowRequest("alice"));
}

TEST(RateLimiterTest, BlocksRequestOverTheLimitAndLogsOnce) {
    StubClock clock;
    StrictMock<MockLogger> logger;  // any unexpected log call fails the test
    RateLimiter limiter("api", 2, 60, clock, logger);

    EXPECT_CALL(logger, log(HasSubstr("alice"))).Times(1);

    EXPECT_TRUE(limiter.allowRequest("alice"));
    EXPECT_TRUE(limiter.allowRequest("alice"));
    EXPECT_FALSE(limiter.allowRequest("alice"));
}

TEST(RateLimiterTest, DoesNotLogWhenRequestIsAllowed) {
    StubClock clock;
    StrictMock<MockLogger> logger;
    RateLimiter limiter("api", 5, 60, clock, logger);

    EXPECT_CALL(logger, log(testing::_)).Times(0);

    EXPECT_TRUE(limiter.allowRequest("bob"));
}

TEST(RateLimiterTest, AllowsRequestsAgainAfterWindowExpires) {
    StubClock clock;
    NiceMock<MockLogger> logger;
    RateLimiter limiter("api", 1, 60, clock, logger);

    EXPECT_TRUE(limiter.allowRequest("carol"));
    EXPECT_FALSE(limiter.allowRequest("carol"));

    clock.now += 61;  // stubbed time moves past the 60 second window

    EXPECT_TRUE(limiter.allowRequest("carol"));
}

TEST(RateLimiterTest, StaysBlockedInsideTheWindow) {
    StubClock clock;
    NiceMock<MockLogger> logger;
    RateLimiter limiter("api", 1, 60, clock, logger);

    EXPECT_TRUE(limiter.allowRequest("dave"));
    clock.now += 59;
    EXPECT_FALSE(limiter.allowRequest("dave"));
}

TEST(RateLimiterTest, ResetWindowClearsUsedTokens) {
    StubClock clock;
    NiceMock<MockLogger> logger;
    RateLimiter limiter("api", 1, 60, clock, logger);

    EXPECT_TRUE(limiter.allowRequest("erin"));
    EXPECT_FALSE(limiter.allowRequest("erin"));

    limiter.resetWindow();

    EXPECT_TRUE(limiter.allowRequest("erin"));
}
