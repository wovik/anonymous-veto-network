#pragma once

#include <openssl/bn.h>
#include <openssl/bio.h>

struct EL
{
    static EL endl;
    friend BIO* operator<<(BIO* bio, const EL& )
    {
        BIO_printf(bio, "\n");
        return bio;
    }
};

class BigNumMod
{
public:
    BigNumMod(const BIGNUM* bn, const BIGNUM* mod, BN_CTX* ctx) : bn_(bn), mod_(mod), ctx_(ctx)
    {
    }

    ~BigNumMod()
    {
    }

    // BigNumMod(const BigNumMod& rhs) : mod_(rhs.mod_), ctx_(rhs.ctx_)
    // {
    //     bn_ = BN_dup(rhs.bn_);
    // }

    // BigNumMod operator=(const BigNumMod& rhs)
    // {
    //     BIGNUM* bn = BN_dup(rhs.bn_);
    //     return BigNumMod(bn, rhs.mod_, rhs.ctx_);
    // }

    BigNumMod operator*(const BigNumMod& rhs)
    {
        BIGNUM* ret = BN_new();
        BN_mod_mul(ret, bn_, rhs.bn_, mod_, ctx_);
        return BigNumMod(ret, mod_, ctx_);
    }

    BigNumMod operator^(const BigNumMod& rhs)
    {
        BIGNUM* ret = BN_new();
        BN_mod_exp(ret, bn_, rhs.bn_, mod_, ctx_);
        return BigNumMod(ret, mod_, ctx_);
    }

    BigNumMod operator- ()
    {
        BIGNUM* ret = BN_new();
        BN_mod_inverse(ret, bn_, mod_, ctx_);
        return BigNumMod(ret, mod_, ctx_);
    }


    friend BIO* operator<<(BIO* bio, const BigNumMod& bnm)
    {
        BN_print(bio, bnm.bn_);
        BIO_printf(bio, " (mod ");
        BN_print(bio, bnm.mod_);
        BIO_printf(bio, ")");
        return bio;
    }

    bool operator==(const BigNumMod& rhs)
    {
        return BN_cmp(bn_, rhs.bn_) == 0;
    }

    static BigNumMod getRandom(const BIGNUM* mod, BN_CTX* ctx)
    {
        BIGNUM* ret = BN_new();
        BN_rand_range(ret, mod);
        return BigNumMod(ret, mod, ctx);
    }

    static BigNumMod neutral(const BIGNUM* mod, BN_CTX* ctx)
    {
        BIGNUM* a = BN_new();
        BN_one(a);
        return BigNumMod(a, mod, ctx);
    }

private:
    BN_CTX* ctx_;
    const BIGNUM* bn_;
    const BIGNUM* mod_;
};


EL EL::endl = EL();
