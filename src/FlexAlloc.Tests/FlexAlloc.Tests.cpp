#include "pch.h"
#include "CppUnitTest.h"
#include "FlexAlloc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FlexAllocTests
{
	TEST_CLASS(FlexAllocTests)
	{
	public:

		TEST_METHOD(TestAllocate)
		{
			size_t size = 1024;
			size_t alignment = 16;
			memory_block block = allocate(size, alignment);

			Assert::IsNotNull(block.ptr, L"Memory allocation failed.");
			Assert::IsTrue(block.size >= size, L"Allocated size is less than requested.");

			deallocate(block, alignment);
		}

		TEST_METHOD(TestDeallocate)
		{
			size_t size = 1024;
			size_t alignment = 16;
			memory_block block = allocate(size, alignment);
			Assert::IsNotNull(block.ptr, L"Memory allocation failed.");

			deallocate(block, alignment);

			// Check if memory is successfully deallocated by trying to reallocate it
			memory_block block2 = allocate(size, alignment);
			Assert::IsNotNull(block2.ptr, L"Memory reallocation failed after deallocation.");

			deallocate(block2, alignment);
		}

		TEST_METHOD(TestTryExpand)
		{
			size_t size = 1024;
			size_t new_size = 2048;
			size_t alignment = 16;
			memory_block block = allocate(size, alignment);
			Assert::IsNotNull(block.ptr, L"Memory allocation failed.");

			if (try_expand(&block, new_size)) {
				Assert::IsTrue(block.size >= new_size, L"Expanded size is less than requested.");
			}
			else {
				Assert::Fail(L"Memory expansion failed.");
			}

			deallocate(block, alignment);
		}
	};
}
