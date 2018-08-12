/**
 * \file info.cpp
 *
 * \author Adam Marshall (ih8celery)
 *
 * \brief parse command line arguments/options
 */
#include "info.h"

namespace cli {
  std::optional<std::string> Info::find(const std::string& name) const {
    opt_data_t::const_iterator iter = this->data.find(name);

    if (iter == this->data.cend()) {
      return std::nullopt;
    }

    return std::make_optional(iter->second);
  }

  std::optional<std::vector<std::string>> Info::find_all(const std::string& name) const {
    const auto rng = data.equal_range(name);

    std::vector<std::string> results;
    for (auto start = rng.first; start != rng.second; start++) {
      results.push_back(start->second);
    }

    if (results.empty()) {
      return std::nullopt;
    }

    return std::make_optional(results);
  }

  opt_data_t::size_type Info::count(const std::string& name) const {
    return data.count(name);
  }

  bool Info::has(const std::string& name) const {
    return (data.find(name) != data.cend());
  }

  bool Info::has_command(const std::string& name) const {
    return (this->commands.find(name) != this->commands.cend());
  }
}
