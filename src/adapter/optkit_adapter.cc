#include "adapter/optkit_adapter.hh"
#include <optkit.hh>
#include <exception>
#include <iostream>
std::unique_ptr<optkit::OPTKIT> OptkitAdapter::impl_;
std::string OptkitAdapter::last_error_;
std::string OptkitAdapter::active_session_name_;

bool OptkitAdapter::initialize(const std::string &session_name)
{
    if (OptkitAdapter::is_initialized())
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

std::unordered_map<std::string, std::vector<std::string>> OptkitAdapter::list_available_metrics()
{

    std::unordered_map<std::string, std::vector<std::string>> metrics_by_category;

    metrics_by_category["cpu_performance"] = optkit::metrics::performance::cpu_metrics::get_all_metrics();
    metrics_by_category["cpu_energy"] = optkit::metrics::energy::cpu_metrics::get_all_metrics();
    metrics_by_category["gpu_energy"] = optkit::metrics::energy::gpu_metrics::get_all_metrics();
    metrics_by_category["gpu_performance"] = optkit::metrics::performance::gpu_metrics::get_all_metrics();
    metrics_by_category["disk"] = optkit::metrics::disk::core_metrics::get_all_metrics();

    return metrics_by_category;
}
std::unordered_map<std::string, std::vector<std::string>> OptkitAdapter::list_available_events()
{
    std::unordered_map<std::string, std::vector<std::string>> metrics_by_category;
 
    metrics_by_category["cpu_core_events"] = optkit::metrics::performance::cpu_get_supported_core_events();
    metrics_by_category["cpu_native_events"] = optkit::metrics::performance::cpu_get_native_events();

    return metrics_by_category;
}