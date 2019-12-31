//
// Created by dc on 2019-12-28.
//
#include <suil/json.h>
#include <suil/sawtooth/client.h>
#include <suil/http/clientapi.h>
#include "app.h"

namespace suil::sawsdk::intkey {

    App::App(cmdl::Cmd& cmd)
        : HttpRest(
                cmd.getvalue("url", "http://localhost"),
                "intkey",
                "1.0",
                cmd.getvalue("key", "2c5683d43573214fa939d2df4ac0767fe3500b6eacbafa11cf8b8aeca8db9d60"),
                // 0364b993e9c7f3dc07c6ee9f1c5f3b4c52d6062e757dd861f0b8cf7e7608b283f0
                cmd.getvalue<int32_t>("port", 8008)),
          args(cmd)
    {}

    void App::modify(const suil::String& verb)
    {
        auto name = args.getPositional<String>(0, "Missing key name - usage: intkeycli set|dec|inc [...] name value");
        auto value = args.getPositional<int32_t>(1, "Missing key value - usage: intkeycli set|dec|inc [...] name value");
        json::Object jobj(json::Obj,
                  "Verb",  verb,
                  "Name",  name,
                  "Value", value);
        auto str = json::encode(jobj);
        auto prefix = Ego.getPrefix();
        Ego.asyncBatches(fromStdString(str), {prefix}, {prefix});
    }

    void App::get()
    {
        auto name = args.getPositional<String>(0, "Missing key name - usage: intkeycli get [...] endpoint name");
        auto state = Ego.getState(name);
        if (!state.empty()) {
            auto obj = json::Object::decode(state);
            auto value = (int32_t) obj(name());
            sdebug("%s = %d", name(), value);
        }
    }
}