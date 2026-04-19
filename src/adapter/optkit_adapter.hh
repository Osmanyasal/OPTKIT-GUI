#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace optkit{
    class OPTKIT;
}

class OptkitAdapter
{
public:
    static bool initialize(const std::string &session_name);
    static void finalize();

    static bool is_initialized();
    static const std::string &last_error();
    static const std::string &active_session_name();

    static std::unordered_map<std::string, std::vector<std::string>> list_available_metrics();
    static std::unordered_map<std::string, std::vector<std::string>> list_available_events();

private: 
    static std::unique_ptr<optkit::OPTKIT> impl_;
    static std::string last_error_;
    static std::string active_session_name_;
};
