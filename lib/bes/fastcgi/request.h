#ifndef BES_FASTCGI_REQUEST_H
#define BES_FASTCGI_REQUEST_H

#include <bes/core.h>

#include <unordered_map>

#include "exception.h"
#include "memory.tcc"
#include "model.h"
#include "transceiver.h"

namespace bes::fastcgi {

class Request
{
   public:
    Request(Transceiver& tns, bes::Container const& cnt);
    bool Run();

    model::Role Role() const;
    uint8_t Flags() const;

    std::string const& Param(std::string const& key) const;
    bool HasParam(std::string const& key) const;
    std::unordered_map<std::string, std::string> const& Params() const;

    uint16_t RequestId() const;

    bes::Container const& container;

   protected:
    /**
     * Process an arbitrary record. Will read the header then invoke the correct handler.
     */
    bool ProcessRecord();

    // Input processors
    void ProcessBeginRequest(model::Header const& header);
    void ProcessParams(model::Header const& header);
    void ProcessIn(model::Header const& header);

    /**
     * Validate that the server is sending the expected record length for a fixed-length record.
     */
    Request& ValidateRecordLength(model::Header const& header, size_t expected);

    /**
     * Reads either 1 byte or 4 bytes of a variable-size field representing a data length.
     */
    uint32_t GetVariableSizeLength(size_t& read_counter);

    Transceiver& transceiver;

    uint16_t request_id = 0;
    model::Role role = static_cast<model::Role>(0);
    uint8_t flags = 0;
    std::string in_data;
    std::unordered_map<std::string, std::string> params;
};

}  // namespace bes::fastcgi

#endif
