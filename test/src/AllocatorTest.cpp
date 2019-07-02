#include <webrpc/detail/fields_alloc.h>

#include <gtest/gtest.h>

namespace webrpctest
{

TEST(Allocator, Ctor)
{
	fields_alloc<char> alloc{1024};
	alloc.~fields_alloc();
}

}