#include "response.h"

using namespace bes::fastcgi;

Response::Response(Request const& request, Transceiver& tns) : request(request), transceiver(tns) {}

std::string const& Response::getParam(std::string const& key) const
{
    return request.getParam(key);
}

bool Response::hasParam(std::string const& key) const
{
    return request.hasParam(key);
}

void Response::flush(bool force)
{
    flushStream(model::RecordType::OUT, out, force && !out_sent);
    flushStream(model::RecordType::ERR, err, force && !err_sent);
}

void Response::flushStream(model::RecordType rt, std::stringstream& str, bool force)
{
    str.seekg(0, std::ios::end);
    if (str.tellg() || force) {
        transceiver.writeStream(rt, str.str(), request.getRequestId());

        if (rt == model::RecordType::OUT) {
            out_sent = true;
        } else if (rt == model::RecordType::ERR) {
            err_sent = true;
        }
    }

    str.str(std::string());
}