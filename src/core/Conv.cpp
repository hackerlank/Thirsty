#include "Conv.h"
#include <iterator>
#include "double-conversion.h" // V8 JavaScript implementation

using namespace double_conversion;

namespace detail {

extern const char digit1[101] =
    "00000000001111111111222222222233333333334444444444"
    "55555555556666666666777777777788888888889999999999";

extern const char digit2[101] =
    "01234567890123456789012345678901234567890123456789"
    "01234567890123456789012345678901234567890123456789";

template <> const char *const MaxString<bool>::value = "true";
template <> const char *const MaxString<uint8_t>::value = "255";
template <> const char *const MaxString<uint16_t>::value = "65535";
template <> const char *const MaxString<uint32_t>::value = "4294967295";
template <> const char *const MaxString<uint64_t>::value = "18446744073709551615";

} // namespace detail


void toAppend(std::string* result, double value, DtoaMode mode, uint32_t numDigits)
{
    DoubleToStringConverter conv(DoubleToStringConverter::NO_FLAGS,
        "infinity", "NaN", 'E',
        -6,  // decimal in shortest low
        21,  // decimal in shortest high
        6,   // max leading padding zeros
        1);  // max trailing padding zeros

    char buffer[256];
    StringBuilder builder(buffer, sizeof(buffer));
    switch (mode) 
    {
    case DoubleToStringConverter::SHORTEST:
        conv.ToShortest(value, &builder);
        break;
    case DoubleToStringConverter::FIXED:
        conv.ToFixed(value, numDigits, &builder);
        break;
    default:
        CHECK(mode == DoubleToStringConverter::PRECISION);
        conv.ToPrecision(value, numDigits, &builder);
        break;
    }
    const size_t length = builder.position();
    builder.Finalize();
    result->append(buffer, length);
}
