#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/bio.h>

#include <iostream>
#include <vector>

#include "BigNum.hpp"
#include "Participant.hpp"

void testInv(DH* dh)
{
    BIO* bioOut = BIO_new_fp(stdout, BIO_NOCLOSE);
    BN_CTX* ctx = BN_CTX_new();
    auto mod = DH_get0_p(dh);
    BigNumMod g{DH_get0_g(dh), mod, ctx};

    auto x = BigNumMod::getRandom(mod, ctx);
    auto y = BigNumMod::getRandom(mod, ctx);
    auto z = BigNumMod::getRandom(mod, ctx);
    auto gx = g^x;
    auto gy = g^y;
    auto gz = g^y;
    bioOut << (gx*gz*gy*(-gx)*(-gz)*(-gy)) << EL::endl;
    bioOut << (gy) << EL::endl;
    bioOut << (gy*gz) << EL::endl;
}

void testProt(DH* dh)
{
    BIO* bioOut = BIO_new_fp(stdout, BIO_NOCLOSE);
    BN_CTX* ctx = BN_CTX_new();
    auto mod = DH_get0_p(dh);
    BigNumMod g{DH_get0_g(dh), mod, ctx};

    BigNumMod x1 = BigNumMod::getRandom(mod, ctx);
    BigNumMod gx1 = g^x1;
    BigNumMod x2 = BigNumMod::getRandom(mod, ctx);
    BigNumMod gx2 = g^x2;
    BigNumMod x3 = BigNumMod::getRandom(mod, ctx);
    BigNumMod gx3 = g^x3;

    BigNumMod y1 = -(gx2*gx3);
    BigNumMod y2 = gx1*(-gx3);
    BigNumMod y3 = gx1*gx2;

    BigNumMod c1 = y1^x1;
    BigNumMod c2 = y2^x2;
    BigNumMod c3 = y3^x3;

    bioOut << (c1*c2*c3) << EL::endl;

}

struct VetoData
{
    VetoData(BigNumMod a, BigNumMod b, BigNumMod g) : x(a), gx(a), y(b)
    {
        gx = g^x;
    }
    BigNumMod x;
    BigNumMod gx;
    BigNumMod y;
};

void testNoVeto(DH* dh)
{
    BN_CTX* ctx = BN_CTX_new();
    auto mod = DH_get0_p(dh);
    auto q = DH_get0_q(dh);
    BigNumMod g{DH_get0_g(dh), mod, ctx};
    std::vector<VetoData> arr;
    for (int i = 0; i < 10; ++i)
    {
        arr.emplace_back(BigNumMod::getRandom(mod, ctx), BigNumMod::neutral(mod, ctx), g);
    }

    BIO* bioOut = BIO_new_fp(stdout, BIO_NOCLOSE);
    bioOut << arr[0].x << EL::endl;
    bioOut << arr[0].gx << EL::endl;
    bioOut << arr[0].y << EL::endl << EL::endl;

    for (int i = 0; i < 10 ;++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            if (j < i)
                arr[i].y = arr[i].y*arr[j].gx;
            else if (j > i)
                arr[i].y = arr[i].y*(-arr[j].gx);
        }
    }

    bioOut << arr[0].x << EL::endl;
    bioOut << arr[0].gx << EL::endl;
    bioOut << arr[0].y << EL::endl << EL::endl;

    auto res = BigNumMod::neutral(mod, ctx);
    bioOut << res << EL::endl;

    for (int i = 0; i < 10; ++i)
    {
        res = res*(arr[i].y^arr[i].x);
    }


    bioOut << res << EL::endl;
}

int main(int, char**) {
    BIO* dhParams = BIO_new_file("/home/wovik/veto_network/dh512.pem", "r");
    DH* dh = PEM_read_bio_DHparams(dhParams, NULL, NULL, NULL);
    if (!dh)
    {
        perror("Failed reading dh params ");
        return 1;
    }
    // testInv(dh);
    // testNoVeto(dh);
    // testProt(dh);

    unsigned short n;

    std::cout << "Enter number of participants: ";
    std::cin >> n;

    std::vector<Participant> participants;

    BN_CTX* ctx = BN_CTX_new();
    auto mod = DH_get0_p(dh);
    auto q = DH_get0_q(dh);
    BigNumMod g{DH_get0_g(dh), mod, ctx};

    for (int i = 0; i < n; ++i)
    {
        auto& participant = participants.emplace_back(g, mod, ctx);
        participant.i = i;

    }

    for (int i = 0; i < n; ++i)
        participants[i].calculateY(participants);

    BIO* bioOut = BIO_new_fp(stdout, BIO_NOCLOSE);
    BigNumMod result = BigNumMod::neutral(mod, ctx);
    // bioOut << result << EL::endl;

    for (int i = 0; i < n; ++i)
    {
        bool b;
        std::cout << "Participant " << i << " input : ";
        std::cin >> b;

        if (b)
            result = result*participants[i].veto();
        else
            result = result*participants[i].noVeto();

        // bioOut << result << EL::endl;
    }


    if (result == BigNumMod::neutral(mod, ctx))
        std::cout << "The result is 0" << std::endl;
    else
        std::cout << "The result is 1" << std::endl;

    return 0;
}
