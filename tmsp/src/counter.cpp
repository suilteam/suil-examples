//
// Created by dc on 13/03/19.
//

#include "counter.h"

namespace suil::tmsp {


    void Counter::getInfo(suil::tmsp::types::ResponseInfo &info)
    {
        OBuffer ob(64);
        ob.appendf(R"({"hashes":%d, "txs": %d})", hashCount, txCount);
        info.set_data(ob.data(), ob.size());
    }


    Result Counter::setOption(const suil::String &key, const suil::String &value,
                              suil::tmsp::types::ResponseSetOption &resp)
    {
        if (key == "serial" && value == "on") {
            Ego.serial = true;
        }
        return Result{Codes::Ok};
    }

    Result Counter::deliverTx(const suil::Data &tx, suil::tmsp::types::ResponseDeliverTx &resp)
    {
        Result res;
        if (Ego.serial) {
            if (tx.size()>9) {
                res(Codes::EncodingError)
                    << "Max expected tx size is 8 bytes, got " << tx.size() << " bytes";
                return std::move(res);
            }

            VarInt        vi{0};
            suil::Heapboard hb{tx};
            hb >> vi;
            auto txValue = vi.read<int64_t>();
            if (txValue != Ego.txCount) {
                res(Codes::InternalError)
                    << "Invalid nounce, expecting '" << Ego.txCount
                    << "', got '" << txValue << "'";
                return std::move(res);
            }
        }
        Ego.txCount++;
        return std::move(res);
    }

    Result Counter::checkTx(const suil::Data &tx, suil::tmsp::types::ResponseCheckTx &resp)
    {
        Result res;
        if (Ego.serial) {
            if (tx.size()>8) {
                res(Codes::EncodingError)
                        << "Max expected tx size is 8 bytes, got " << tx.size() << " bytes";
                return std::move(res);
            }

            VarInt        vi{0};
            suil::Heapboard hb{tx};
            hb >> vi;
            auto txValue = vi.read<int64_t>();
            if (txValue != Ego.txCount) {
                res(Codes::InternalError)
                        << "Invalid nounce, expecting >= '" << Ego.txCount
                        << "', got '" << txValue << "'";
                return std::move(res);
            }
        }

        return std::move(res);
    }

    void Counter::commit(suil::tmsp::types::ResponseCommit &resp)
    {
        Ego.hashCount++;
        if (Ego.txCount != 0) {
            suil::Stackboard<32> sb;
            sb << Ego.txCount;
            auto raw = sb.raw();
            resp.set_data(raw.data(), raw.size());
        }
    }

    Result Counter::query(const types::RequestQuery &req, types::ResponseQuery &resp)
    {
        Result res{Codes::Ok};
        String path(req.path());

        if (path == "hash") {
            auto str = utils::hexstr((uint8_t *)&Ego.hashCount, sizeof(Ego.hashCount));
            resp.set_value(str.data(), str.size());
        }
        else if (path == "tx") {
            resp.set_value(&Ego.txCount, sizeof(Ego.txCount));
        }
        else {
            res(Codes::InternalError)
                << "Invalid query path";
        }
        return std::move(res);
    }
}