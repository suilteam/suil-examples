#include <suil/init.h>
#include <suil/cmdl.h>
#include <suil/sawtooth/sdk.h>
#include <suil/json.h>
#include "addresser.h"

#define URL_PREFIX       "tcp://"
#define URL_PREFIX_LEN   6
#define URL_DEFAULT      "tcp://127.0.0.1:4004"

#define MIN_VALUE        0x0
#define MAX_VALUE        0xffffffff

using namespace suil;

define_log_tag(SAWTOOTH_INTKEY);

static const suil::String INTKEY_NAMESPACE{"intkey"};

struct IntkeyTransactor : public sawsdk::Transactor, LOGGER(SAWTOOTH_INTKEY) {

    IntkeyTransactor(sawsdk::intkey::Addresser& addresser, sawsdk::Transaction&& txn, sawsdk::GlobalState&& state)
        : sawsdk::Transactor(std::move(txn), std::move(state)),
          mAddresser(addresser)
    {}


    void apply() override  {
        idebug("IntKey apply");
        size_t size{Ego.mTxn.payload().size()};
        json::Object obj = json::Object::decode(
                reinterpret_cast<const char *>(Ego.mTxn.payload().data()), size);

        auto verb = (String) obj("Verb", false);
        if (verb.empty()) {
            throw Exception::create(sawsdk::Errors::InvalidTransaction, "Verb required");
        }

        auto name = (String) obj("Name", false);
        if (name.empty()) {
            throw Exception::create(sawsdk::Errors::InvalidTransaction, "Name required");
        }
        else if (name.size() > 20) {
            throw Exception::create(sawsdk::Errors::InvalidTransaction, "Name too long, maximum 20 characters");
        }

        auto vp = obj("Value", false);
        if (!vp) {
            throw Exception::create(sawsdk::Errors::InvalidTransaction, "Value required");
        }
        auto value = (uint32_t) vp;
        if (value < MIN_VALUE || value > MAX_VALUE) {
            throw Exception::create(sawsdk::Errors::InvalidTransaction, "Value '",
                    value, " is out range [", MIN_VALUE, " - ", MAX_VALUE, "]");
        }

        if (verb == "set") {
            Ego.doSet(name, value);
        }
        else if (verb == "dec") {
            Ego.doDec(name, value);
        }
        else if (verb == "inc") {
            Ego.doInc(name, value);
        }
        else {
            throw Exception::create(sawsdk::Errors::InvalidTransaction,
                    "Invalid verb '", verb, "'");
        }
    }
private:
    inline suil::String makeAddress(const suil::String& name) {
        return mAddresser.makeAddress(name, 64, std::string::npos);
    }

    void doSet(const suil::String& name, uint32_t value) {
        auto addr = Ego.makeAddress(name);
        idebug("Intkey::Transactor set {name: %s, address: %s, value: %s}", name(), addr(), value);

        auto state = Ego.mState.getState(addr);
        json::Object obj;
        if (!state.empty()) {
            obj = json::Object::decode(state);
            auto stateValue = obj(name(), false);
            if (stateValue) {
                auto num = (uint32_t) stateValue;
                throw Exception::create("Invalid set verb, already exists {name: ", name, ", value: ", num, "}");
            }
        }
        obj.set(name(), value);
        auto str = json::encode(obj);
        Ego.mState.setState(addr, sawsdk::fromStdString(str));
    }

    void doDec(const suil::String& name, uint32_t value) {

    }

    void doInc(const suil::String& name, uint32_t value) {

    }

private:
    sawsdk::intkey::Addresser& mAddresser;

};

struct IntKeyHandler : public sawsdk::TransactionHandler, LOGGER(SAWTOOTH_INTKEY) {
    IntKeyHandler()
        : mAddresser(INTKEY_NAMESPACE),
          mNsPrefix(mAddresser.getNamespacePrefix())
    {}

    suil::String getFamilyName() const {
        static const suil::String FAMILY_NAME{"intkey"};
        return FAMILY_NAME.peek();
    }

    std::vector<suil::String> getVersions() const {
        return {"1.0"};
    }

    std::vector<suil::String> getNamespaces() const {
        return {mNsPrefix.peek()};
    }

    sawsdk::Transactor::Ptr getTransactor(sawsdk::Transaction&& txn, sawsdk::GlobalState&& state) {
        return sawsdk::Transactor::Ptr(new IntkeyTransactor(mAddresser, std::move(txn), std::move(state)));
    }

private:
    sawsdk::intkey::Addresser mAddresser;
    suil::String mNsPrefix;
};

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, 0));

    try {
        sawsdk::TransactionHandler::UPtr handler(new IntKeyHandler);
        sawsdk::TransactionProcessor tp(URL_DEFAULT);
        tp.registerHandler(std::move(handler));
        tp.run();
    }
    catch (...) {
        auto ex = Exception::fromCurrent();
        serror("%s", ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}