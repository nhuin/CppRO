// -*- Mode: c++; c-basic-offset: 4; tab-width: 4; -*-

/******************************************************************************
 *
 *  file:  StandardTraits.h
 *
 *  Copyright (c) 2007, Daniel Aarno, Michael E. Smoot .
 *  All rights reverved.
 *
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/

// This is an internal tclap file, you should probably not have to
// include this directly

#ifndef TCLAP_STANDARD_TRAITS_H
#define TCLAP_STANDARD_TRAITS_H

#ifdef HAVE_CONFIG_H
#include <config.h> // To check for long long
#endif

// If Microsoft has already typedef'd wchar_t as an unsigned
// short, then compiles will break because it's as if we're
// creating ArgTraits twice for unsigned short. Thus...
#ifdef _MSC_VER
#ifndef _NATIVE_WCHAR_T_DEFINED
#define TCLAP_DONT_DECLARE_WCHAR_T_ARGTRAITS
#endif
#endif

#include "ArgTraits.hpp"

#include <cstdint>
#include <string>

namespace TCLAP {

// ======================================================================
// Integer types
// ======================================================================

/**
 * longs have value-like semantics.
 */
template <>
struct ArgTraits<int64_t> {
    using ValueCategory = TCLAP::ValueLike;
};

/**
 * ints have value-like semantics.
 */
template <>
struct ArgTraits<int> {
    using ValueCategory = TCLAP::ValueLike;
};

/**
 * shorts have value-like semantics.
 */
template <>
struct ArgTraits<int16_t> {
    using ValueCategory = TCLAP::ValueLike;
};

/**
 * chars have value-like semantics.
 */
template <>
struct ArgTraits<char> {
    using ValueCategory = TCLAP::ValueLike;
};

#ifdef HAVE_LONG_LONG
/**
 * long longs have value-like semantics.
 */
template <>
struct ArgTraits<long long> {
    typedef ValueLike ValueCategory;
};
#endif

// ======================================================================
// Unsigned integer types
// ======================================================================

/**
 * unsigned longs have value-like semantics.
 */
template <>
struct ArgTraits<uint64_t> {
    using ValueCategory = TCLAP::ValueLike;
};

/**
 * unsigned ints have value-like semantics.
 */
template <>
struct ArgTraits<unsigned int> {
    using ValueCategory = TCLAP::ValueLike;
};

/**
 * unsigned shorts have value-like semantics.
 */
template <>
struct ArgTraits<uint16_t> {
    using ValueCategory = TCLAP::ValueLike;
};

/**
 * unsigned chars have value-like semantics.
 */
template <>
struct ArgTraits<unsigned char> {
    using ValueCategory = TCLAP::ValueLike;
};

// Microsoft implements size_t awkwardly.
#if defined(_MSC_VER) && defined(_M_X64)
/**
 * size_ts have value-like semantics.
 */
/*template <>
struct ArgTraits<std::size_t> {
    using ValueCategory = TCLAP::ValueLike;
};*/
#endif

#ifdef HAVE_LONG_LONG
/**
 * unsigned long longs have value-like semantics.
 */
template <>
struct ArgTraits<unsigned long long> {
    using ValueCategory = TCLAP::ValueLike;
};
#endif

// ======================================================================
// Float types
// ======================================================================

/**
 * floats have value-like semantics.
 */
template <>
struct ArgTraits<float> {
    using ValueCategory = TCLAP::ValueLike;
};

/**
 * doubles have value-like semantics.
 */
template <>
struct ArgTraits<double> {
    using ValueCategory = TCLAP::ValueLike;
};

// ======================================================================
// Other types
// ======================================================================

/**
 * bools have value-like semantics.
 */
template <>
struct ArgTraits<bool> {
    using ValueCategory = TCLAP::ValueLike;
};

/**
 * wchar_ts have value-like semantics.
 */
#ifndef TCLAP_DONT_DECLARE_WCHAR_T_ARGTRAITS
template <>
struct ArgTraits<wchar_t> {
    using ValueCategory = TCLAP::ValueLike;
};
#endif

/**
 * Strings have string like argument traits.
 */
template <>
struct ArgTraits<std::string> {
    using ValueCategory = TCLAP::StringLike;
};

template <typename T>
void SetString(T& dst, const std::string& src) {
    dst = src;
}

} // namespace TCLAP

#endif
