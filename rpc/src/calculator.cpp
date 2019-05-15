//
// Created by dc on 27/12/18.
//

#include "rpc.scc.h"
#include "rpc.scc.cpp"

namespace suil {

    DemoService::DemoService()
    {
        user.name = "Carter Mbotho";
        user.age = 29;
    }

    int DemoService::add(int a, int b)
    {
        Ego.totalCalls++;
        return a + b;
    }

    void DemoService::setUser(User&& usr)
    {
        Ego.totalCalls++;
        user = std::move(usr);
    }

    User DemoService::getUser()
    {
        Ego.totalCalls++;
        return Ego.user;
    }

    int DemoService::getCalls()
    {
        return Ego.totalCalls++;
    }
}
