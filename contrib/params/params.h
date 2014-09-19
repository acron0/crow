#pragma once

#include "../../include/crow.h"

using namespace crow;

namespace crow_contrib
{

struct Params
{
    struct context
    {
    };

    void before_handle(request& req, response& res, context& ctx)
    {
    }

    void after_handle(request& req, response& res, context& ctx)
    {
        // no-op        
    }
};

} // end namespace