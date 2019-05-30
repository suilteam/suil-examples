#include <suil/cmdl.h>
#include <suil/init.h>
#include <suil/process.h>
#include <suil/utils.h>

using namespace suil;

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, 2));
    auto  proc  = Process::bash({{"FILE", "/etc/sysctl.conf"}}, "cat", "${FILE}");
    auto  proc2 = Process::bash("tail", "-f", "/etc/sysctl.conf", "1>&2");

    proc->readAsync(var(onStdOutput) = [&](String &&output) {
        swarn("{--1--} %s", output());
        return true;
    });

    proc2->readAsync(var(onStdError) = [&](String&& err) {
        serror("{--2--} %s", err());
        return true;
    });

    proc2->waitExit(5000);
    proc2->terminate();
    return EXIT_SUCCESS;
}
