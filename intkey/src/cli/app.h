//
// Created by dc on 2019-12-28.
//

#ifndef SUIL_APP_H
#define SUIL_APP_H

#include <suil/cmdl.h>
#include <suil/sawtooth/client.h>

namespace suil::sawsdk::intkey {

    struct App : Client::HttpRest {
        DISABLE_COPY(App);
        DISABLE_MOVE(App);

        App(cmdl::Cmd& cmd);
        void modify(const suil::String& modify);
        void get();
        void batch();
        void list();

    private:
        cmdl::Cmd& args;
    };
}
#endif //SUIL_APP_H
