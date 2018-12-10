#include "doctest.h"
#include "type_name/type_name.hpp"

#define LOG(str) std::cout << str << std::endl
#define LOG_VALUE(var) std::cout << #var << " = " << var << std::endl

TEST_CASE("type_name_full_test")
{
    {
        // Standard types
        int v = 5;
        // The function type_name::full will add a reference
        // (since the arg is passed by universal reference (&&))
        REQUIRE_EQ(type_name::full(v),
        "int&"
        );
        // The macro will output the exact type
        REQUIRE_EQ(tn_type_name_full(v),
        "int"
        );
    }
    {
        // Reference
        int a = 5;
        int &v = a;
        REQUIRE_EQ(type_name::full(v),
        "int&"
        );
        REQUIRE_EQ(tn_type_name_full(v),
        "int&"
        );
    }
    {
        // Const reference
        int a = 5;
        const int &v = a;
        REQUIRE_EQ(type_name::full(v),
        "int const&"
        );
        REQUIRE_EQ(tn_type_name_full(v),
        "int const&"
        );
    }
    {
        // Pointer to const
        int a = 5;
        const int *v = &a;
        REQUIRE_EQ(type_name::full(v),
        "int const*&"
        );
        REQUIRE_EQ(tn_type_name_full(v),
        "int const*"
        );
    }
    {
        // Const pointer (but modifiable content)
        int a = 5;
        int * const v = &a;
        REQUIRE_EQ(type_name::full(v),
        "int* const&"
        );
        REQUIRE_EQ(tn_type_name_full(v),
        "int* const"
        );
    }
    {
        // Volatile
        volatile int v = 5;
        REQUIRE_EQ(type_name::full(v),
        "int volatile&"
        );
        REQUIRE_EQ(tn_type_name_full(v),
        "int volatile"
        );
    }
}


TEST_CASE("type_name_full_r_value_references")
{
    using TypenamePair = std::array<std::string, 2>;
    // r-value reference tests
    auto output_received_type = [](auto && v) -> TypenamePair {
        return { type_name::full<decltype(v)>(), type_name::full(v) };
    };
    auto require_eq_typename_pair = [](const TypenamePair & p1, const TypenamePair & p2 ) {
        REQUIRE_EQ(p1[0], p2[0]);
        REQUIRE_EQ(p1[1], p2[1]);
    };

    {
        // with a standard value
        int v = 5;
        require_eq_typename_pair(output_received_type(v),
        { "int&", "int&"}
        );
    }
    // with a reference
    {
        int a = 5;
        int &v = a;
        require_eq_typename_pair(output_received_type(v),
        { "int&", "int&"}
        );
    }
    // with a const reference
    {
        int a = 5;
        const int &v = a;
        require_eq_typename_pair(output_received_type(v),
        { "int const&", "int const&"}
        );
    }
    // with an r-value reference
    {
        std::string s("hello there, how are you. This is not a short string");
        require_eq_typename_pair(output_received_type(42),
        { "int&&", "int&" }
        );
    }

}


TEST_CASE("tn_type_name_full")
{
    {
        int v = 5;
        REQUIRE_EQ(
            tn_type_name_full(v),
            "int"
         );
    }
    {
        REQUIRE_EQ(
            tn_type_name_full(42),
            "int"
         );
    }
}


template<typename... Args>
struct TemplateClass
{
    static std::string full_type() {
        return type_name::full<Args...>();
    }
};


template<typename... Args>
void check_multiple_args(const std::string & expected)
{
    REQUIRE_EQ(
        type_name::full<Args...>(),
        expected
    );
    REQUIRE_EQ(
        TemplateClass<Args...>::full_type(),
        expected
    );
}


TEST_CASE("type_name_full_multiple")
{
    check_multiple_args<
         int, int const>(
        "int, int const");
    check_multiple_args<
         int&, int const& >(
        "int&, int const&"
    );
    check_multiple_args<
         int&&>(
        "int&&"
    );
    check_multiple_args<
         int&&>(
        "int&&"
    );
    check_multiple_args<
         int*, int const*, int* const >(
        "int*, int const*, int* const"
    );
}


template<typename... Args>
void check_multiple_args_fromvalues(const std::string & expected, Args... args)
{
    REQUIRE_EQ(
        type_name::full(args...),
        expected
    );
}


TEST_CASE("type_name_full_multiple_fromvalues")
{
    REQUIRE_EQ(
        type_name::full(1),
        "int"
    );
    REQUIRE_EQ(
        type_name::full(1, 1),
        "int, int"
    );
    REQUIRE_EQ(
        type_name::full(1, "hello"),
        "int, char [6] const&"
    );

    {
        // Three params and perfect forwarding -> fail !
        // This fails because it is on the third param
        // See possible leads:
        // http://www.cplusplus.com/reference/tuple/forward_as_tuple/
        // +
        // https://stackoverflow.com/questions/1198260/iterate-over-tuple
        int a = 5;
        int &v = a;
        int &v2 = a;
        int const& c = a;
        // REQUIRE_EQ(type_name::full(1, 2, c),
        // "int, int, int const&" --> "int, int, int" !!!
        // );
    }

}