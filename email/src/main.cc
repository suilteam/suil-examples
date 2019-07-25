#include <suil/cmdl.h>
#include <suil/email.h>
#include <suil/init.h>

using namespace suil;

int main(int argc, char *argv[])
{
    suil::init();
    log::setup(opt(verbose, 1));

    SslMailOutbox outbox("smtp.gmail.com", 000, {"sender@gmail.com", "Sender Name"});

    if (outbox.login("sender@gmail.com", "password")) {
        auto msg = outbox.draft("recepient@gmail.com", "Testing MailOutbux");
        msg->body() << "Hello beautiful user";
        auto why = outbox.send(std::move(msg), 15000);
        if (why) {
            serror("Why: %s", why());
        }
    }

    return 0;
}