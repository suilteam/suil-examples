//
// Created by dc on 2019-12-22.
//

#include <suil/init.h>
#include <suil/cmdl.h>

#include "app.h"

using namespace suil;

static void cmdSet(cmdl::Parser& parser);
static void cmdInc(cmdl::Parser& parser);
static void cmdDec(cmdl::Parser& parser);
static void cmdGet(cmdl::Parser& parser);

int main(int argc, char *argv[])
{
    cmdl::Parser parser(APP_NAME, APP_VERSION, "Application used to interact with suil's sawtooth IntKey TP");
    try {
        parser << cmdl::Arg{"url", "The host address of the rest API server endpoint",
                      'H', false, false, true};
        parser << cmdl::Arg{"port", "The port on which rest API server is listening",
                            'P', false, false, true};
        parser << cmdl::Arg{"key", "The port on which rest API server is listening",
                            'K', false, false, true};
        // add commands and run
        cmdSet(parser);
        cmdInc(parser);
        cmdDec(parser);
        cmdGet(parser);

        parser.parse(argc, argv);
        parser.handle();

        return EXIT_SUCCESS;
    }
    catch(...) {
        // unhandled Exception
        serror("unhandled error %s", Exception::fromCurrent().what());
        return EXIT_FAILURE;
    }
}

void cmdSet(cmdl::Parser& parser)
{
    cmdl::Cmd set{"set", "Create a new key with the given name and value"};
    set([](cmdl::Cmd& cmd) {
        sawsdk::intkey::App app(cmd);
        app.modify("set");
    });
    parser.add(std::move(set));
}

void cmdInc(cmdl::Parser& parser)
{
    cmdl::Cmd inc{"inc", "Increment the given key with the given value"};
    inc([](cmdl::Cmd& cmd) {
        sawsdk::intkey::App app(cmd);
        app.modify("inc");
    });
    parser.add(std::move(inc));
}

void cmdDec(cmdl::Parser& parser)
{
    cmdl::Cmd dec{"dec", "Decrement the given key with the given value"};
    dec([](cmdl::Cmd& cmd) {
        sawsdk::intkey::App app(cmd);
        app.modify("dec");
    });
    parser.add(std::move(dec));
}

void cmdGet(cmdl::Parser& parser)
{
    cmdl::Cmd get{"get", "Get the value associated with the given key"};
    get([](cmdl::Cmd& cmd) {
        sawsdk::intkey::App app(cmd);
        app.get();
    });
    parser.add(std::move(get));
}