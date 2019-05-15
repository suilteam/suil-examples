#include <suil/cmdl.h>
#include <suil/init.h>
#include <suil/utils.h>
#include <suil/tmsp/server.h>

#include "counter.h"

using namespace suil;

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, 0));
    tmsp::Counter counter;
    tmsp::AbciTcpServer server(counter, opt(port, 26658));
    return server.start();
}