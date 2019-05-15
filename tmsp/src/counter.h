//
// Created by dc on 13/03/19.
//

#ifndef SUIL_COUNTER_H
#define SUIL_COUNTER_H


#include <suil/tmsp/abci.h>

namespace suil::tmsp {

    struct  Counter: Application {

        virtual void getInfo(types::ResponseInfo& info);

        virtual Result setOption(const String& key, const String& value, types::ResponseSetOption& resp);

        virtual Result deliverTx(const suil::Data& tx, types::ResponseDeliverTx& resp);

        virtual Result checkTx(const Data& tx, types::ResponseCheckTx& resp);

        virtual void commit(types::ResponseCommit& resp);

        virtual Result query(const types::RequestQuery& req, types::ResponseQuery& resp);

    private:
        int     txCount{0};
        int     hashCount{0};
        bool    serial{true};
    };
}
#endif //SUIL_COUNTER_H
