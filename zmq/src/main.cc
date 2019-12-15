#include <suil/init.h>
#include <suil/cmdl.h>
#include <suil/zmq/zmq.h>

using namespace suil;

coroutine void respond(zmq::Context& ctx) {
    zmq::Responder sock(ctx);
    if (!sock.bind("tcp://*:5555")) {
        serror("Failed to bind to tcp://*:5555");
        return;
    }

    sinfo("Waiting for zmq messages to arrive");
    while (1) {
        String msg = sock.receive();
        if (msg.empty()) {
            swarn("received terminal message");
            return;
        }
        sinfo("Server received %.*s", msg.size(), msg());
        msleep(mnow()+1000);
        sock.send(msg);
    }
}

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, 0));
    zmq::Context context;

    go(respond(context));
    msleep(mnow()+500);

    zmq::Requestor sock(context);
    if (!sock.connect("tcp://localhost:5555")) {
        serror("Failed to connect to tcp://localhost:5555");
        return EXIT_FAILURE;
    }

    sock.send("Hello World");
    String str = sock.receive();
    sinfo("Client received message %.*s", str.size(), str());
    sock.send("");
    msleep(mnow()+1000);

    return EXIT_SUCCESS;
}