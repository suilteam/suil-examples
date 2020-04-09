//
// Created by dc on 2019-12-15.
//
#include <suil/json.h>

#include "tp.h"

namespace suil::sawsdk::intkey {

    constexpr uint32_t MIN_VALUE{0x0};
    constexpr uint32_t MAX_VALUE{0xffffffff};

    template <typename T>
    static json::Object parseJson(const T& data) {
        try {
            auto size{data.size()};
            return json::Object::decode(
                    reinterpret_cast<const char *>(data.data()), size);
        }
        catch (...) {
            auto ex = Exception::fromCurrent();
            throw Errors::invalidTransaction(ex.what());
        }
    }

    const suil::String IntKeyProcessor::NAMESPACE{"intkey"};

    void IntKeyProcessor::apply()
    {
        idebug("IntKey apply");
        size_t size{Ego.mTxn.payload().size()};
        json::Object obj = parseJson(Ego.mTxn.payload());

        auto verb = (String) obj("Verb", false);
        if (verb.empty()) {
            throw Errors::invalidTransaction("Verb required");
        }

        auto name = (String) obj("Name", false);
        if (name.empty()) {
            throw Errors::invalidTransaction("Name required");
        }
        else if (name.size() > 20) {
            throw Errors::invalidTransaction("Name too long, maximum 20 characters");
        }

        auto vp = obj("Value", false);
        if (!vp) {
            throw Exception::create(sawsdk::Errors::InvalidTransaction, "Value required");
        }
        auto value = (uint32_t) vp;
        if (value < MIN_VALUE || value > MAX_VALUE) {
            throw Errors::invalidTransaction("Value '",
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
            throw Errors::invalidTransaction("Invalid verb '", verb, "'");
        }
    }

    void IntKeyProcessor::doSet(const suil::String& name, uint32_t value)
    {
        auto addr = Ego.mkAddr(name);
        idebug("Intkey::Transactor set {name: %s, address: %s, value: %u}", name(), addr(), value);

        auto state = Ego.mState.getState(addr);
            json::Object obj;
        if (!state.empty()) {
            obj = parseJson(state);
            auto stateValue = obj(name(), false);
            if (stateValue) {
                auto num = (uint32_t) stateValue;
                throw Errors::invalidTransaction("Invalid set verb, already exists {name: ", name, ", value: ", num, "}");
            }
        }
        else {
            obj = json::Object(json::Obj, name(), value);
        }
        auto str = json::encode(obj);
        Ego.mState.setState(addr, sawsdk::fromStdString(str));
    }

    void IntKeyProcessor::doDec(const suil::String& name, uint32_t value)
    {
        auto addr = mkAddr(name);
        idebug("Intkey::Transactor decrement {name: %s, address: %s, value: %u}", name(), addr(), value);

        json::Object obj;
        auto state = Ego.mState.getState(addr);
        if (!state.empty()) {
            obj = parseJson(state);
            if (!obj(name(), false)) {
                throw Errors::invalidTransaction(
                        "Failed to decrement '", name, "' which does not exist in state");
            }
        }
        else {
            throw Errors::invalidTransaction(
                    "Failed to decrement '", name, "' which does not have state");
        }
        auto stateValue = (uint32_t) obj[name];
        if (value > (stateValue - MIN_VALUE)) {
            throw Exception::create(Errors::InvalidTransaction,
                "Failed to decrement '", name, "' input value to large");
        }

        stateValue -= value;
        obj = json::Object(json::Obj, name(), stateValue);
        auto str = json::encode(obj);
        Ego.mState.setState(addr, sawsdk::fromStdString(str));
    }

    void IntKeyProcessor::doInc(const suil::String& name, uint32_t value)
    {
        auto addr = Ego.mkAddr(name);
        idebug("Intkey::Transactor increment {name: %s, address: %s, value: %u}", name(), addr(), value);

        json::Object obj;
        auto state = Ego.mState.getState(addr);
        if (!state.empty()) {
            obj = parseJson(state);
            if (!obj(name(), false)) {
                throw Errors::invalidTransaction(
                        "Failed to increment '", name, "' which does not exist in state");
            }
        }
        else {
            throw Errors::invalidTransaction(
                    "Failed to increment '", name, "' which does not have state");
        }
        auto stateValue = (uint32_t) obj[name];
        if ((MAX_VALUE - stateValue) < value) {
            throw Errors::invalidTransaction(
                    "Failed to increment '", name, "' input value to large");
        }

        stateValue += value;
        obj = json::Object(json::Obj, name(), stateValue);
        auto str = json::encode(obj);
        Ego.mState.setState(addr, sawsdk::fromStdString(str));
    }
}