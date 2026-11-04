#include <lghubex/injector.hpp>

#include <gtest/gtest.h>

TEST(InjectorTest, Inject) {
    lghubex::Injector injector;
    injector.Inject();
}