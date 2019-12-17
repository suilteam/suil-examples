//
// Created by dc on 2019-12-17.
//

#ifndef SUIL_ADDRESSER_H
#define SUIL_ADDRESSER_H

namespace suil::sawsdk::intkey {

    struct Addresser {
        Addresser(const suil::String& ns);
        Addresser(Addresser&& other) noexcept ;
        Addresser&operator=(Addresser&& other) noexcept ;
        Addresser(const Addresser&) = delete;
        Addresser&operator=(const Addresser&) = delete;

        suil::String mapNamespace(const suil::String& key) const;
        suil::String mapKey(const suil::String& key, size_t pos, size_t count);
        const suil::String& getNamespacePrefix();
        suil::String makeAddress(const suil::String& key, size_t pos, size_t count);

    private:
        suil::String mNamespace{};
        suil::String mNsPrefix{};
    };
}
#endif //SUIL_ADDRESSER_H
