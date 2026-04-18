#pragma once

#include <memory>
#include <string>

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

private: 
    static std::unique_ptr<optkit::OPTKIT> impl_;
    static std::string last_error_;
    static std::string active_session_name_;
};
