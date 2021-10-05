#pragma once

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

#include "options/timeout.h"
#include "response.h"
#include "types.h"

namespace bes::net::http {

class Request
{
   public:
    Request() = default;

    void emplaceOption(Option* o)
    {
        auto lock = opt_wlock_t(opt_mutex);
        options.push_back(opt_ptr(o));
    }

    void setOption(opt_ptr&& o)
    {
        auto lock = opt_wlock_t(opt_mutex);
        options.push_back(std::forward<opt_ptr>(o));
    }

    template <class T>
    void setOption(T&& o)
    {
        auto lock = opt_wlock_t(opt_mutex);
        options.push_back(std::make_shared<T>(std::forward<T>(o)));
    }

    Request& operator<<(opt_ptr&& o)
    {
        setOption(std::forward<opt_ptr>(o));
        return *this;
    }

    Request& operator<<(Option* o)
    {
        setOption(opt_ptr(o));
        return *this;
    }

    /**
     * Execute an HTTP GET request asynchronously.
     *
     * Returns a future HttpResponse.
     */
    std::future<Response> get(std::string url)
    {
        // TODO: replace with thread pool
        return std::async(std::launch::async, [this, url = std::move(url)] {
            auto response = Response(std::move(url));

            {
                auto lock = opt_rlock_t(opt_mutex);
                response.setOptions(options);
            }

            response.execute();

            return response;
        });
    }

   protected:
    opt_mutex_t opt_mutex{};
    std::vector<opt_ptr> options;
};

}  // namespace bes::net::http
