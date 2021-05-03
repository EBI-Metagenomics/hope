#include "hope/hope.h"
#include <math.h>

int main()
{
    char a = 'f';
    COND(3 == 3);
    NOTNULL(&a);
    ISNULL(NULL);
    EQ("afdw", "afdw");
    EQ(2, 2);
    COND(isinf(INFINITY));
    COND(isnan(NAN));
    CLOSE(1.0, 1.0 + 1e-10);
    CLOSE(0.0, 0.0);
    CLOSE2(0.0, 0.0 + 1e-10, 1e-9, 1e-9);
    return hope_status();
}
