#include <lghubex/app/injector.hpp>

int main() {
    try {
        lghubex::Injector injector;
        injector.Inject();
    } catch (std::exception& e) {
        spdlog::error("Exception caught: {}.", e.what());
    }
    return 0;
}