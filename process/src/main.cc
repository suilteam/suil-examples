#include <suil/cmdl.h>
#include <suil/init.h>
#include <suil/process.h>
#include <suil/utils.h>

using namespace suil;

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose,2));
    auto  proc  = Process::launch({{"FILE", "/var/log/syslog"}}, "tail", "-f", "/var/log/syslog");
    auto  proc2 = Process::bash("tail", "-f", "/var/log/syslog", "1>&2");

    proc->readAsync(var(onStdOutput) =
        [&](String &&output) {
            swarn("{--1--} %s", output());
            return true;
        }
    );
    proc2->readAsync(var(onStdError) = [&](String&& err) {
        serror("{--2--} %s", err());
        return true;
    });

    proc->waitExit(10000);
    proc->terminate();
    proc2->waitExit();
    return EXIT_SUCCESS;
}