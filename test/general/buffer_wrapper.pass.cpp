#include "support/test_config.h"

#include _PSTL_TEST_HEADER(execution)
#include _PSTL_TEST_HEADER(algorithm)
#include _PSTL_TEST_HEADER(numeric)
#include _PSTL_TEST_HEADER(iterator)

#include "support/utils.h"
#include <vector>


//This macro is required for the tests to work correctly in CI with tbb-backend.
#if TEST_DPCPP_BACKEND_PRESENT
struct test_buffer_wrapper
{
    template <typename Iterator, typename T>
    void operator()(Iterator begin, Iterator end, T* expected_data, std::size_t size)
    {   
        EXPECT_TRUE(begin == begin, "operator == returned false negative");
        EXPECT_TRUE(!(begin == begin + 1), "operator == returned false positive");

        EXPECT_TRUE(begin != begin + 1, "operator != returned false negative");
        EXPECT_TRUE(!(begin != begin), "operator != returned false positive");

        auto it = begin;

        EXPECT_TRUE(it == begin, "wrong effect of iterator's copy constructor");

        it = end;

        EXPECT_TRUE(it == end, "wrong effect of iterator's copy assignment operator");
        EXPECT_TRUE(it - size == begin, "wrong effect of iterator's operator - integer");
        EXPECT_TRUE(begin + size == end, "wrong effect of iterator's operator + integer");
        EXPECT_TRUE(end - begin == size, "wrong effect of iterator's operator - iterator");

        auto buf = begin.get_buffer();
        T* actual_data = sycl::host_accessor<T, 1, sycl::access_mode::read>(buf).get_pointer();
        EXPECT_TRUE(actual_data == expected_data, "wrong effect of iterator's method get_buffer");
    }
};
#endif

int32_t
main()
{
#if TEST_DPCPP_BACKEND_PRESENT

    std::size_t size = 1000;
    sycl::buffer<uint32_t> buf{size};
    test_buffer_wrapper test{};
    auto begin = sycl::host_accessor<uint32_t, 1, sycl::access_mode::read>(buf).get_pointer();

    test(oneapi::dpl::begin(buf), oneapi::dpl::end(buf), begin, size);
    test(oneapi::dpl::begin(buf, sycl::write_only), oneapi::dpl::end(buf, sycl::write_only), begin, size);
    test(oneapi::dpl::begin(buf, sycl::write_only, sycl::noinit), oneapi::dpl::end(buf, sycl::write_only, sycl::noinit), begin, size);
    test(oneapi::dpl::begin(buf, sycl::noinit), oneapi::dpl::end(buf, sycl::noinit), begin, size);

#endif
    return TestUtils::done(TEST_DPCPP_BACKEND_PRESENT);
}
