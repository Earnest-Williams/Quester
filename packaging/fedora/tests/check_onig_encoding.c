#include <assert.h>
#include <oniguruma.h>

#include "qsp/bindings/default/qsp_default.h"

int main(void) {
    assert(QSP_ONIG_ENC == ONIG_ENCODING_KOI8_R);
    return 0;
}
