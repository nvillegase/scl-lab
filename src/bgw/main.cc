#include <iostream>
#include <math.h>

#include "scl/math/fp.h"
#include "scl/ss/shamir.h"
#include "scl/util/prg.h"

using namespace scl;
using namespace std;
using Mersenne61 = math::Fp<61>;

int main() {
 
    auto prg = util::PRG::Create();
    auto x = Mersenne61::Random(prg);
    auto y = Mersenne61::Random(prg);
    auto xy = x * y;

    cout << "x = " << x << endl;
    cout << "y = " << y << endl;
    cout << "x * y = " << xy << endl;
    
    uint n = 8;
    auto t = floor(n / 2); // t < n / 2

    const auto x_shares_t = ss::ShamirShare(x, t, n, prg);
    const auto y_shares_t = ss::ShamirShare(y, t, n, prg);
    const auto xy_shares_t_direct = ss::ShamirShare(xy, t, n, prg);
    auto xy_shares_t = math::Vec<Mersenne61>(n);
    
    auto z_shares = std::vector<math::Vec<Mersenne61>>();
    
    for (uint i = 0; i < n; ++i) {
        // Las parties multiplican localmente [x*y]_2t <- [x]_t * [y]_t
        auto xy_share_2t = x_shares_t[i] * y_shares_t[i]; 

        // Resharing: Cada party genera y distribuye sharings [z_i]_t
        const auto zi_shares = ss::ShamirShare(xy_share_2t, t, n, prg);
        z_shares.emplace_back(zi_shares);
    }

    for (uint i = 0; i < n; ++i) {
        
        // Las parties computan localmente [xy]_t
        auto zi_shares = math::Vec<Mersenne61>(n);
        for (uint j = 0; j < n; ++j) {
            zi_shares[j] = z_shares[j][i];
        }
        auto xy_share_t = ss::ShamirRecoverP(zi_shares);
        xy_shares_t[i] = xy_share_t;
    }

    // Reconstrucción de x*y
    auto xy_reconstructed = ss::ShamirRecoverP(xy_shares_t);
    cout << "Al reconstruir, queda: " << xy_reconstructed << endl;

    if (xy_reconstructed == xy) {
        cout << "Reconstrucción exitosa." << endl;
        return 0;
    }
    else {
        cout << "Error en la reconstrucción!" << endl;
        return 1;
    }
    
}
