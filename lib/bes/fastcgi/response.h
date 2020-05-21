#ifndef BES_RESPONSE_H
#define BES_RESPONSE_H

#include "bes/log.h"
#include "bes/net.h"
#include "model.h"
#include "request.h"

namespace bes::fastcgi {

class Response
{
   public:
    Response(Request const& request, Transceiver& transceiver);

    virtual int Run() = 0;

    /**
     * Get a request parameter
     */
    std::string const& Param(std::string const& key) const;
    bool HasParam(std::string const& key) const;

    /**
     * Send output streams to the server.
     *
     * @param force Will force a stream sent (even if blank), but only if it hasn't been sent before.
     */
    virtual void Flush(bool force);

   protected:
    Request const& request;
    Transceiver& transceiver;

    std::stringstream out;
    std::stringstream err;

    bool out_sent = false;
    bool err_sent = false;

   private:
    void FlushStream(model::RecordType rt, std::stringstream& str, bool force = false);
};

}  // namespace bes::fastcgi

#endif
