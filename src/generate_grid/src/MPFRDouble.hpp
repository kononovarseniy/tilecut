#pragma once

#include <mpfr.h>

#include <ka/common/fixed.hpp>

namespace ka
{

class MPFRDouble final
{
public:
    MPFRDouble()
    {
        mpfr_init2(value_, 53);
    }

    ~MPFRDouble()
    {
        mpfr_clear(value_);
    }

public:
    [[nodiscard]] operator mpfr_ptr() noexcept
    {
        return value_;
    }

    [[nodiscard]] explicit operator f64() noexcept
    {
        return mpfr_get_d(value_, MPFR_RNDN);
    }

private:
    mpfr_t value_;
};

} // namespace ka
