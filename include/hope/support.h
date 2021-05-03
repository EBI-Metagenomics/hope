#ifndef HOPE_SUPPORT_H
#define HOPE_SUPPORT_H

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int __hope_errors = 0;

inline static int __hope_close(double actual, double desired, double rel_tol,
                               double abs_tol);

inline static void __hope_print_context(const char *file, int line)
{
    fprintf(stderr, "\nAssertion error at %s:%d\n", file, line);
}

inline static void __hope_print_newline(void)
{
    fprintf(stderr, "\n");
    fflush(stderr);
}

#define __HOPE_REL_TOL(x) _Generic((x), float : 5e-05, double : 1e-09)

static void __hope_close2(double actual, double desired, double rel_tol,
                          double abs_tol, char const *file, int line)
{
    double a = actual;
    double d = desired;
    if (__hope_close(a, d, rel_tol, abs_tol))
    {
        __hope_print_context(file, line);
        fprintf(stderr, " Items are not close:\n");
        fprintf(stderr, "  ACTUAL : %.10f\n", (double)a);
        fprintf(stderr, "  DESIRED: %.10f\n", (double)d);
        __hope_print_newline();
        ++__hope_errors;
    }
}

#define __MAKE_EQ(S, T, F)                                                     \
    static void __hope_eq_##S(T a, T d, char const *file, int line)            \
    {                                                                          \
        if (!(a == d))                                                         \
        {                                                                      \
            __hope_print_context(file, line);                                  \
            fprintf(stderr, " Items are not equal:\n");                        \
            fprintf(stderr, "  ACTUAL : %" F "\n", a);                         \
            fprintf(stderr, "  DESIRED: %" F "\n", d);                         \
            __hope_print_newline();                                            \
            ++__hope_errors;                                                   \
        }                                                                      \
    }

__MAKE_EQ(uint64, uint64_t, PRIu64)
__MAKE_EQ(uint32, uint32_t, PRIu32)
__MAKE_EQ(uint16, uint16_t, PRIu16)
__MAKE_EQ(uint8, uint8_t, PRIu8)

__MAKE_EQ(int64, int64_t, PRIi64)
__MAKE_EQ(int32, int32_t, PRIi32)
__MAKE_EQ(int16, int16_t, PRIi16)
__MAKE_EQ(int8, int8_t, PRIi8)

#define __hope_eq(actual, desired, file, line)                                 \
    _Generic((actual), uint64_t                                                \
             : __hope_eq_uint64, uint32_t                                      \
             : __hope_eq_uint32, uint16_t                                      \
             : __hope_eq_uint16, uint8_t                                       \
             : __hope_eq_uint8, int64_t                                        \
             : __hope_eq_int64, int32_t                                        \
             : __hope_eq_int32, int16_t                                        \
             : __hope_eq_int16, int8_t                                         \
             : __hope_eq_int8, char *                                          \
             : __hope_eq_str, char const *                                     \
             : __hope_eq_str)((actual), (desired), file, line)

static void __hope_isnull(void const *addr, int cond, char const *expr,
                          char const *file, int line)
{
    if (cond)
    {
        __hope_print_context(file, line);
        fprintf(stderr, " Address should be NULL:\n");
        fprintf(stderr, "  ADDRESS   : %p\n", addr);
        fprintf(stderr, "  EXPRESSION: %s\n", expr);
        __hope_print_newline();
        ++__hope_errors;
    }
}

static void __hope_notnull(void const *addr, int cond, char const *expr,
                           char const *file, int line)
{
    if (cond)
    {
        __hope_print_context(file, line);
        fprintf(stderr, " Address should not be NULL:\n");
        fprintf(stderr, "  ADDRESS   : %p\n", addr);
        fprintf(stderr, "  EXPRESSION: %s\n", expr);
        __hope_print_newline();
        ++__hope_errors;
    }
}

static void __hope_eq_str(char const *a, char const *d, char const *file,
                          int line)
{
    if (strcmp(a, d) != 0)
    {
        __hope_print_context(file, line);
        fprintf(stderr, " Strings are not equal:\n");
        fprintf(stderr, "  ACTUAL : %s\n", a);
        fprintf(stderr, "  DESIRED: %s\n", d);
        __hope_print_newline();
        ++__hope_errors;
    }
}

static void __hope_cond(char const *expr, int cond, char const *file, int line)
{
    if (cond)
    {
        __hope_print_context(file, line);
        fprintf(stderr, " Condition evaluates to false:\n");
        fprintf(stderr, "  EXPRESSION: %s\n", expr);
        __hope_print_newline();
        ++__hope_errors;
    }
}

inline static int __hope_close(double actual, double desired, double rel_tol,
                               double abs_tol)
{
    /* This implementation is basically a copy of the `math.isclose`
     * implementation of the Python library plus returning 0 in case
     * both values are NaN.
     */
    if (actual == desired)
    {
        /* short circuit exact equality -- needed to catch two infinities of
         * the same sign. And perhaps speeds things up a bit sometimes.
         */
        return 0;
    }

    if (isnan(actual) && isnan(desired))
    {
        return 0;
    }

    /* This catches the case of two infinities of opposite sign, or
     * one infinity and one finite number. Two infinities of opposite
     * sign would otherwise have an infinite relative tolerance.
     * Two infinities of the same sign are caught by the equality check
     * above.
     */

    if (isinf(actual) || isinf(desired))
    {
        return 1;
    }

    /* now do the regular computation
     * this is essentially the "weak" test from the Boost library
     */

    double diff = fabs(desired - actual);

    return !(((diff <= fabs(rel_tol * desired)) ||
              (diff <= fabs(rel_tol * actual))) ||
             (diff <= abs_tol));
}

#endif
