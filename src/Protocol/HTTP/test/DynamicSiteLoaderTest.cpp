#include "DynamicSiteLoader.h"
#include <gtest/gtest.h>
#include "coverage/ThorMock.h"
#include "ThorsNisseCoreService/Server.h"
#include "ThorsNisseCoreService/Handler.h"

using ThorsAnvil::Nisse::Core::Service::Server;
using ThorsAnvil::Nisse::Protocol::HTTP::DynamicSiteLoader;

TEST(DynamicSiteLoaderTest, Construct)
{
    Server              server;
    DynamicSiteLoader   loader(server);
}
TEST(DynamicSiteLoaderTest, LoadLib)
{
    Server              server;
    DynamicSiteLoader   loader(server);

    loader.load("TestLib/Loadable/Loadable.dylib", 80406, "test.com", "");
}
TEST(DynamicSiteLoaderTest, UnLoadLib)
{
    Server              server;
    DynamicSiteLoader   loader(server);

    loader.load("TestLib/Loadable/Loadable.dylib", 80406, "test.com", "");
    loader.unload(80406, "test.com", "");
}


TEST(DynamicSiteLoaderTest, LoadLibFailsAlreadyLoaded)
{
    Server              server;
    DynamicSiteLoader   loader(server);
    loader.load("TestLib/Loadable/Loadable.dylib", 80406, "test.com", "");

    auto result = loader.load("TestLib/NotThere/NotThere.dylib", 80406, "test.com", "");

    ASSERT_FALSE(std::get<0>(result));
}
TEST(DynamicSiteLoaderExceptionTest, LoadLibFailsToLoad)
{
    Server              server;
    DynamicSiteLoader   loader(server);
    auto doTest = [&loader](){loader.load("TestLib/NotThere/NotThere.dylib", 80406, "test.com", "");};

    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}
TEST(DynamicSiteLoaderExceptionTest, LoadLibFailsToFindGetFunc)
{
    Server              server;
    DynamicSiteLoader   loader(server);
    auto doTest = [&loader](){loader.load("TestLib/NoGet/NoGet.dylib", 80406, "test.com", "");};

    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}
TEST(DynamicSiteLoaderExceptionTest, LoadLibFailsGetFuncReturnsNull)
{
    Server              server;
    DynamicSiteLoader   loader(server);
    auto doTest = [&loader](){loader.load("TestLib/GetNull/GetNull.dylib", 80406, "test.com", "");};

    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}
TEST(DynamicSiteLoaderExceptionTest, UnLoadLibFailsNotThere)
{
    Server              server;
    DynamicSiteLoader   loader(server);
    loader.load("TestLib/Loadable/Loadable.dylib", 80406, "test.com", "");
    auto result = loader.unload(80406, "bad.com", "");

    ASSERT_FALSE(std::get<0>(result));
}
TEST(DynamicSiteLoaderExceptionTest, UnLoadLibFailsDlCloseFail)
{
    MOCK_SYS(dlclose, [](void*){return -1;});

    Server              server;
    DynamicSiteLoader   loader(server);
    loader.load("TestLib/Loadable/Loadable.dylib", 80406, "test.com", "");
    auto doTest = [&loader](){loader.unload(80406, "test.com", "");};

    ASSERT_THROW(
        doTest(),
        std::runtime_error
    );
}

