#include "%s"

typedef Fun<
    %s
> program;

typedef St<
    %s
> input;

typedef typename Eval<Str<Ap<program, input> > >::value output;

#include <string>
#include <iostream>

template<char... out> void print(St<out...>) {
    std::string s{out...};
    std::cout << s;
}

int main() {
    print(output{});
    return 0;
}
