#pragma once

#include <memory>
#include <string>

namespace optkit{
    class OPTKIT;
}

class OptkitAdapter
{
public:
	OptkitAdapter();
    ~OptkitAdapter();

	OptkitAdapter(const OptkitAdapter &) = delete;
	OptkitAdapter &operator=(const OptkitAdapter &) = delete;
	OptkitAdapter(OptkitAdapter &&) = delete;
	OptkitAdapter &operator=(OptkitAdapter &&) = delete;

	bool initialize(const std::string &session_name);
	void finalize();

	bool is_initialized() const;
	const std::string &last_error() const;
	const std::string &active_session_name() const;

private: 
	std::unique_ptr<optkit::OPTKIT> impl_;
	std::string last_error_;
	std::string active_session_name_;
};
