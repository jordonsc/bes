#ifndef BES_LOG_CONSOLELOGBACKEND_H
#define BES_LOG_CONSOLELOGBACKEND_H

#include "../colour.h"
#include "lib/log/model.h"

namespace bes::log::backend {

class ConsoleLogBackend : public LogBackend
{
   public:
    ConsoleLogBackend(LogFormat f = LogFormat::STANDARD, ColourMode use_colour = ColourMode::AUTO);

    bool UsingColour() const;
    ConsoleLogBackend& SetColour(bool);

   protected:
    LogFormat fmt;
    bool use_colour;

   private:
    virtual void Process(LogRecord const& log_record);
};

}  // namespace bes::log::backend

#endif
