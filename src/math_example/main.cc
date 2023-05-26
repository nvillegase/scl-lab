#include "scl/math/fp.h"
#include "scl/util/prg.h"
#include <iostream>

using namespace scl;
using namespace std;
using Mersenne61 = math::Fp<61>;

int main() {
    auto prg = util::PRG::Create("seed");
    Mersenne61 zero = Mersenne61::Zero();
    Mersenne61 one = Mersenne61::One();
    auto random = Mersenne61::Random(prg);
    auto nz_plus_one = one + random;
    auto nz_plus_zero = zero + random;
    cout << random << '\n';
    cout << nz_plus_zero << '\n';
    cout << nz_plus_one << '\n';
}
