#include <suil/cmdl.h>
#include <suil/init.h>
#include "rpc.scc.h"

using namespace suil;

template <typename Server>
coroutine void run_Server(Server& server)
{
    sdebug("starting RPC server .....");
    server.run();
    sdebug("RPC server ended....");
}

template <typename Client>
void startClient(Client& client)
{
    client.connect("127.0.0.1", 4243);
    msleep(utils::after(2000));
    auto start = mnow();
    int count{10000};
    while(0<count--) {
        // sinfo("add(1, 4) = %d", client.add(1, 4));
        client.add(1, 10);
        User u1;
        u1.name = "Mpho";
        u1.age  = count;
        client.setUser(std::move(u1));
        u1 = client.getUser();
        //sdebug("you said user is %s of %d yrs old", u1.name(), u1.age);
    }
    sinfo("duration: %ld ms, calls=%d", mnow()-start, client.getCalls());
}

int main(int argc, char *argv[])
{
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, 1));

    sDemoServiceHandler sDemo;
    rpc::SuilRpcServer sServer(sDemo, opt(port, 4243));
    go(run_Server(sServer));
    msleep(utils::after(2000));
    /* Create client */
    sDemoServiceClient sClient;
    startClient(sClient);
    sServer.stop();

    jDemoServiceHandler jDemo;
    rpc::JsonRpcServer jServer(jDemo, opt(port, 4243));
    go(run_Server(jServer));
    msleep(utils::after(2000));
    /* Create client */
    jDemoServiceClient jClient;
    startClient(jClient);
    sServer.stop();

    return 0;
}
