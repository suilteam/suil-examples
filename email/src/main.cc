#include <suil/cmdl.h>
#include <suil/email.h>
#include <suil/init.h>

using namespace suil;

int main(int argc, char *argv[])
{
    suil::init();
    log::setup(opt(verbose, 1));

    TcpMailOutbox outbox("localhost", 2525, {"sender@example.com", "Sender Name"});

    if (outbox.login("sender@example.com", "password")) {
        auto msg = outbox.draft("recepient@example.com", "Testing MailOutbux");
        msg->body() << "Hello beautiful user";
        auto why = outbox.send(std::move(msg), 15000);
        if (why) {
            serror("Why: %s", why());
        }
    }

    return 0;
}