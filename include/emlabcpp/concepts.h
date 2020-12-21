#include "emlabcpp/types.h"
#include <concepts>

#pragma once

namespace emlabcpp
{

template< typename T >
concept arithmetic = requires(T a, T b){
	{ a + b } -> std::convertible_to<T>;
	{ a - b } -> std::convertible_to<T>;
	{ a / b } -> std::convertible_to<T>;
	{ a * b } -> std::convertible_to<T>;
};

template< typename T >
concept assignment_arithmetic = requires(T a, T b){
	{a += b};
	{a -= b};
	{a /= b};
	{a *= b};
};

template< typename T >
concept get_cont = is_std_tuple_v<T>;

template< typename T >
// so, std::ranges::range is meh because it expects return of begin() being input_output_iterator, which has to be def.constructible
concept range_cont = 
	( requires (T a){ begin(a); } || requires (T a){ std::begin(a); } ) &&
	( requires (T a){ end(a); } || requires (T a){ std::end(a); } );

template< typename T >
concept container = range_cont<T> || get_cont<T>;

template< typename T >
concept back_pusshable = requires(T a, typename T::value_type b){
	{ a.push_back( b ) };
};

// TODO: test this
template< typename T >
concept referenceable_range = std::ranges::borrowed_range<T> || ( range_cont<T> && !std::is_rvalue_reference_v<T> );

template< typename T >
concept mappable_range = is_std_array_v<T> || requires(T a){ 
	{ a.push_back(*a.begin()) };
} || requires(T a ) {
	{ a.insert(*a.begin()) };
};

template< typename T >
concept static_sized = requires(T a ){
	{ static_size_v< T > } -> std::same_as<std::size_t>;
};

template< typename T, typename U >
concept summable = requires(T a, U b){
	{ a + b };
	{ a += b };
};

}
