#include <webrpc/AbstractMethod.h>
#include <webrpc/Registry.h>

#include <gtest/gtest.h>


struct Method : public AbstractMethod
{
	Method()
		: AbstractMethod("Method")
	{}

	boost::optional<value_t> execute(const boost::optional<value_t> arg) override
	{
		return arg;
	}
};

TEST(Registry, ExistingKey)
{
    Registry registry;
    registry.emplace(std::make_pair("Echo", std::make_unique<Method>()));
    registry.emplace(std::make_pair("DateTime", std::make_unique<Method>()));
    registry.emplace(std::make_pair("GetSetValue", std::make_unique<Method>()));
    registry.emplace(std::make_pair("Revert", std::make_unique<Method>()));
    registry.emplace(std::make_pair("Shuffle", std::make_unique<Method>()));
    registry.emplace(std::make_pair("Sum", std::make_unique<Method>()));
    registry.emplace(std::make_pair("?", std::make_unique<Method>()));

    {
        const auto res = registry.find("Echo");
        ASSERT_FALSE(res == registry.cend());
        ASSERT_TRUE(res != registry.cend());
    }

    {
        const auto res = registry.find("?");
        ASSERT_TRUE(res != registry.cend());
    }

    {
        const auto res = registry.find("favicon.ico");
        ASSERT_TRUE(res == registry.cend());
    }

}
