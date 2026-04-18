#pragma once

#include "adapter/adapter.hh"

#include <string>

enum AppTheme
{
    AppTheme_Dark,
    AppTheme_White,
};

enum AppPage
{
    AppPage_Welcome,
    AppPage_RecentSessions,
    AppPage_CreateSession,
    AppPage_About,
};

struct SessionDraft
{
    int selected_template = 0;
    bool collect_cpu = true;
    bool collect_memory = true;
    bool collect_gpu = false;
    std::string session_name = "New Profiling Session";
    std::string target_binary = "./bin/my_workload";
    std::string target_arguments;
    bool post_mortem = true;
};

struct AppState
{
    AppTheme current_theme = AppTheme_Dark;
    AppPage current_page = AppPage_Welcome;
    bool show_settings_popup = false;
    bool open_target_binary_picker = false;
    char target_binary_picker_directory[1024] = ".";
    SessionDraft session_draft;
    OptkitAdapter optkit;
};