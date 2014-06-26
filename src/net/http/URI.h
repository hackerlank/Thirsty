/*
 * Copyright 2014 Facebook, Inc.
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

#ifndef FOLLY_URI_H_
#define FOLLY_URI_H_

#include <string>
#include "core/Range.h"

/**
 * Class representing a URI.
 *
 * Consider http://www.facebook.com/foo/bar?key=foo#anchor
 *
 * The URI is broken down into its parts: scheme ("http"), authority
 * (ie. host and port, in most cases: "www.facebook.com"), path
 * ("/foo/bar"), query ("key=foo") and fragment ("anchor").  The scheme is
 * lower-cased.
 *
 * If this Uri represents a URL, note that, to prevent ambiguity, the component
 * parts are NOT percent-decoded; you should do this yourself with
 * uriUnescape() (for the authority and path) and uriUnescape(...,
 * UriEscapeMode::QUERY) (for the query, but probably only after splitting at
 * '&' to identify the individual parameters).
 */
class Uri {
 public:
  /**
   * Parse a Uri from a string.  Throws std::invalid_argument on parse error.
   */
  explicit Uri(StringPiece str);

  const std::string& scheme() const { return scheme_; }
  const std::string& username() const { return username_; }
  const std::string& password() const { return password_; }
  /**
   * Get host part of URI. If host is an IPv6 address, square brackets will be
   * returned, for example: "[::1]".
   */
  const std::string& host() const { return host_; }
  /**
   * Get host part of URI. If host is an IPv6 address, square brackets will not
   * be returned, for exmaple "::1"; otherwise it returns the same thing as
   * host().
   *
   * hostname() is what one needs to call if passing the host to any other tool
   * or API that connects to that host/port; e.g. getaddrinfo() only understands
   * IPv6 host without square brackets
   */
  std::string hostname() const;
  uint16_t port() const { return port_; }
  const std::string& path() const { return path_; }
  const std::string& query() const { return query_; }
  const std::string& fragment() const { return fragment_; }

  std::string authority() const;

  std::string str() const;

  void setPort(uint16_t port) {port_ = port;}

 private:
  std::string   scheme_;
  std::string   username_;
  std::string   password_;
  std::string   host_;
  uint16_t      port_;
  std::string   path_;
  std::string   query_;
  std::string   fragment_;
};


#endif /* FOLLY_URI_H_ */
