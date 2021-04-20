#pragma once

#include <bes/log.h>
#include <bes/net.h>

#include "model.h"
#include "request.h"

namespace bes::fastcgi {

class Response
{
   public:
    Response(Request const& request, Transceiver& transceiver);

    virtual int run() = 0;

    /**
     * Get a request parameter
     */
    std::string const& getParam(std::string const& key) const;
    bool hasParam(std::string const& key) const;

    /**
     * Send output streams to the server.
     *
     * @param force Will force a stream sent (even if blank), but only if it hasn't been sent before.
     */
    virtual void flush(bool force);

   protected:
    Request const& request;
    Transceiver& transceiver;

    std::stringstream out;
    std::stringstream err;

    bool out_sent = false;
    bool err_sent = false;

   private:
    void flushStream(model::RecordType rt, std::stringstream& str, bool force = false);
};

}  // namespace bes::fastcgi
