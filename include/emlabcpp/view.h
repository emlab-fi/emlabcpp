#include "emlabcpp/types.h"

#ifdef EMLABCPP_USE_STREAMS
#include <ostream>
#endif

#pragma once

namespace emlabcpp
{

/// Generic class to represent view of some container.
///
/// The view stores iterators to the input container and acts as container.
///
/// Note: is_view_v<T> can be used to detect if T is view<I>
///
template < typename Iterator >
class view
{
        Iterator begin_;
        Iterator end_;

public:
        /// standard public usings for container
        using value_type =
            typename std::remove_reference_t< decltype( *std::declval< Iterator >() ) >;
        using reverse_iterator = std::reverse_iterator< Iterator >;
        using iterator         = Iterator;
        using difference_type  = typename std::iterator_traits< Iterator >::difference_type;
        using size_type        = std::size_t;

        constexpr view() = default;

        /// constructor from Container, uses begin/end of the container
        constexpr view( range_container auto& cont )
          : begin_( std::begin( cont ) )
          , end_( std::end( cont ) )
        {
        }

        /// constructor from Container, uses begin/end of the container
        constexpr view( const range_container auto& cont )
          : begin_( std::begin( cont ) )
          , end_( std::end( cont ) )
        {
        }

        /// constructor from the iterators that should internally be stored
        constexpr view( Iterator begin, Iterator end )
          : begin_( std::move( begin ) )
          , end_( std::move( end ) )
        {
        }

        template < std::convertible_to< Iterator > OtherIterator >
        constexpr view( view< OtherIterator > other )
          : begin_( other.begin() )
          , end_( other.end() )
        {
        }

        /// Start of the dataset iterator
        [[nodiscard]] constexpr Iterator begin() const
        {
                return begin_;
        }

        /// Past the end iterator
        [[nodiscard]] constexpr Iterator end() const
        {
                return end_;
        }

        /// Access to i-th element in the range, expects Iterator::operator+
        [[nodiscard]] constexpr decltype( auto ) operator[]( size_type i ) const
        {
                return *( begin_ + static_cast< difference_type >( i ) );
        }

        /// Returns iterator to the last element that goes in reverse
        [[nodiscard]] constexpr reverse_iterator rbegin() const
        {
                return reverse_iterator{ end_ };
        }

        /// Returns iterator to the element before first element, that can go in
        /// reverse
        [[nodiscard]] constexpr reverse_iterator rend() const
        {
                return reverse_iterator{ begin_ };
        }

        /// Size of the view over dataset uses std::distance() to tell the size
        [[nodiscard]] constexpr size_type size() const
        {
                return static_cast< std::size_t >( std::distance( begin(), end() ) );
        }

        /// View is empty if both iterators are equal
        [[nodiscard]] constexpr bool empty() const
        {
                return begin() == end();
        }

        /// Returns first value of the range
        [[nodiscard]] constexpr const value_type& front() const
        {
                return *begin_;
        }

        /// Returns last value of the range
        [[nodiscard]] constexpr const value_type& back() const
        {
                return *std::prev( end_ );
        }
};

template < typename IteratorLh, typename IteratorRh >
constexpr bool operator==( const view< IteratorLh >& lh, const view< IteratorRh >& rh )
{
        if ( lh.size() != rh.size() ) {
                return false;
        }

        IteratorLh lhiter = lh.begin();
        IteratorRh rhiter = rh.begin();

        for ( ; lhiter != lh.end(); ++lhiter, ++rhiter ) {
                if ( *lhiter != *rhiter ) {
                        return false;
                }
        }
        return true;
}

template < typename IteratorLh, typename IteratorRh >
constexpr bool operator!=( const view< IteratorLh >& lh, const view< IteratorRh >& rh )
{
        return !( lh == rh );
}

/// The container deduction guide uses iterator_of_t
template < range_container Container >
view( Container& cont ) -> view< iterator_of_t< Container > >;

/// Support for our deduction guide to types - is_view_v
template < typename Iter >
struct impl::is_view< view< Iter > > : std::true_type
{
};

/// Creates view over 'n' items of dataset starting at 'begin'
/// This does not check validity of the range!
template < typename Iter >
constexpr view< Iter > view_n( Iter begin, std::size_t n )
{
        auto end = std::next(
            begin, static_cast< typename std::iterator_traits< Iter >::difference_type >( n ) );
        return view< Iter >{ std::move( begin ), end };
}

/// Creates the view over over Container, where we ignore first r*size/2 items
/// and last r*size/2 items. This can be used to get the dataset without
/// first/last 5% for example, by using r=0.1
template < range_container Container >
constexpr view< iterator_of_t< Container > > trim_view( Container& cont, float r )
{
        std::size_t step = cont.size() * ( 1.f - r ) / 2.f;
        return { cont.begin() + step, cont.end() - step };
}

/// Returns view to the Container in reverse order.
constexpr auto reversed( referenceable_container auto&& container )
    -> view< decltype( container.rbegin() ) >
{
        return { container.rbegin(), container.rend() };
}

#ifdef EMLABCPP_USE_STREAMS
// Output operator for the view, uses comma to separate the items in the view.
template < typename Iterator >
inline std::ostream& operator<<( std::ostream& os, const view< Iterator >& output )
{
        using value_type = decltype( *std::declval< Iterator >() );
        bool first       = true;
        for ( const value_type& item : output ) {
                if ( !first ) {
                        os << ',';
                }
                os << item;
                first = false;
        }
        return os;
}
#endif

}  // namespace emlabcpp
