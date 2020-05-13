#ifndef BES_LOG_CONSOLELOGBACKEND_H
#define BES_LOG_CONSOLELOGBACKEND_H

#include "../colour.h"
#include "../model.h"

namespace bes::log::backend {

class ConsoleLogBackend : public LogBackend
{
   public:
    explicit ConsoleLogBackend(LogFormat f = LogFormat::STANDARD, ColourMode use_colour = ColourMode::AUTO);

    [[nodiscard]] bool UsingColour() const;
    ConsoleLogBackend& SetColour(bool);

   protected:
    LogFormat fmt;
    bool use_colour;

   private:
    void Process(LogRecord const& log_record) override;
};

}  // namespace bes::log::backend

#endif
