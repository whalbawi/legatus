#include "legatus/status.h"

#include <cstdint>

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

namespace legatus {

TEST(StatusTest, Ok) {
    Status status = Status<int, int>::make_ok(-2);

    ASSERT_TRUE(status.is_ok());
    ASSERT_EQ(std::optional<int>(-2), status.ok());
    ASSERT_FALSE(status.is_err());
    ASSERT_EQ(std::nullopt, status.err());
}

TEST(StatusTest, Error) {
    Status status = Status<int, std::string>::make_err("error message");

    ASSERT_TRUE(status.is_err());
    ASSERT_EQ(std::optional<std::string>("error message"), status.err());
    ASSERT_FALSE(status.is_ok());
    ASSERT_EQ(std::nullopt, status.ok());
}

template <typename T>
struct ComplexResult {
    std::unique_ptr<T> result;
    std::vector<char> payload;
};

enum class ErrorCode : uint8_t {
    NOT_FOUND,
    INVALID,
    UNKNOWN,
};

TEST(StatusTest, ComplexOk) {
    const std::vector<char> payload = {'a', 'c', 'b'};
    const int result_expected = 23;
    ComplexResult<int> res{std::make_unique<int>(result_expected), payload};
    Status status = Status<ComplexResult<int>, ErrorCode>::make_ok(std::move(res));

    ASSERT_TRUE(status.is_ok());
    ASSERT_FALSE(status.is_err());
    ASSERT_EQ(std::nullopt, status.err());

    std::optional<ComplexResult<int>> ok_val = status.ok();
    ASSERT_TRUE(ok_val.has_value());
    const ComplexResult<int>& val = ok_val.value(); // NOLINT(bugprone-unchecked-optional-access)
    ASSERT_EQ(result_expected, *val.result);
    ASSERT_EQ(payload, val.payload);
}

TEST(StatusTest, ComplexError) {
    const std::vector<char> payload = {'a', 'c', 'b'};
    ComplexResult<ErrorCode> res{std::make_unique<ErrorCode>(ErrorCode::INVALID), payload};
    Status status = Status<float, ComplexResult<ErrorCode>>::make_err(std::move(res));

    ASSERT_TRUE(status.is_err());
    ASSERT_FALSE(status.is_ok());
    ASSERT_EQ(std::nullopt, status.ok());

    std::optional<ComplexResult<ErrorCode>> err_val = status.err();
    ASSERT_TRUE(err_val.has_value());
    const ComplexResult<ErrorCode>& val =
        err_val.value(); // NOLINT(bugprone-unchecked-optional-access)
    ASSERT_EQ(ErrorCode::INVALID, *val.result);
    ASSERT_EQ(payload, val.payload);
}

TEST(StatusTest, NoneOk) {
    Status status = Status<None, int>::make_ok();

    ASSERT_TRUE(status.is_ok());
    ASSERT_EQ(std::optional<None>({}), status.ok());
    ASSERT_FALSE(status.is_err());
    ASSERT_EQ(std::nullopt, status.err());
}

TEST(StatusTest, NoneErr) {
    Status status = Status<int>::make_err();

    ASSERT_TRUE(status.is_err());
    ASSERT_EQ(std::optional<None>({}), status.err());
    ASSERT_FALSE(status.is_ok());
    ASSERT_EQ(std::nullopt, status.ok());
}

} // namespace legatus
