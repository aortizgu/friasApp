#include "DigestAuthFilter.h"
#include "Md5.h"
#define REALM "myrealm"
#define OPAQUE "myopaque"

std::string method2String(HttpMethod m)
{
    switch (m)
    {
    case Get:
        return "GET";
    case Post:
        return "POST";
    case Head:
        return "HEAD";
    case Put:
        return "PUT";
    case Delete:
        return "DELETE";
    case Options:
        return "OPTIONS";
    default:
        return "INVALID";
    }
}

bool DigestAuthFilter::isEndOfAttributeName(size_t pos, size_t len, const char *data)
{
    if (pos >= len)
        return true;
    if (isspace(static_cast<unsigned char>(data[pos])))
        return true;
    // The reason for this complexity is that some attributes may contain trailing
    // equal signs (like base64 tokens in Negotiate auth headers)
    if ((pos + 1 < len) && (data[pos] == '=') &&
        !isspace(static_cast<unsigned char>(data[pos + 1])) &&
        (data[pos + 1] != '='))
    {
        return true;
    }
    return false;
}

void DigestAuthFilter::httpParseAttributes(const char *data,
                                           size_t len,
                                           HttpAttributeList &attributes)
{
    size_t pos = 0;
    while (true)
    {
        // Skip leading whitespace
        while ((pos < len) && isspace(static_cast<unsigned char>(data[pos])))
        {
            ++pos;
        }

        // End of attributes?
        if (pos >= len)
            return;

        // Find end of attribute name
        size_t start = pos;
        while (!isEndOfAttributeName(pos, len, data))
        {
            ++pos;
        }

        HttpAttribute attribute;
        attribute.first.assign(data + start, data + pos);

        // Attribute has value?
        if ((pos < len) && (data[pos] == '='))
        {
            ++pos; // Skip '='
            // Check if quoted value
            if ((pos < len) && (data[pos] == '"'))
            {
                while (++pos < len)
                {
                    if (data[pos] == '"')
                    {
                        ++pos;
                        break;
                    }
                    if ((data[pos] == '\\') && (pos + 1 < len))
                        ++pos;
                    attribute.second.append(1, data[pos]);
                }
            }
            else
            {
                while ((pos < len) && !isspace(static_cast<unsigned char>(data[pos])) &&
                       (data[pos] != ','))
                {
                    attribute.second.append(1, data[pos++]);
                }
            }
        }

        attributes.push_back(attribute);
        if ((pos < len) && (data[pos] == ','))
            ++pos; // Skip ','
    }
}

bool DigestAuthFilter::httpHasAttribute(const HttpAttributeList &attributes,
                                        const std::string &name,
                                        std::string *value)
{
    for (HttpAttributeList::const_iterator it = attributes.begin();
         it != attributes.end(); ++it)
    {
        if (it->first == name)
        {
            if (value)
            {
                *value = it->second;
            }
            return true;
        }
    }
    return false;
}

DigestAuthFilter::DigestAuthFilter(const std::map<std::string, std::string> &credentials) : credentials(credentials)
{
}

void DigestAuthFilter::doFilter(const HttpRequestPtr &req,
                                FilterCallback &&cb,
                                FilterChainCallback &&ccb)
{
    trantor::Date now = trantor::Date::date();
    if (!req->session())
    {
        // no session support by framework,pls enable session
        auto resp = HttpResponse::newNotFoundResponse();
        cb(resp);
        return;
    }

    auto auth_header = req->getHeader("Authorization");
    if (!auth_header.empty())
    {
        LOG_DEBUG << "Check WWW-Authenticate header";
        HttpAttributeList att_list;
        httpParseAttributes(auth_header.c_str(), auth_header.size(), att_list);
        std::string username, realm, nonce, uri, opaque, response;
        bool has_required_attr = true;
        has_required_attr &= httpHasAttribute(att_list, "username", &username);
        has_required_attr &= httpHasAttribute(att_list, "realm", &realm);
        has_required_attr &= httpHasAttribute(att_list, "nonce", &nonce);
        has_required_attr &= httpHasAttribute(att_list, "uri", &uri);
        has_required_attr &= httpHasAttribute(att_list, "opaque", &opaque);
        has_required_attr &= httpHasAttribute(att_list, "response", &response);
        if (has_required_attr)
        {
            LOG_DEBUG << "username: " << username;
            LOG_DEBUG << "realm: " << realm;
            LOG_DEBUG << "nonce: " << nonce;
            LOG_DEBUG << "uri: " << uri;
            LOG_DEBUG << "opaque: " << opaque;
            LOG_DEBUG << "response: " << response;
            LOG_DEBUG << "credentials size: " << credentials.size();
            if (credentials.find(username) != credentials.end())
            {
                std::string A1 = username + ":" + realm + ":" + credentials.at(username);
                std::string middle;
                std::string qop, cnonce, ncount;
                if (httpHasAttribute(att_list, "qop", &qop) && httpHasAttribute(att_list, "cnonce", &cnonce) && httpHasAttribute(att_list, "ncount", &ncount))
                {
                    qop = "auth";
                    middle = nonce + ":" + ncount + ":" + cnonce + ":" + qop;
                }
                else
                {
                    middle = nonce;
                }
                std::string A2 = method2String(req->getMethod()) + ":" + uri;
                std::string A1_middle_A2 = Md5Encode::encode(A1) + ":" + middle + ":" + Md5Encode::encode(A2);
                LOG_DEBUG << "A1: " << A1;
                LOG_DEBUG << "middle: " << middle;
                LOG_DEBUG << "A2: " << A2;
                LOG_DEBUG << "A1:middle:A2: " << A1_middle_A2;
                std::string calculated_response = Md5Encode::encode(A1_middle_A2);
                if (response == calculated_response)
                {
                    // Passed
                    ccb();
                    return;
                }
                else
                {
                    LOG_DEBUG << "invalid response " << response << ", calculated " << calculated_response;
                }
            }
            else
            {
                LOG_DEBUG << "invalid username " << username;
            }
        }
        else
        {
            LOG_DEBUG << "missing attributes in WWW-Authenticate header" << auth_header;
        }
    }
    // not Passed
    LOG_DEBUG << "Fill WWW-Authenticate header";
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k401Unauthorized);
    resp->addHeader("WWW-Authenticate", " Digest realm=\"" + std::string(REALM) + "\", nonce=\"" + Md5Encode::encode(std::to_string(time(0))) + "\", opaque=\"" + std::string(OPAQUE) + "\"");
    cb(resp);
    return;
}