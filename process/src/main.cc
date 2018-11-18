#include <suil/utils.h>
#include <suil/cmdl.hpp>
#include <suil/process.h>

using namespace suil;

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose,2));
    auto  proc  = Process::launch({{"FILE", "/var/log/syslog"}}, "tail", "-f", "/var/log/syslog");
    auto  proc2 = Process::bash("tail", "-f", "/var/log/syslog", "1>&2");

    proc->readAsync(var(onStdOutput) =
        [&](zcstring &&output) {
            swarn("{--1--} %s", output());
            return true;
        }
    );
    proc2->readAsync(var(onStdError) = [&](zcstring&& err) {
        serror("{--2--} %s", err());
        return true;
    });

    proc->waitExit(10000);
    proc->terminate();
    proc2->waitExit();
    return EXIT_SUCCESS;
}