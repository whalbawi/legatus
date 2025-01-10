#include "legatus/status.h"

#include <cstdint>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

namespace legatus {

TEST(StatusTest, Ok) {
    Status status = Status<int, int>::make_ok(-2);

    ASSERT_TRUE(status.is_ok());
    ASSERT_EQ(-2, status.ok());
    ASSERT_FALSE(status.is_err());
}

TEST(StatusTest, Error) {
    Status status = Status<int, std::string>::make_err("error message");

    ASSERT_TRUE(status.is_err());
    ASSERT_EQ(std::string("error message"), status.err());
    ASSERT_FALSE(status.is_ok());
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

    const ComplexResult<int> cplx_res = status.ok();
    ASSERT_EQ(result_expected, *cplx_res.result);
    ASSERT_EQ(payload, cplx_res.payload);
}

TEST(StatusTest, ComplexError) {
    const std::vector<char> payload = {'a', 'c', 'b'};
    ComplexResult<ErrorCode> res{std::make_unique<ErrorCode>(ErrorCode::INVALID), payload};
    Status status = Status<float, ComplexResult<ErrorCode>>::make_err(std::move(res));

    ASSERT_TRUE(status.is_err());
    ASSERT_FALSE(status.is_ok());

    const ComplexResult<ErrorCode> cplx_res = status.err();
    ASSERT_EQ(ErrorCode::INVALID, *cplx_res.result);
    ASSERT_EQ(payload, cplx_res.payload);
}

TEST(StatusTest, NoneOk) {
    Status status = Status<None, int>::make_ok();

    ASSERT_TRUE(status.is_ok());
    ASSERT_EQ(None{}, status.ok());
    ASSERT_FALSE(status.is_err());
}

TEST(StatusTest, NoneErr) {
    Status status = Status<int>::make_err();

    ASSERT_TRUE(status.is_err());
    ASSERT_EQ(None{}, status.err());
    ASSERT_FALSE(status.is_ok());
}

} // namespace legatus
