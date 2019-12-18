//
// Created by dc on 2019-12-17.
//
#include <suil/utils.h>
#include "addresser.h"

namespace suil::sawsdk::intkey {

    constexpr size_t MERKLE_ADDRESS_LENGTH = 70;
    constexpr size_t NAMESPACE_PREFIX_LENGTH = 6;


    static bool isHex(const suil::String& str) {
        suil::strview  view = str;
        return view.find_first_not_of("0123456789abcdef") == std::string::npos;
    }

    static void isValidAddr(const suil::String& addr) {
        if (addr.size() != MERKLE_ADDRESS_LENGTH) {
            throw Exception::create("Address size is invalid, {Expected: ",
                    MERKLE_ADDRESS_LENGTH, ", Got: ", addr.size(), "}");
        }
        else if (!isHex(addr)) {
            throw Exception::create("Address must be lowercase hexadecimal ONLY");
        }
    }

    static void isNamespaceValid(const suil::String& ns) {
        if (ns.size() != NAMESPACE_PREFIX_LENGTH) {
            throw Exception::create("Namespace size is invalid, {Expected: ",
                    NAMESPACE_PREFIX_LENGTH, ", Got: ", ns.size(), "}");
        }
        else if (!isHex(ns)) {
            throw Exception::create("Namespace prefix must be lowercase hexadecimal ONLY");
        }
    }

    Addresser::Addresser(const suil::String &ns)
        : mNamespace(ns.dup())
    {}

    Addresser::Addresser(Addresser &&other) noexcept
        : mNamespace(std::move(other.mNamespace)),
          mNsPrefix(std::move(other.mNsPrefix))
    {}

    Addresser& Addresser::operator=(suil::sawsdk::intkey::Addresser &&other) noexcept
    {
        Ego.mNamespace = std::move(other.mNamespace);
        Ego.mNsPrefix = std::move(other.mNsPrefix);
        return Ego;
    }

    suil::String Addresser::mapKey(const suil::String &key, size_t pos, size_t count)
    {
        return utils::SHA512(key).substr(pos, count, false);
    }

    suil::String Addresser::mapNamespace(const suil::String &ns) const
    {
        return utils::SHA512(ns).substr(0, NAMESPACE_PREFIX_LENGTH, false);
    }

    const suil::String& Addresser::getNamespacePrefix() {
        if (Ego.mNsPrefix.empty()) {
            Ego.mNsPrefix = Ego.mapNamespace(Ego.mNamespace);
            isNamespaceValid(Ego.mNsPrefix);
        }
        return Ego.mNsPrefix;
    }

    suil::String Addresser::makeAddress(const suil::String& key, size_t pos, size_t count) {
        auto addr = utils::catstr(Ego.getNamespacePrefix(), Ego.mapKey(key, pos, count));
        isValidAddr(addr);
        return addr;
    }
}