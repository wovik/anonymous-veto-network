#pragma once

#include <openssl/bn.h>

#include "BigNum.hpp"

class Participant
{
public:
    Participant(BigNumMod g, const BIGNUM* mod, BN_CTX* ctx)
    : g(g)
    , x(BigNumMod::getRandom(mod, ctx))
    , c(BigNumMod::getRandom(mod, ctx))
    , y(BigNumMod::neutral(mod, ctx))
    {
    }

    BigNumMod publishedX()
    {
        return g^x;
    }

    void calculateY(std::vector<Participant>& participants)
    {
        for (int j = 0; j < participants.size(); ++j)
        {
            if      (j < i) y = y*participants[j].publishedX();
            else if (i < j) y = y*(-participants[j].publishedX());
        }
    }

    BigNumMod veto()
    {
        return y^c;
    }

    BigNumMod noVeto()
    {
        return y^x;
    }

    int i;
    BigNumMod g;
private:
    BigNumMod x;
    BigNumMod c;
    BigNumMod y;
};