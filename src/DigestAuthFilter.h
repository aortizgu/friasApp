#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;

typedef std::pair<std::string, std::string> HttpAttribute;
typedef std::vector<HttpAttribute> HttpAttributeList;

class DigestAuthFilter : public drogon::HttpFilter<DigestAuthFilter, false>
{
  const std::map<std::string, std::string> &credentials;
  static bool isEndOfAttributeName(size_t pos, size_t len, const char *data);
  static void httpParseAttributes(const char *data,
                                  size_t len,
                                  HttpAttributeList &attributes);

  static bool httpHasAttribute(const HttpAttributeList &attributes,
                               const std::string &name,
                               std::string *value);

public:
  explicit DigestAuthFilter(const std::map<std::string, std::string> &credentials);
  virtual void doFilter(const HttpRequestPtr &req,
                        FilterCallback &&cb,
                        FilterChainCallback &&ccb) override;
};
