//
// Created by dc on 27/12/18.
//

#include "rpc.scc.h"
#include "rpc.scc.cpp"

namespace suil {

    int DemoService::add(int a, int b)
    {
        return a + b;
    }

    void DemoService::setUser(const User&& usr)
    {
        sinfo("Hello user: %s, you are %d yrs old", usr.name(), usr.age);
    }

    User DemoService::getUser()
    {
        User u;
        u.name  = "Carter";
        u.age   = 29;
        return std::move(u);
    }
}