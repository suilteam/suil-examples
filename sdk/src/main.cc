#include <suil/init.h>
#include <suil/logging.h>
#include <suil/crypto.h>
#include <suil/secp256k1.h>

using namespace suil;

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, 2));

    auto key = crypto::ECKey::generate();
    auto pub = key.getPublicKey().toString();
    auto priv = key.getPrivateKey().toString();
    sinfo("private_key: %s", priv());
    sinfo("public_key: %s", pub());
    auto key2 = crypto::ECKey::fromKey(key.getPrivateKey());
    auto pub2 = key2.getPublicKey().toString();
    auto priv2 = key2.getPrivateKey().toString();
    sinfo("private_key2: %s", priv2());
    sinfo("public_key2: %s", pub2());
    auto privKey = crypto::PrivateKey::fromString(priv);
    auto key3 = crypto::ECKey::fromKey(privKey);
    auto pub3 = key3.getPublicKey().toString();
    auto priv3 = key3.getPrivateKey().toString();
    sinfo("private_key3: %s", priv3());
    sinfo("public_key3: %s", pub3());
    String msg{"Hello world again"};
    auto sig = crypto::ECDSASign(privKey, msg);
    auto sigStr = sig.toString();
    auto sigCB64 = sig.toCompactForm();
    auto sigCHex = sig.toCompactForm(false);
    sinfo("signature: %s", sigStr());
    sinfo("signature_comp_b64: %s", sigCB64());
    sinfo("signature_comp_hex: %s", sigCHex());
    auto sig1 = crypto::ECDSASignature::fromCompactForm(sigCB64);
    auto sig2 = crypto::ECDSASignature::fromCompactForm(sigCHex, false);
    bool isValid = crypto::ECDSAVerify(msg, sig, key.getPublicKey());
    sinfo("message sig is: %s", (isValid? "Valid" : "Not Valid"));
    isValid = crypto::ECDSAVerify(msg, sig1, key.getPublicKey());
    sinfo("message sig1 is: %s", (isValid? "Valid" : "Not Valid"));
    isValid = crypto::ECDSAVerify(msg, sig2, key.getPublicKey());
    sinfo("message sig2 is: %s", (isValid? "Valid" : "Not Valid"));

    auto key4 = secp256k1::KeyPair::fromPrivateKey(priv);
    auto key4p = key4.Private.toString();
    auto key4P = key4.Public.toString();
    sinfo("secp256k1_priv %s", key4p());
    sinfo("secp256k1_pub %s", key4P());
    auto sig4 = secp256k1::ECDSASign(key4.Private, msg);
    auto sig4B64 = sig4.toString(true);
    auto sig4Hex = sig4.toString();
    sinfo("secp256k1_sigb64 %s", sig4B64());
    sinfo("secp256k1_sigbHex %s", sig4Hex());
    isValid = secp256k1::ECDSAVerify(msg, sig4, key4.Public);
    sinfo("message sig4 is: %s", (isValid? "Valid" : "Not Valid"));
    return EXIT_SUCCESS;
}