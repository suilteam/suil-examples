//
// Created by dc on 2019-12-15.
//
#include <suil/json.h>

#include "tp.h"

namespace suil::sawsdk::intkey {

constexpr uint32_t MIN_VALUE{0x0};
constexpr uint32_t MAX_VALUE{0xffffffff};


const suil::String IntkeyTransactor::NAMESPACE{"intkey"};

IntkeyTransactor::IntkeyTransactor(sawsdk::intkey::Addresser& addresser, sawsdk::Transaction&& txn, sawsdk::GlobalState&& state)
    : sawsdk::Transactor(std::move(txn), std::move(state)),
        mAddresser(addresser)
{}

void IntkeyTransactor::apply()
{
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

void IntkeyTransactor::doSet(const suil::String& name, uint32_t value)
{
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

void IntkeyTransactor::doDec(const suil::String& name, uint32_t value)
{
    auto addr = Ego.makeAddress(name);
    idebug("Intkey::Transactor decrement {name: %s, address: %s, value: %s}", name(), addr(), value);

    json::Object obj;
    auto state = Ego.mState.getState(addr);
    if (!state.empty()) {
        obj = json::Object::decode(state);
        if (!obj(name(), false)) {
            throw Exception::create(Errors::InvalidTransaction,
                "Failed to decrement '", name, "' which does not exist in state");
        }
    }
    else {
        throw Exception::create(Errors::InvalidTransaction,
            "Failed to decrement '", name, "' which does not have state");
    }
    auto stateValue = (uint32_t) obj[name];
    if (value > (stateValue - MIN_VALUE)) {
        throw Exception::create(Errors::InvalidTransaction,
            "Failed to decrement '", name, "' input value to large");
    }

    stateValue -= value;
    obj.set(name(), stateValue);
    auto str = json::encode(obj);
    Ego.mState.setState(addr, sawsdk::fromStdString(str));
}

void IntkeyTransactor::doInc(const suil::String& name, uint32_t value)
{
    auto addr = Ego.makeAddress(name);
    idebug("Intkey::Transactor increment {name: %s, address: %s, value: %s}", name(), addr(), value);

    json::Object obj;
    auto state = Ego.mState.getState(addr);
    if (!state.empty()) {
        obj = json::Object::decode(state);
        if (!obj(name(), false)) {
            throw Exception::create(Errors::InvalidTransaction,
                "Failed to increment '", name, "' which does not exist in state");
        }
    }
    else {
        throw Exception::create(Errors::InvalidTransaction,
            "Failed to increment '", name, "' which does not have state");
    }
    auto stateValue = (uint32_t) obj[name];
    if ((MAX_VALUE - stateValue) > value) {
        throw Exception::create(Errors::InvalidTransaction,
            "Failed to increment '", name, "' input value to large");
    }

    stateValue += value;
    obj.set(name(), stateValue);
    auto str = json::encode(obj);
    Ego.mState.setState(addr, sawsdk::fromStdString(str));
}

IntKeyHandler::IntKeyHandler()
    : mAddresser(IntkeyTransactor::NAMESPACE),
      mNsPrefix(mAddresser.getNamespacePrefix())
{}

suil::String IntKeyHandler::getFamilyName() const {
    return IntkeyTransactor::NAMESPACE.peek();
}

std::vector<suil::String> IntKeyHandler::getVersions() const {
    return {"1.0"};
}

std::vector<suil::String> IntKeyHandler::getNamespaces() const {
    return {mNsPrefix.peek()};
}

sawsdk::Transactor::Ptr IntKeyHandler::getTransactor(sawsdk::Transaction&& txn, sawsdk::GlobalState&& state) {
    return sawsdk::Transactor::Ptr(new IntkeyTransactor(mAddresser, std::move(txn), std::move(state)));
}

}