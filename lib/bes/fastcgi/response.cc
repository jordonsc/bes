#include "response.h"

using namespace bes::fastcgi;

Response::Response(Request const& request, Transceiver& tns) : request(request), transceiver(tns) {}

std::string const& Response::Param(std::string const& key) const
{
    return request.Param(key);
}

bool Response::HasParam(std::string const& key) const
{
    return request.HasParam(key);
}

void Response::Flush(bool force)
{
    FlushStream(model::RecordType::OUT, out, force && !out_sent);
    FlushStream(model::RecordType::ERR, err, force && !err_sent);
}

void Response::FlushStream(model::RecordType rt, std::stringstream& str, bool force)
{
    str.seekg(0, std::ios::end);
    if (str.tellg() || force) {
        transceiver.WriteStream(rt, str.str(), request.RequestId());

        if (rt == model::RecordType::OUT) {
            out_sent = true;
        } else if (rt == model::RecordType::ERR) {
            err_sent = true;
        }
    }

    str.str(std::string());
}