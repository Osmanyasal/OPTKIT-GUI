#include "adapter/optkit_adapter.hh"

#include <optkit.hh>

#include <exception>

std::unique_ptr<optkit::OPTKIT> OptkitAdapter::impl_;
std::string OptkitAdapter::last_error_;
std::string OptkitAdapter::active_session_name_;

bool OptkitAdapter::initialize(const std::string &session_name)
{
    if(OptkitAdapter::is_initialized())
        return false;

    const std::string execution_name = session_name.empty() ? std::string("optkit-gui-session") : session_name;

    if (impl_)
        finalize();
    try
    {
        optkit::OPTKIT_CONFIG config(true, execution_name, true);
        if (!impl_)
            impl_ = std::make_unique<optkit::OPTKIT>(config);
        active_session_name_ = execution_name;
        last_error_.clear();
        return true;
    }
    catch (const std::exception &error)
    {
        if (impl_)
            impl_.reset();
        active_session_name_.clear();
        last_error_ = error.what();
        return false;
    }
    catch (...)
    {
        if (impl_)
            impl_.reset();
        active_session_name_.clear();
        last_error_ = "Unknown error while initializing OPTKIT.";
        return false;
    }
}

void OptkitAdapter::finalize()
{
    if (impl_)
        impl_.reset();
    active_session_name_.clear();
    last_error_.clear();
}

bool OptkitAdapter::is_initialized()
{
    return impl_ != nullptr;
}

const std::string &OptkitAdapter::last_error()
{
    return last_error_;
}

const std::string &OptkitAdapter::active_session_name()
{
    return active_session_name_;
}