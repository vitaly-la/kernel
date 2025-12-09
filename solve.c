#include <stddef.h>
#include <stdint.h>

static size_t dgtcnt(uint64_t number)
{
    size_t ans = 0;
    for (; number; ++ans, number /= 10) {}
    return ans;
}

static uint64_t leftmost_grp(size_t n, size_t sz, uint64_t number)
{
    for (size_t i = 0; i < sz * (n - 1); ++i) number /= 10;
    return number;
}

static uint64_t pow(uint64_t base, size_t n)
{
    uint64_t ans = 1;
    for (size_t i = 0; i < n; ++i) ans *= base;
    return ans;
}

static uint64_t compose(uint64_t base, size_t n)
{
    size_t sz = dgtcnt(base);
    uint64_t ans = 0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < sz; ++j) ans *= 10;
        ans += base;
    }
    return ans;
}

static uint64_t solve_n(size_t n, uint64_t lower, uint64_t upper)
{
    size_t lower_cnt = dgtcnt(lower);
    size_t upper_cnt = dgtcnt(upper);

    size_t lower_grp_size = (lower_cnt + n - 1) / n;
    size_t upper_grp_size = upper_cnt / n;

    uint64_t lower_leftmost = leftmost_grp(n, lower_grp_size, lower);
    uint64_t upper_leftmost = leftmost_grp(n, upper_grp_size, upper);

    uint64_t lower_limit = pow(10, lower_grp_size - 1);
    uint64_t upper_limit = pow(10, upper_grp_size) - 1;

    if (lower_leftmost < lower_limit) lower_leftmost = lower_limit;
    if (upper_leftmost > upper_limit) upper_leftmost = upper_limit;

    uint64_t lower_compose = compose(lower_leftmost, n);
    uint64_t upper_compose = compose(upper_leftmost, n);

    if (lower_compose < lower) ++lower_leftmost;
    if (upper_compose > upper) --upper_leftmost;

    uint64_t ans = 0;
    for (uint64_t x = lower_leftmost; x <= upper_leftmost; ++x) {
        ans += compose(x, n);
    }

    return ans;
}

uint64_t solve(uint64_t lower, uint64_t upper)
{
    uint64_t ans = 0;
    ans += solve_n(2, lower, upper);
    ans += solve_n(3, lower, upper);
    ans += solve_n(5, lower, upper);
    ans += solve_n(7, lower, upper);
    ans -= solve_n(6, lower, upper);
    ans -= solve_n(10, lower, upper);
    return ans;
}
