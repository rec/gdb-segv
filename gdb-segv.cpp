#include <functional>

struct Data {
  int member;
};

/**
   We're interested in writing an alias template to get the MemberType: to take
   a base type named T, and a pointer-to-member type named PTM, and return
   the type of that member in T.

   In specific, we're going to try take the struct Data and member &Data::member
   and get the type int.

   There are two nearly identical mechanisms to do it, neither of which compile
   under clang.  Under g++, both compile and run correctly, but in one case gdb
   will crash when trying to load this binary.

   (g++ 4.8.5, clang 3.4, gdb 7.7.1).
*/

/** Here's the the first mechanism - making an alias directly. */
template <class PTM, class T>
using MemberTypeAlias = decltype((T()).*(PTM()));

/** The second mechanism uses a dependent type.
    This turns out subtly problematic in one case... */
template <class PTM, class T>
struct Dep {
    using MemberType = decltype((T()).*(PTM()));
};

template <class PTM, class T>
using MemberTypeDep = typename Dep<PTM, T>::MemberType;


/** It turns out that one-argument generic functions trigger the behavior, but
    zero argument generic functions do not. */
template <class T> using Func0 = std::function<void()>;
template <class T> using Func1 = std::function<void(T const&)>;

// We've set everything up, now let's make some empty functions!

template <class T, class PTM> void trivial(PTM) {}

template <class T, class PTM>
void alias0(PTM, Func0<MemberTypeAlias<PTM, T>>) {}

template <class T, class PTM>
void struct0(PTM, Func0<MemberTypeDep<PTM, T>>) {}

template <class T, class PTM>
void alias1(PTM, Func1<MemberTypeAlias<PTM, T>>) {}  // DANGER.

template <class T, class PTM>
void struct1(PTM, Func1<MemberTypeDep<PTM, T>>) {}

int main() {
    trivial<Data>(&Data::member);

    /** So far, so good. Using either gcc or clang, everything above here
        compiles and runs both under gdb and as a standalone executable. */
#ifndef CLANG_FAILURE_BLOCK

    /** This next block of code won't compile in clang - it gives a "candidate
        template ignored: substitution failure" error - but it compiles and runs
        fine with g++. */
    struct0<Data>(&Data::member, {});
    alias0<Data>(&Data::member, {});
    struct1<Data>(&Data::member, {});

#ifndef GDB_FAILURE_BLOCK

    /** If this ONE line of code is enabled, the executable builds and no
        warnings with g++ - but trying to load that executable into gdb results
        in a segmentation fault! */
    alias1<Data>(&Data::member, {});

#endif  // GDB_FAILURE_BLOCK
#endif  // CLANG_FAILURE_BLOCK

    return 0;
}
