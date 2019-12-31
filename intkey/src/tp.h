//
// Created by dc on 2019-12-15.
//

#ifndef SUIL_INTKEY_TP_H_
#define SUIL_INTKEY_TP_H_

#include <suil/sawtooth/sdk.h>

namespace suil::sawsdk::intkey {

    define_log_tag(SAWSDK_INTKEY);

    struct IntKeyProcessor : public Processor, LOGGER(SAWSDK_INTKEY) {
        using Processor::Processor;

        void apply() override;

    public:
        static const suil::String NAMESPACE;

    private:
        void doSet(const suil::String& name, uint32_t value);
        void doDec(const suil::String& name, uint32_t value);
        void doInc(const suil::String& name, uint32_t value);
    };

    struct IntKeyHandler : public sawsdk::TransactionHandler, LOGGER(SAWSDK_INTKEY)
    {
        IntKeyHandler();

        Processor::Ptr getProcessor(Transaction&& txn, GlobalState&& state) override;
    };

}

#endif // SUIL_INTKEY_TP_H_