#pragma once

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
    char session_name[128] = "New Profiling Session";
    char target_binary[1024] = "./bin/my_workload";
    char target_arguments[512] = "";
};

struct AppState
{
    AppTheme current_theme = AppTheme_Dark;
    AppPage current_page = AppPage_Welcome;
    bool show_settings_popup = false;
    bool open_target_binary_picker = false;
    char target_binary_picker_directory[1024] = ".";
    SessionDraft session_draft;
};