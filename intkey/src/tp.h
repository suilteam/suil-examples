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
        static const String NAMESPACE;

    private:
        void doSet(const String& name, uint32_t value);
        void doDec(const String& name, uint32_t value);
        void doInc(const String& name, uint32_t value);

    private:
        AddressEncoder mkAddr{NAMESPACE};
    };

}

#endif // SUIL_INTKEY_TP_H_