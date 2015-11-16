#include <cinttypes>


typedef int64_t IntType;

template<class T> struct Eval {
    typedef T value;
};

template<char... Chars> class St {};

template<IntType value> struct Int {
    //static const IntType intValue = value;
};

template<class A, class B> struct Add {};
template<IntType a, IntType b> struct Eval<Add<Int<a>, Int<b> > > {
    typedef Int<a + b> value;
};
template<class A, class B> struct Eval<Add<A,B> > {
    typedef typename Eval<Add<typename Eval<A>::value, typename Eval<B>::value> >::value value;
};


template<class A, class B> struct Sub {};
template<IntType a, IntType b> struct Eval<Sub<Int<a>, Int<b> > > {
    typedef Int<a - b> value;
};
template<class A, class B> struct Eval<Sub<A,B> > {
    typedef typename Eval<Sub<typename Eval<A>::value, typename Eval<B>::value> >::value value;
};


template<class A, class B> struct Mul {};
template<IntType a, IntType b> struct Eval<Mul<Int<a>, Int<b> > > {
    typedef Int<a * b> value;
};
template<class A, class B> struct Eval<Mul<A,B> > {
    typedef typename Eval<Mul<typename Eval<A>::value, typename Eval<B>::value> >::value value;
};


template<class A, class B> struct Div {};
template<IntType a, IntType b> struct Eval<Div<Int<a>, Int<b> > > {
    typedef Int<a / b> value;
};
template<class A, class B> struct Eval<Div<A,B> > {
    typedef typename Eval<Div<typename Eval<A>::value, typename Eval<B>::value> >::value value;
};


template<class A> struct Neg {};
template<IntType a> struct Eval<Neg<Int<a> > > {
    typedef Int<-a> value;
};
template<class A> struct Eval<Neg<A> > {
    typedef typename Eval<Neg<typename Eval<A>::value > >::value value;
};


typedef Int<1> T;
typedef Int<0> F;

template<class X> struct Bln {};
template<class X> struct BlnV {
    typedef T value;
};
template<> struct BlnV<F> {
    typedef F value;
};
template<> struct BlnV<St<> > {
    typedef F value;
};

template<class X> struct Eval<Bln<X> > {
    typedef typename BlnV<typename Eval<X>::value>::value value;
};



template<class... > struct If {};

template<class Cond, class TVal, class... Else> struct Eval<If<Cond,TVal,Else...> > {
    typedef typename Eval<If<typename Eval<Bln<Cond> >::value,TVal, Else...> >::value value;
};
template<class TVal, class... Else> struct Eval<If<T,TVal,Else...> > {
    typedef typename Eval<TVal>::value value;
};
template<class TVal, class... Else> struct Eval<If<F,TVal,Else...> > {
    typedef typename Eval<If<Else...> >::value value;
};
template<class ElseVal> struct Eval<If<ElseVal> > {
    typedef typename Eval<ElseVal>::value value;
};



template<class Expr> struct Fun {};

template<size_t N, size_t Level = 0> struct Arg {}; //lambda argument

template<class...> struct TypePack {};

template<class Head_, class... Others> struct TypePack<Head_, Others...> {
    typedef Head_ Head;
    typedef TypePack<Others...> Tail;
};
template<class Pack, size_t N> struct TypePackNth {
    typedef typename TypePackNth<typename Pack::Tail, N-1>::value value;
};
template<class Pack> struct TypePackNth<Pack, 0> {
    typedef typename Pack::Head value;
};


template<class Expr, size_t ArgL, class Args> struct SubArgs {
    typedef Expr value;
};
template<size_t ArgN, size_t ArgL, class Args> struct SubArgs<Arg<ArgN,ArgL>,ArgL,Args> {
    typedef typename TypePackNth<Args, ArgN>::value value;
};


template<size_t ArgL, class Args, template<class...> class Fn, class... FnArgs> struct SubArgsF;
template<size_t ArgL, class Args, template<class...> class Fn, class FnArg0, class... FnArgs1>
struct SubArgsF<ArgL, Args, Fn, FnArg0, FnArgs1...> {
    template<class... FnArgs1_> using Fn1 = Fn<typename SubArgs<FnArg0, ArgL, Args>::value, FnArgs1_...>;
    typedef typename SubArgsF<ArgL, Args, Fn1, FnArgs1...>::value value;
};
/* had to special case SubArgsF with one fn arg because g++ assumed parameter pack contained at least
   one value in, e.g., template<class A, class...B> using Fn1 = Neg<B...>;  */
template<size_t ArgL, class Args, template<class...> class Fn, class FnArg0>
struct SubArgsF<ArgL, Args, Fn, FnArg0> {
    typedef Fn<typename SubArgs<FnArg0,ArgL,Args>::value> value;
};
template<size_t ArgL, class Args, template<class...> class Fn>
struct SubArgsF<ArgL, Args, Fn> {
    typedef Fn<> value;
};

template<size_t ArgL, class Args, template<class...> class Fn, class... FnArgs>
struct SubArgs<Fn<FnArgs...>,ArgL,Args> {
    typedef typename SubArgsF<ArgL,Args,Fn,FnArgs...>::value value;
};
template<size_t ArgL, class Args, class Expr> struct SubArgs<Fun<Expr>,ArgL,Args> {
    typedef Fun<typename SubArgs<Expr, ArgL + 1, Args>::value> value;
};


template<class F, class... Args> struct Ap {};

template<class Expr, class... Args> struct Eval<Ap<Fun<Expr>,Args...> > {
    typedef typename Eval<typename SubArgs<Expr, 0, TypePack<Fun<Expr>, Args...> >::value>::value value;
};
template<class A, class... Args> struct Eval<Ap<A, Args...> > {
    typedef typename Eval<Ap<typename Eval<A>::value, Args...> >::value value;
};


template<class A, class B> struct EqN {};  //Equal no eval
template<class A, class B> struct Eval<EqN<A,B> > {
    typedef F value;
};
template<class A> struct Eval<EqN<A,A> > {
    typedef T value;
};

template<class A, class B> struct Eq {};
template<class A, class B> struct Eval<Eq<A,B> > {
    typedef typename Eval<EqN<typename Eval<A>::value, typename Eval<B>::value> >::value value;
};



template<class A, class B> struct And {};
template<class A, class B> struct Eval<And<A, B> > {
    typedef typename Eval<If<A, Bln<B>, F> >::value value;
};

template<class N> struct Abs {};
template<IntType n> struct Eval<Abs<Int<n> > > {
    typedef Int<n < 0 ? -n : n> value;
};
template<class N> struct Eval<Abs<N> > {
    typedef typename Eval<Abs<Eval<N> > >::value value;
};

template<class A> struct Str {};

template<char... s> struct Eval<Str<St<s...> > > {
    typedef St<s...> value;
};

template<class A> struct Eval<Str<A> > {
    typedef typename Eval<Str<typename Eval<A>::value> >::value value;
};

template<class A, class B> struct Cat {};

template<class> struct Cat_ {};
template<char... s> struct Cat_<St<s...> > {
    template<class> struct cat {};
    template<char... t> struct cat<St<t...> > {
        typedef St<s..., t...> value;
    };
};

template<class A, class B> struct Eval<Cat<A, B> > {
    typedef typename Cat_<typename Eval<Str<A> >::value>::template cat<typename Eval<Str<B> >::value>::value value;
};

typedef St<' '> SP;
typedef St<'\n'> LF;

template<IntType n> using I = Int<n>;
template<size_t N, size_t Level = 0> using A = Arg<N, Level>;

template<IntType n> struct Eval<Str<Int<n> > > {
    typedef typename Eval<If<
        Int<n < 0>,
        Cat<St<'-'>, Int<-n> >,
        Int<n < 10>,
        St<n%10 + '0'>,
        Cat< Int<n / 10>, St<n % 10 + '0'> >
    > >::value value;
};

template<class A, class B> struct lt {};
template<class A, class B> struct Eval<lt<A,B> > {
    typedef typename Eval<lt<typename Eval<A>::value, typename Eval<B>::value > >::value value;
};
template<IntType a, IntType b> struct Eval<lt<Int<a>, Int<b> > > {
    typedef Int<a < b> value;
};

template<class... Elems> struct Lst {};
template<class List> struct Hd {};
template<class List> struct Tl {};
template<class List> struct mt {};
template<class... Lists> struct lj {};
template<class List, class N> struct Nth {};

template<class A> struct Eval<Hd<A> > {
    typedef typename Hd<typename Eval<A>::value>::value value;
};
template<class Head, class... Tails> struct Eval<Hd<Lst<Head, Tails...> > > {
    typedef typename Eval<Head>::value value;
};
template<class L0, class... L1> struct Eval<Hd<lj<L0, L1...> > > {
    typedef typename Eval<Hd<If<mt<L0>, lj<L1...>, L0> > >::value value;
};

template<class A> struct Eval<Tl<A> > {
    typedef typename Tl<typename Eval<A>::value>::value value;
};
template<class Head, class... Tails> struct Eval<Tl<Lst<Head, Tails...> > > {
    typedef Lst<Tails...> value;
};
template<class L0, class... L1> struct Eval<Tl<lj<L0, L1...> > > {
    typedef typename Eval<If<mt<L0>, Tl<lj<L1...> >, lj<Tl<L0>, L1...> > >::value value;
};

template<class A> struct Eval<mt<A> > {
    typedef typename mt<typename Eval<A>::value>::value value;
};
template<class Head, class... Tails> struct Eval<mt<Lst<Head, Tails...> > > {
    typedef F value;
};
template<> struct Eval<mt<Lst<> > > {
    typedef T value;
};
template<class L0, class... L1> struct Eval<mt<lj<L0,L1...> > > {
    typedef typename Eval<And<mt<L0>, mt<lj<L1...> > > >::value value;
};
template<> struct Eval<mt<lj<> > > {
    typedef T value;
};

template<class List, class N> struct Eval<Nth<List, N> > {
    typedef typename Nth<List, typename Eval<N>::value>::value value;
};
template<class List, size_t n> struct Eval<Nth<List, Int<n> > > {
    typedef typename Eval<Nth<typename Eval<Tl<List> >::value, Int<n - 1> > >::value value;
};
template<class List> struct Eval<Nth<List, Int<0> > > {
    typedef typename Eval<Hd<List> >::value value;
};


template<class List, class Delim = St<> > class Jn {};

