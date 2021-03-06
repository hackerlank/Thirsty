/*
 * Copyright 2015 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Uri.h"
#include <ctype.h>
#include <regex>
#include "Conv.h"

namespace {

std::string submatch(const std::cmatch& m, size_t idx) 
{
    auto& sub = m[idx];
    return std::string(sub.first, sub.second);
}

void toLower(std::string& s) 
{
    for (auto& c : s)
    {
        c = tolower(c);
    }
}

}  // namespace

Uri::Uri(StringPiece str) : port_(0) 
{
    static const std::regex uriRegex(
        "([a-zA-Z][a-zA-Z0-9+.-]*):"  // scheme:
        "([^?#]*)"                    // authority and path
        "(?:\\?([^#]*))?"             // ?query
        "(?:#(.*))?");                // #fragment
    static const std::regex authorityAndPathRegex("//([^/]*)(/.*)?");

    std::cmatch match;
    if (UNLIKELY(!std::regex_match(str.begin(), str.end(), match, uriRegex))) {
        throw std::invalid_argument(to<std::string>("invalid URI ", str));
    }

    scheme_ = submatch(match, 1);
    toLower(scheme_);

    StringPiece authorityAndPath(match[2].first, match[2].second);
    std::cmatch authorityAndPathMatch;
    if (!std::regex_match(authorityAndPath.begin(),
        authorityAndPath.end(),
        authorityAndPathMatch,
        authorityAndPathRegex)) {
        // Does not start with //, doesn't have authority
        path_ = authorityAndPath.str();
    }
    else {
        static const std::regex authorityRegex(
            "(?:([^@:]*)(?::([^@]*))?@)?"  // username, password
            "(\\[[^\\]]*\\]|[^\\[:]*)"     // host (IP-literal (e.g. '['+IPv6+']',
            // dotted-IPv4, or named host)
            "(?::(\\d*))?");               // port

        auto authority = authorityAndPathMatch[1];
        std::cmatch authorityMatch;
        if (!std::regex_match(authority.first,
            authority.second,
            authorityMatch,
            authorityRegex)) {
            throw std::invalid_argument(
                to<std::string>("invalid URI authority ",
                StringPiece(authority.first, authority.second)));
        }

        StringPiece port(authorityMatch[4].first, authorityMatch[4].second);
        if (!port.empty()) {
            port_ = to<uint16_t>(port);
        }

        username_ = submatch(authorityMatch, 1);
        password_ = submatch(authorityMatch, 2);
        host_ = submatch(authorityMatch, 3);
        path_ = submatch(authorityAndPathMatch, 2);
    }

    query_ = submatch(match, 3);
    fragment_ = submatch(match, 4);
}

std::string Uri::authority() const
{
    std::string result;

    // Port is 5 characters max and we have up to 3 delimiters.
    result.reserve(host().size() + username().size() + password().size() + 8);

    if (!username().empty() || !password().empty()) {
        result.append(username());

        if (!password().empty()) {
            result.push_back(':');
            result.append(password());
        }

        result.push_back('@');
    }

    result.append(host());

    if (port() != 0) {
        result.push_back(':');
        toAppend(&result, port());
    }

    return result;
}

std::string Uri::str() const 
{
    std::string str;
    toAppend(&str, scheme_, "://");
    if (!password_.empty()) {
        toAppend(&str, username_, ":", password_, "@");
    }
    else if (!username_.empty()) {
        toAppend(&str, username_, "@");
    }
    toAppend(&str, host_);
    if (port_ != 0) {
        toAppend(&str, ":", port_);
    }
    toAppend(&str, path_);
    if (!query_.empty()) {
        toAppend(&str, "?", query_);
    }
    if (!fragment_.empty()) {
        toAppend(&str, "#", fragment_);
    }
    return str;
}

std::string Uri::hostname() const 
{
    if (host_.size() > 0 && host_[0] == '[') {
        // If it starts with '[', then it should end with ']', this is ensured by
        // regex
        return host_.substr(1, host_.size() - 2);
    }
    return host_;
}

const std::vector<std::pair<std::string, std::string>>& Uri::getQueryParams() 
{
    if (!query_.empty() && queryParams_.empty()) {
        // Parse query string
        static const std::regex queryParamRegex(
            "(^|&)" /*start of query or start of parameter "&"*/
            "([^=&]*)=?" /*parameter name and "=" if value is expected*/
            "([^=&]*)" /*parameter value*/
            "(?=(&|$))" /*forward reference, next should be end of query or
                          start of next parameter*/);
        std::cregex_iterator paramBeginItr(
            query_.data(), query_.data() + query_.size(), queryParamRegex);
        std::cregex_iterator paramEndItr;
        for (auto itr = paramBeginItr; itr != paramEndItr; itr++) {
            if (itr->length(2) == 0) {
                // key is empty, ignore it
                continue;
            }
            queryParams_.emplace_back(
                std::string((*itr)[2].first, (*itr)[2].second), // parameter name
                std::string((*itr)[3].first, (*itr)[3].second) // parameter value
                );
        }
    }
    return queryParams_;
}
