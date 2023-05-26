#include <iostream>

#include "scl/math/fp.h"
#include "scl/ss/shamir.h"
#include "scl/util/prg.h"

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

    auto prg = util::PRG::Create();
    auto nz = RandomNonZero<Field>(prg);
    const auto shares = ss::ShamirShare(nz, 3, 4, prg);

    cout << "El secreto es :" << nz << endl;
    cout << "Se generan " << shares.Size() << " shares: \n";
    
    for (uint i = 0; i < shares.Size(); ++i) {
        cout << "\t- " << shares[i] << endl;
    }

    auto secret = ss::ShamirRecoverP(shares);

    cout << "Al reconstruir, queda: " << secret << endl;
    
}
