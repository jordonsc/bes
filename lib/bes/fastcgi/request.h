#pragma once

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
    bool run();

    model::Role getRole() const;
    uint8_t getFlags() const;

    std::string const& getParam(std::string const& key) const;
    bool hasParam(std::string const& key) const;
    std::unordered_map<std::string, std::string> const& getParams() const;

    uint16_t getRequestId() const;

    bes::Container const& container;

   protected:
    /**
     * Process an arbitrary record. Will read the header then invoke the correct handler.
     */
    bool processRecord();

    // Input processors
    void processBeginRequest(model::Header const& header);
    void processParams(model::Header const& header);
    void processIn(model::Header const& header);

    /**
     * Validate that the server is sending the expected record length for a fixed-length record.
     */
    Request& validateRecordLength(model::Header const& header, size_t found);

    /**
     * Reads either 1 byte or 4 bytes of a variable-size field representing a data length.
     */
    uint32_t getVariableSizeLength(size_t& read_counter);

    Transceiver& transceiver;

    uint16_t request_id = 0;
    model::Role role = static_cast<model::Role>(0);
    uint8_t flags = 0;
    std::string in_data;
    std::unordered_map<std::string, std::string> params;
};

}  // namespace bes::fastcgi
