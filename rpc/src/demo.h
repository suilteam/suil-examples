//
// Created by dc on 15/05/19.
//

#ifndef SUIL_DEMO_H
#define SUIL_DEMO_H

#include "types.scc.h"

namespace suil {

    struct DemoAttrs {
    protected:
        String serviceName{"DemoService"};
        int    totalCalls{0};
        User   user;
    };
}
#endif //SUIL_DEMO_H
