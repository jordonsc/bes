#pragma once

#include "../colour.h"
#include "../model.h"

namespace bes::log::backend {

class ConsoleLogBackend : public LogBackend
{
   public:
    explicit ConsoleLogBackend(LogFormat f = LogFormat::STANDARD, ColourMode use_colour = ColourMode::AUTO);

    [[nodiscard]] bool isColour() const;
    ConsoleLogBackend& setColour(bool);

   protected:
    LogFormat fmt;
    bool use_colour;

   private:
    void process(LogRecord  const& log_record) override;
};

}  // namespace bes::log::backend
