#include <suil/init.h>
#include <suil/cmdl.h>
#include <suil/sawtooth/sdk.h>

#include "tp.h"

#define URL_DEFAULT      "tcp://127.0.0.1:4004"


using namespace suil;

static void startTransactionProcessor(suil::String&& endpont);

static void cmd_Start(cmdl::Parser& parser) {
    // run command
    cmdl::Cmd start{"start", "Start running intkey transaction processor"};
    start << cmdl::Arg{"endpoint", "An MQTT endoint of the validator to connect to",
                     'C', false, false};
    start([](cmdl::Cmd& cmd){
        // hand run command
        String config = cmd.getvalue<String>("endpoint", URL_DEFAULT).dup();
        startTransactionProcessor(std::move(config));
    });
    parser.add(std::move(start));
}

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, 0));

    cmdl::Parser parser(APP_NAME, APP_VERSION, "intkey transaction processor");
    try {
        // add commands and run
        cmd_Start(parser);
        parser.parse(argc, argv);
        parser.handle();

        return EXIT_SUCCESS;
    }
    catch(...) {
        // unhandled Exception
        serror("unhandled error %s", Exception::fromCurrent().what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void startTransactionProcessor(suil::String&& endpont)
{
    try {
        sawsdk::TransactionHandler::UPtr handler(new sawsdk::intkey::IntKeyHandler);
        sawsdk::TransactionProcessor tp(std::move(endpont));
        tp.registerHandler(std::move(handler));
        tp.run();
    }
    catch (...) {
        auto ex = Exception::fromCurrent();
        serror("%s", ex.what());
    }
}