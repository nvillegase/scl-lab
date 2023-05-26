#include "scl/math/fp.h"
#include "scl/util/prg.h"
#include <iostream>

using namespace scl;
using namespace std;
using Field = math::Fp<61>;

template <typename T>
T RandomNonZero(util::PRG& prg) {
    auto a = T::Random(prg);
    for (std::size_t i = 0; i < 10; ++i) {
        if (a == T::Zero()) {
            a = T::Random(prg);
        }
        break;
    }
    if (a == T::Zero()) {
        throw std::logic_error("could not generate a non-zero random value");
    }
    return a;
}

int main() {
    auto prg = util::PRG::Create("seed");
    Field zero = Field::Zero();
    Field one = Field::One();
    auto nz = RandomNonZero<Field>(prg);
    auto nz_plus_one = one + nz;
    auto nz_plus_zero = zero + nz;
    cout << nz << '\n';
    cout << nz_plus_zero << '\n';
    cout << nz_plus_one << '\n';
}
