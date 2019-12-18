//
// Created by dc on 2019-12-15.
//

#ifndef SUIL_INTKEY_TP_H_
#define SUIL_INTKEY_TP_H_

#include <suil/sawtooth/sdk.h>

#include "addresser.h"

namespace suil::sawsdk::intkey {

define_log_tag(SAWSDK_INTKEY);

struct IntkeyTransactor : public sawsdk::Transactor, LOGGER(SAWSDK_INTKEY) {

    IntkeyTransactor(sawsdk::intkey::Addresser& addresser, sawsdk::Transaction&& txn, sawsdk::GlobalState&& state);

    void apply() override;

public:
    static const suil::String NAMESPACE;

private:
    inline suil::String makeAddress(const suil::String& name) {
        return mAddresser.makeAddress(name, 64, std::string::npos);
    }

    void doSet(const suil::String& name, uint32_t value);

    void doDec(const suil::String& name, uint32_t value);

    void doInc(const suil::String& name, uint32_t value);

private:
    sawsdk::intkey::Addresser& mAddresser;

};

struct IntKeyHandler : public sawsdk::TransactionHandler, LOGGER(SAWSDK_INTKEY) {

    IntKeyHandler();

    suil::String getFamilyName() const override;

    std::vector<suil::String> getVersions() const override;

    std::vector<suil::String> getNamespaces() const override;

    sawsdk::Transactor::Ptr getTransactor(sawsdk::Transaction&& txn, sawsdk::GlobalState&& state) override;

private:
    sawsdk::intkey::Addresser mAddresser;
    suil::String mNsPrefix;
};

}

#endif // SUIL_INTKEY_TP_H_