#include <controller/injector.hpp>

#include <gtest/gtest.h>

TEST(InjectorTest, Inject) {
    controller::Injector injector;
    injector.Inject();
}