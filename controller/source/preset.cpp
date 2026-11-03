namespace controller {
void ConfigurePreset() {
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
#ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
        spdlog::info("Debug mode enabled");
#endif // _DEBUG
}
} // namespace controller