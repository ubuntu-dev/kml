/***************************************************************************
 *  The Kernel-Machine Library                                             *
 *  Copyright (C) 2004, 2005 by Rutger W. ter Borg                         *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU General Public License            *
 *  as published by the Free Software Foundation; either version 2         *
 *  of the License, or (at your option) any later version.                 *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307  *
 ***************************************************************************/

#ifndef FUNCTOR_ELEMENT_HPP
#define FUNCTOR_ELEMENT_HPP

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/size.hpp>
#include <boost/range/value_type.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_scalar.hpp>

namespace mpl = boost::mpl;

namespace kml { namespace detail {

template<template<typename> class Functor>
struct functor_element_scalar {
	template<typename T>
	static T compute( T const x, T const y ) {
		return Functor<T>()(x, y);
	}
};


template<template<typename> class Functor>
struct functor_element_range {
	template<typename T>
	static T compute( T const &x, T const &y ) {
		typedef typename boost::range_value<T>::type scalar_type;
		T answer( boost::size(x) );
		std::transform( boost::begin(x), boost::end(x), boost::begin(y), boost::begin(answer), Functor<scalar_type>() );
		return answer;
	}
};

template<template<typename> class Functor, typename T>
T functor_element( T const &x, T const &y ) {
	return mpl::if_< boost::is_scalar<T>, functor_element_scalar<Functor>, functor_element_range<Functor> >::type::compute( x, y );
}

}} // namespace kml::detail

#endif


