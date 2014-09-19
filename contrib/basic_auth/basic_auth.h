#pragma once

#include "../../include/crow.h"
#include "base64.h"

using namespace crow;

namespace crow_contrib
{

struct BasicAuth
{

private:
    struct credentials
    {
        string serverAuthString;
        string realm;

        bool passAuth(const string authHeader) const
        {
            return authHeader.length() == serverAuthString.length() && authHeader == serverAuthString;
        }
    };

    std::unordered_map<string, credentials> m_credentials;

    const credentials* getCredentials(const string url);

public:

    void addAuthorisation(
        const string routeRegex, 
        const string username,
        const string password,
        const string realm = "Authorization Required");


    struct context
    {
    };

    void before_handle(request& req, response& res, context& ctx)
    {
        auto creds = getCredentials(req.url);
        if(creds != nullptr) {
            CROW_LOG_INFO << "Credentials required for " << req.url;
            if(!req.headers.count("authorization") ||  !creds->passAuth(req.get_header_value("authorization")) ) {
                res.code = 401;
                res.add_header("WWW-Authenticate", "Basic realm=\"" + creds->realm + "\"");
                res.end();
            }
        } 
    }

    void after_handle(request& req, response& res, context& ctx)
    {
        // no-op
    }
};

//
//std::unordered_map<string, BasicAuth::credentials> BasicAuth::m_credentials;

void BasicAuth::addAuthorisation(
    const string routeRegex, 
    const string username, 
    const string password, 
    const string realm)
{
        
    string unencodedUserPass = username + ":" + password;
    int resultLen = 0;
    string encodedUserPass = base64(unencodedUserPass.c_str(), unencodedUserPass.length(), &resultLen);
    m_credentials[routeRegex] = credentials{string("Basic ") + encodedUserPass, realm};
}

const BasicAuth::credentials* BasicAuth::getCredentials(const string url)
{
    for(auto i = m_credentials.begin(); i != m_credentials.end(); ++i) {
        // TODO replace with regex?
        if (boost::starts_with(url, i->first)) {
            return &(i->second);
        }
    }
    return nullptr;
}

} // end namespace