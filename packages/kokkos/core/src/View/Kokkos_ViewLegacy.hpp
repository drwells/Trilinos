//@HEADER
// ************************************************************************
//
//                        Kokkos v. 4.0
//       Copyright (2022) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Part of Kokkos, under the Apache License v2.0 with LLVM Exceptions.
// See https://kokkos.org/LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//@HEADER

#ifndef KOKKOS_IMPL_PUBLIC_INCLUDE
#include <Kokkos_Macros.hpp>
static_assert(false,
              "Including non-public Kokkos header files is not allowed.");
#endif
#ifndef KOKKOS_VIEWLEGACY_HPP
#define KOKKOS_VIEWLEGACY_HPP

#include <type_traits>
#include <string>
#include <algorithm>
#include <initializer_list>

#include <Kokkos_Core_fwd.hpp>
#include <Kokkos_HostSpace.hpp>
#include <Kokkos_MemoryTraits.hpp>
#include <Kokkos_ExecPolicy.hpp>
#include <View/Hooks/Kokkos_ViewHooks.hpp>

#include <impl/Kokkos_Tools.hpp>
#include <impl/Kokkos_Utilities.hpp>

#ifdef KOKKOS_ENABLE_IMPL_MDSPAN
#include <View/MDSpan/Kokkos_MDSpan_Extents.hpp>
#include <View/MDSpan/Kokkos_MDSpan_Layout.hpp>
#include <View/MDSpan/Kokkos_MDSpan_Accessor.hpp>
#endif
#include <Kokkos_MinMax.hpp>

#include <View/Kokkos_ViewTraits.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace Kokkos {

/** \class View
 *  \brief View to an array of data.
 *
 * A View represents an array of one or more dimensions.
 * For details, please refer to Kokkos' tutorial materials.
 *
 * \section Kokkos_View_TemplateParameters Template parameters
 *
 * This class has both required and optional template parameters.  The
 * \c DataType parameter must always be provided, and must always be
 * first. The parameters \c Arg1Type, \c Arg2Type, and \c Arg3Type are
 * placeholders for different template parameters.  The default value
 * of the fifth template parameter \c Specialize suffices for most use
 * cases.  When explaining the template parameters, we won't refer to
 * \c Arg1Type, \c Arg2Type, and \c Arg3Type; instead, we will refer
 * to the valid categories of template parameters, in whatever order
 * they may occur.
 *
 * Valid ways in which template arguments may be specified:
 *   - View< DataType >
 *   - View< DataType , Layout >
 *   - View< DataType , Layout , Space >
 *   - View< DataType , Layout , Space , MemoryTraits >
 *   - View< DataType , Space >
 *   - View< DataType , Space , MemoryTraits >
 *   - View< DataType , MemoryTraits >
 *
 * \tparam DataType (required) This indicates both the type of each
 *   entry of the array, and the combination of compile-time and
 *   run-time array dimension(s).  For example, <tt>double*</tt>
 *   indicates a one-dimensional array of \c double with run-time
 *   dimension, and <tt>int*[3]</tt> a two-dimensional array of \c int
 *   with run-time first dimension and compile-time second dimension
 *   (of 3).  In general, the run-time dimensions (if any) must go
 *   first, followed by zero or more compile-time dimensions.  For
 *   more examples, please refer to the tutorial materials.
 *
 * \tparam Space (required) The memory space.
 *
 * \tparam Layout (optional) The array's layout in memory.  For
 *   example, LayoutLeft indicates a column-major (Fortran style)
 *   layout, and LayoutRight a row-major (C style) layout.  If not
 *   specified, this defaults to the preferred layout for the
 *   <tt>Space</tt>.
 *
 * \tparam MemoryTraits (optional) Assertion of the user's intended
 *   access behavior.  For example, RandomAccess indicates read-only
 *   access with limited spatial locality, and Unmanaged lets users
 *   wrap externally allocated memory in a View without automatic
 *   deallocation.
 *
 * \section Kokkos_View_MT MemoryTraits discussion
 *
 * \subsection Kokkos_View_MT_Interp MemoryTraits interpretation depends on
 * Space
 *
 * Some \c MemoryTraits options may have different interpretations for
 * different \c Space types.  For example, with the Cuda device,
 * \c RandomAccess tells Kokkos to fetch the data through the texture
 * cache, whereas the non-GPU devices have no such hardware construct.
 *
 * \subsection Kokkos_View_MT_PrefUse Preferred use of MemoryTraits
 *
 * Users should defer applying the optional \c MemoryTraits parameter
 * until the point at which they actually plan to rely on it in a
 * computational kernel.  This minimizes the number of template
 * parameters exposed in their code, which reduces the cost of
 * compilation.  Users may always assign a View without specified
 * \c MemoryTraits to a compatible View with that specification.
 * For example:
 * \code
 * // Pass in the simplest types of View possible.
 * void
 * doSomething (View<double*, Cuda> out,
 *              View<const double*, Cuda> in)
 * {
 *   // Assign the "generic" View in to a RandomAccess View in_rr.
 *   // Note that RandomAccess View objects must have const data.
 *   View<const double*, Cuda, RandomAccess> in_rr = in;
 *   // ... do something with in_rr and out ...
 * }
 * \endcode
 */

}  // namespace Kokkos

namespace Kokkos {

template <class T1, class T2>
struct is_always_assignable_impl;

template <class... ViewTDst, class... ViewTSrc>
struct is_always_assignable_impl<Kokkos::View<ViewTDst...>,
                                 Kokkos::View<ViewTSrc...>> {
  using mapping_type = Kokkos::Impl::ViewMapping<
      typename Kokkos::View<ViewTDst...>::traits,
      typename Kokkos::View<ViewTSrc...>::traits,
      typename Kokkos::View<ViewTDst...>::traits::specialize>;

  constexpr static bool value =
      mapping_type::is_assignable &&
      static_cast<int>(Kokkos::View<ViewTDst...>::rank_dynamic) >=
          static_cast<int>(Kokkos::View<ViewTSrc...>::rank_dynamic);
};

template <class View1, class View2>
using is_always_assignable = is_always_assignable_impl<
    std::remove_reference_t<View1>,
    std::remove_const_t<std::remove_reference_t<View2>>>;

template <class T1, class T2>
inline constexpr bool is_always_assignable_v =
    is_always_assignable<T1, T2>::value;

template <class... ViewTDst, class... ViewTSrc>
constexpr bool is_assignable(const Kokkos::View<ViewTDst...>& dst,
                             const Kokkos::View<ViewTSrc...>& src) {
  using DstTraits = typename Kokkos::View<ViewTDst...>::traits;
  using SrcTraits = typename Kokkos::View<ViewTSrc...>::traits;
  using mapping_type =
      Kokkos::Impl::ViewMapping<DstTraits, SrcTraits,
                                typename DstTraits::specialize>;

  return is_always_assignable_v<Kokkos::View<ViewTDst...>,
                                Kokkos::View<ViewTSrc...>> ||
         (mapping_type::is_assignable &&
          ((DstTraits::dimension::rank_dynamic >= 1) ||
           (dst.static_extent(0) == src.extent(0))) &&
          ((DstTraits::dimension::rank_dynamic >= 2) ||
           (dst.static_extent(1) == src.extent(1))) &&
          ((DstTraits::dimension::rank_dynamic >= 3) ||
           (dst.static_extent(2) == src.extent(2))) &&
          ((DstTraits::dimension::rank_dynamic >= 4) ||
           (dst.static_extent(3) == src.extent(3))) &&
          ((DstTraits::dimension::rank_dynamic >= 5) ||
           (dst.static_extent(4) == src.extent(4))) &&
          ((DstTraits::dimension::rank_dynamic >= 6) ||
           (dst.static_extent(5) == src.extent(5))) &&
          ((DstTraits::dimension::rank_dynamic >= 7) ||
           (dst.static_extent(6) == src.extent(6))) &&
          ((DstTraits::dimension::rank_dynamic >= 8) ||
           (dst.static_extent(7) == src.extent(7))));
}

} /* namespace Kokkos */

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#include <View/Kokkos_ViewMapping.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace Kokkos {

template <class DataType, class... Properties>
class View;

template <class>
struct is_view : public std::false_type {};

template <class D, class... P>
struct is_view<View<D, P...>> : public std::true_type {};

template <class D, class... P>
struct is_view<const View<D, P...>> : public std::true_type {};

template <class T>
inline constexpr bool is_view_v = is_view<T>::value;

template <class DataType, class... Properties>
class View : public ViewTraits<DataType, Properties...> {
 private:
  template <class, class...>
  friend class View;
  template <class, class...>
  friend class Kokkos::Impl::ViewMapping;

  using view_tracker_type = Kokkos::Impl::ViewTracker<View>;

 public:
  using traits = ViewTraits<DataType, Properties...>;

 private:
  using map_type =
      Kokkos::Impl::ViewMapping<traits, typename traits::specialize>;
  template <typename V>
  friend struct Kokkos::Impl::ViewTracker;
  using hooks_policy = typename traits::hooks_policy;

  view_tracker_type m_track;
  map_type m_map;

 public:
  //----------------------------------------
  /** \brief  Compatible view of array of scalar types */
  using array_type =
      View<typename traits::scalar_array_type, typename traits::array_layout,
           typename traits::device_type, typename traits::hooks_policy,
           typename traits::memory_traits>;

  /** \brief  Compatible view of const data type */
  using const_type =
      View<typename traits::const_data_type, typename traits::array_layout,
           typename traits::device_type, typename traits::hooks_policy,
           typename traits::memory_traits>;

  /** \brief  Compatible view of non-const data type */
  using non_const_type =
      View<typename traits::non_const_data_type, typename traits::array_layout,
           typename traits::device_type, typename traits::hooks_policy,
           typename traits::memory_traits>;

  /** \brief  Compatible host mirror view */
  using host_mirror_type =
      View<typename traits::non_const_data_type, typename traits::array_layout,
           Device<DefaultHostExecutionSpace,
                  typename traits::host_mirror_space::memory_space>,
           typename traits::hooks_policy>;

  /** \brief  Compatible host mirror view */
  using HostMirror = host_mirror_type;

  /** \brief Unified types */
  using uniform_type = typename Impl::ViewUniformType<View, 0>::type;
  using uniform_const_type =
      typename Impl::ViewUniformType<View, 0>::const_type;
  using uniform_runtime_type =
      typename Impl::ViewUniformType<View, 0>::runtime_type;
  using uniform_runtime_const_type =
      typename Impl::ViewUniformType<View, 0>::runtime_const_type;
  using uniform_nomemspace_type =
      typename Impl::ViewUniformType<View, 0>::nomemspace_type;
  using uniform_const_nomemspace_type =
      typename Impl::ViewUniformType<View, 0>::const_nomemspace_type;
  using uniform_runtime_nomemspace_type =
      typename Impl::ViewUniformType<View, 0>::runtime_nomemspace_type;
  using uniform_runtime_const_nomemspace_type =
      typename Impl::ViewUniformType<View, 0>::runtime_const_nomemspace_type;

  using reference_type = typename map_type::reference_type;
  using pointer_type   = typename map_type::pointer_type;

  // Typedefs from mdspan
  // using extents_type -> not applicable
  // Defining layout_type here made MSVC+CUDA fail
  // using layout_type = typename traits::array_layout;
  // using accessor_type -> not applicable
  // using mapping_type -> not applicable
  using element_type = typename traits::value_type;
  // using value_type -> conflicts with traits::value_type
  using index_type = typename traits::memory_space::size_type;
  // using size_type -> already from traits::size_type; where it is
  // memory_space::size_type
  using rank_type        = size_t;
  using data_handle_type = pointer_type;
  using reference        = reference_type;

  //----------------------------------------
  // Domain rank and extents

  static constexpr Impl::integral_constant<size_t, traits::dimension::rank>
      rank = {};
  static constexpr Impl::integral_constant<size_t,
                                           traits::dimension::rank_dynamic>
      rank_dynamic = {};
#ifdef KOKKOS_ENABLE_DEPRECATED_CODE_4
  enum {Rank KOKKOS_DEPRECATED_WITH_COMMENT("Use rank instead.") =
            map_type::Rank};
#endif

  template <typename iType>
  KOKKOS_INLINE_FUNCTION constexpr std::enable_if_t<std::is_integral_v<iType>,
                                                    size_t>
  extent(const iType& r) const noexcept {
    return m_map.extent(r);
  }

  static KOKKOS_INLINE_FUNCTION constexpr size_t static_extent(
      const unsigned r) noexcept {
    return map_type::static_extent(r);
  }

  template <typename iType>
  KOKKOS_INLINE_FUNCTION constexpr std::enable_if_t<std::is_integral_v<iType>,
                                                    int>
  extent_int(const iType& r) const noexcept {
    return static_cast<int>(m_map.extent(r));
  }

  KOKKOS_INLINE_FUNCTION constexpr typename traits::array_layout layout()
      const {
    return m_map.layout();
  }

  //----------------------------------------
  /*  Deprecate all 'dimension' functions in favor of
   *  ISO/C++ vocabulary 'extent'.
   */

  KOKKOS_INLINE_FUNCTION constexpr size_t size() const {
    return m_map.dimension_0() * m_map.dimension_1() * m_map.dimension_2() *
           m_map.dimension_3() * m_map.dimension_4() * m_map.dimension_5() *
           m_map.dimension_6() * m_map.dimension_7();
  }

  KOKKOS_INLINE_FUNCTION constexpr size_t stride_0() const {
    return m_map.stride_0();
  }
  KOKKOS_INLINE_FUNCTION constexpr size_t stride_1() const {
    return m_map.stride_1();
  }
  KOKKOS_INLINE_FUNCTION constexpr size_t stride_2() const {
    return m_map.stride_2();
  }
  KOKKOS_INLINE_FUNCTION constexpr size_t stride_3() const {
    return m_map.stride_3();
  }
  KOKKOS_INLINE_FUNCTION constexpr size_t stride_4() const {
    return m_map.stride_4();
  }
  KOKKOS_INLINE_FUNCTION constexpr size_t stride_5() const {
    return m_map.stride_5();
  }
  KOKKOS_INLINE_FUNCTION constexpr size_t stride_6() const {
    return m_map.stride_6();
  }
  KOKKOS_INLINE_FUNCTION constexpr size_t stride_7() const {
    return m_map.stride_7();
  }

  template <typename iType>
  KOKKOS_INLINE_FUNCTION constexpr std::enable_if_t<std::is_integral_v<iType>,
                                                    size_t>
  stride(iType r) const {
    return (
        r == 0
            ? m_map.stride_0()
            : (r == 1
                   ? m_map.stride_1()
                   : (r == 2
                          ? m_map.stride_2()
                          : (r == 3
                                 ? m_map.stride_3()
                                 : (r == 4
                                        ? m_map.stride_4()
                                        : (r == 5
                                               ? m_map.stride_5()
                                               : (r == 6
                                                      ? m_map.stride_6()
                                                      : m_map.stride_7())))))));
  }

  template <typename iType>
  KOKKOS_INLINE_FUNCTION void stride(iType* const s) const {
    m_map.stride(s);
  }

  //----------------------------------------
  // Range span is the span which contains all members.

  enum {
    reference_type_is_lvalue_reference =
        std::is_lvalue_reference_v<reference_type>
  };

  KOKKOS_INLINE_FUNCTION constexpr size_t span() const { return m_map.span(); }
  KOKKOS_INLINE_FUNCTION bool span_is_contiguous() const {
    return m_map.span_is_contiguous();
  }
  KOKKOS_INLINE_FUNCTION constexpr bool is_allocated() const {
    return m_map.data() != nullptr;
  }
  KOKKOS_INLINE_FUNCTION constexpr pointer_type data() const {
    return m_map.data();
  }

  //----------------------------------------
  // Allow specializations to query their specialized map

  KOKKOS_INLINE_FUNCTION
  const Kokkos::Impl::ViewMapping<traits, typename traits::specialize>&
  impl_map() const {
    return m_map;
  }
  KOKKOS_INLINE_FUNCTION
  const Kokkos::Impl::SharedAllocationTracker& impl_track() const {
    return m_track.m_tracker;
  }
  //----------------------------------------

 private:
  static constexpr bool is_layout_left =
      std::is_same_v<typename traits::array_layout, Kokkos::LayoutLeft>;

  static constexpr bool is_layout_right =
      std::is_same_v<typename traits::array_layout, Kokkos::LayoutRight>;

  static constexpr bool is_layout_stride =
      std::is_same_v<typename traits::array_layout, Kokkos::LayoutStride>;

  static constexpr bool is_default_map =
      std::is_void_v<typename traits::specialize> &&
      (is_layout_left || is_layout_right || is_layout_stride);

#if defined(KOKKOS_ENABLE_DEBUG_BOUNDS_CHECK)

#define KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(...)                               \
  Kokkos::Impl::runtime_check_memory_access_violation<                      \
      typename traits::memory_space>(                                       \
      "Kokkos::View ERROR: attempt to access inaccessible memory space",    \
      __VA_ARGS__);                                                         \
  Kokkos::Impl::view_verify_operator_bounds<typename traits::memory_space>( \
      __VA_ARGS__);

#else

#define KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(...)                            \
  Kokkos::Impl::runtime_check_memory_access_violation<                   \
      typename traits::memory_space>(                                    \
      "Kokkos::View ERROR: attempt to access inaccessible memory space", \
      __VA_ARGS__);

#endif

  template <typename... Is>
  static KOKKOS_FUNCTION void check_access_member_function_valid_args(Is...) {
    static_assert(rank <= sizeof...(Is));
    static_assert(sizeof...(Is) <= 8);
    static_assert(Kokkos::Impl::are_integral<Is...>::value);
  }

  template <typename... Is>
  static KOKKOS_FUNCTION void check_operator_parens_valid_args(Is...) {
    static_assert(rank == sizeof...(Is));
    static_assert(Kokkos::Impl::are_integral<Is...>::value);
  }

 public:
  //------------------------------
  // Rank 1 default map operator()

  template <typename I0>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0>::value &&  //
                        (1 == rank) && is_default_map && !is_layout_stride),
                       reference_type>
      operator()(I0 i0) const {
    check_operator_parens_valid_args(i0);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0)
    return m_map.m_impl_handle[i0];
  }

  template <typename I0>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0>::value &&  //
                        (1 == rank) && is_default_map && is_layout_stride),
                       reference_type>
      operator()(I0 i0) const {
    check_operator_parens_valid_args(i0);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0)
    return m_map.m_impl_handle[m_map.m_impl_offset.m_stride.S0 * i0];
  }

  //------------------------------
  // Rank 1 operator[]

  template <typename I0>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      ((1 == rank) && Kokkos::Impl::are_integral<I0>::value && !is_default_map),
      reference_type>
  operator[](I0 i0) const {
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0)
    return m_map.reference(i0);
  }

  template <typename I0>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<((1 == rank) && Kokkos::Impl::are_integral<I0>::value &&
                        is_default_map && !is_layout_stride),
                       reference_type>
      operator[](I0 i0) const {
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0)
    return m_map.m_impl_handle[i0];
  }

  template <typename I0>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<((1 == rank) && Kokkos::Impl::are_integral<I0>::value &&
                        is_default_map && is_layout_stride),
                       reference_type>
      operator[](I0 i0) const {
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0)
    return m_map.m_impl_handle[m_map.m_impl_offset.m_stride.S0 * i0];
  }

  //------------------------------
  // Rank 2 default map operator()

  template <typename I0, typename I1>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1>::value &&  //
       (2 == rank) && is_default_map &&
       (is_layout_left || is_layout_right || is_layout_stride)),
      reference_type>
  operator()(I0 i0, I1 i1) const {
    check_operator_parens_valid_args(i0, i1);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1)
    if constexpr (is_layout_left) {
      if constexpr (rank_dynamic == 0)
        return m_map.m_impl_handle[i0 + m_map.m_impl_offset.m_dim.N0 * i1];
      else
        return m_map.m_impl_handle[i0 + m_map.m_impl_offset.m_stride * i1];
    } else if constexpr (is_layout_right) {
      if constexpr (rank_dynamic == 0)
        return m_map.m_impl_handle[i1 + m_map.m_impl_offset.m_dim.N1 * i0];
      else
        return m_map.m_impl_handle[i1 + m_map.m_impl_offset.m_stride * i0];
    } else {
      static_assert(is_layout_stride);
      return m_map.m_impl_handle[i0 * m_map.m_impl_offset.m_stride.S0 +
                                 i1 * m_map.m_impl_offset.m_stride.S1];
    }
  }

  // Rank 0 -> 8 operator() except for rank-1 and rank-2 with default map which
  // have "inlined" versions above

  template <typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<Is...>::value &&  //
       (2 != rank) && (1 != rank) && (0 != rank) && is_default_map),
      reference_type>
  operator()(Is... indices) const {
    check_operator_parens_valid_args(indices...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, indices...)
    return m_map.m_impl_handle[m_map.m_impl_offset(indices...)];
  }

  template <typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<Is...>::value &&  //
                        ((0 == rank) || !is_default_map)),
                       reference_type>
      operator()(Is... indices) const {
    check_operator_parens_valid_args(indices...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, indices...)
    return m_map.reference(indices...);
  }

  //------------------------------
  // Rank 0

  template <typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<Is...>::value && (0 == rank)), reference_type>
  access(Is... extra) const {
    check_access_member_function_valid_args(extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, extra...)
    return m_map.reference();
  }

  //------------------------------
  // Rank 1

  template <typename I0, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0, Is...>::value &&
                        (1 == rank) && !is_default_map),
                       reference_type>
      access(I0 i0, Is... extra) const {
    check_access_member_function_valid_args(i0, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, extra...)
    return m_map.reference(i0);
  }

  template <typename I0, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0, Is...>::value &&
                        (1 == rank) && is_default_map && !is_layout_stride),
                       reference_type>
      access(I0 i0, Is... extra) const {
    check_access_member_function_valid_args(i0, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, extra...)
    return m_map.m_impl_handle[i0];
  }

  template <typename I0, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0, Is...>::value &&
                        (1 == rank) && is_default_map && is_layout_stride),
                       reference_type>
      access(I0 i0, Is... extra) const {
    check_access_member_function_valid_args(i0, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, extra...)
    return m_map.m_impl_handle[m_map.m_impl_offset.m_stride.S0 * i0];
  }

  //------------------------------
  // Rank 2

  template <typename I0, typename I1, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0, I1, Is...>::value &&
                        (2 == rank) && !is_default_map),
                       reference_type>
      access(I0 i0, I1 i1, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, extra...)
    return m_map.reference(i0, i1);
  }

  template <typename I0, typename I1, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, Is...>::value && (2 == rank) &&
       is_default_map &&
       (is_layout_left || is_layout_right || is_layout_stride)),
      reference_type>
  access(I0 i0, I1 i1, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, extra...)
    if constexpr (is_layout_left) {
      if constexpr (rank_dynamic == 0)
        return m_map.m_impl_handle[i0 + m_map.m_impl_offset.m_dim.N0 * i1];
      else
        return m_map.m_impl_handle[i0 + m_map.m_impl_offset.m_stride * i1];
    } else if constexpr (is_layout_right) {
      if constexpr (rank_dynamic == 0)
        return m_map.m_impl_handle[i1 + m_map.m_impl_offset.m_dim.N1 * i0];
      else
        return m_map.m_impl_handle[i1 + m_map.m_impl_offset.m_stride * i0];
    } else {
      static_assert(is_layout_stride);
      return m_map.m_impl_handle[i0 * m_map.m_impl_offset.m_stride.S0 +
                                 i1 * m_map.m_impl_offset.m_stride.S1];
    }
  }

  //------------------------------
  // Rank 3

  template <typename I0, typename I1, typename I2, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0, I1, I2, Is...>::value &&
                        (3 == rank) && is_default_map),
                       reference_type>
      access(I0 i0, I1 i1, I2 i2, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, extra...)
    return m_map.m_impl_handle[m_map.m_impl_offset(i0, i1, i2)];
  }

  template <typename I0, typename I1, typename I2, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0, I1, I2, Is...>::value &&
                        (3 == rank) && !is_default_map),
                       reference_type>
      access(I0 i0, I1 i1, I2 i2, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, extra...)
    return m_map.reference(i0, i1, i2);
  }

  //------------------------------
  // Rank 4

  template <typename I0, typename I1, typename I2, typename I3, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, I2, I3, Is...>::value && (4 == rank) &&
       is_default_map),
      reference_type>
  access(I0 i0, I1 i1, I2 i2, I3 i3, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, extra...)
    return m_map.m_impl_handle[m_map.m_impl_offset(i0, i1, i2, i3)];
  }

  template <typename I0, typename I1, typename I2, typename I3, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, I2, I3, Is...>::value && (4 == rank) &&
       !is_default_map),
      reference_type>
  access(I0 i0, I1 i1, I2 i2, I3 i3, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, extra...)
    return m_map.reference(i0, i1, i2, i3);
  }

  //------------------------------
  // Rank 5

  template <typename I0, typename I1, typename I2, typename I3, typename I4,
            typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, I2, I3, I4, Is...>::value &&
       (5 == rank) && is_default_map),
      reference_type>
  access(I0 i0, I1 i1, I2 i2, I3 i3, I4 i4, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, i4, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, i4,
                                     extra...)
    return m_map.m_impl_handle[m_map.m_impl_offset(i0, i1, i2, i3, i4)];
  }

  template <typename I0, typename I1, typename I2, typename I3, typename I4,
            typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, I2, I3, I4, Is...>::value &&
       (5 == rank) && !is_default_map),
      reference_type>
  access(I0 i0, I1 i1, I2 i2, I3 i3, I4 i4, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, i4, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, i4,
                                     extra...)
    return m_map.reference(i0, i1, i2, i3, i4);
  }

  //------------------------------
  // Rank 6

  template <typename I0, typename I1, typename I2, typename I3, typename I4,
            typename I5, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, I2, I3, I4, I5, Is...>::value &&
       (6 == rank) && is_default_map),
      reference_type>
  access(I0 i0, I1 i1, I2 i2, I3 i3, I4 i4, I5 i5, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, i4, i5, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, i4, i5,
                                     extra...)
    return m_map.m_impl_handle[m_map.m_impl_offset(i0, i1, i2, i3, i4, i5)];
  }

  template <typename I0, typename I1, typename I2, typename I3, typename I4,
            typename I5, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, I2, I3, I4, I5, Is...>::value &&
       (6 == rank) && !is_default_map),
      reference_type>
  access(I0 i0, I1 i1, I2 i2, I3 i3, I4 i4, I5 i5, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, i4, i5, extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, i4, i5,
                                     extra...)
    return m_map.reference(i0, i1, i2, i3, i4, i5);
  }

  //------------------------------
  // Rank 7

  template <typename I0, typename I1, typename I2, typename I3, typename I4,
            typename I5, typename I6, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, I2, I3, I4, I5, I6, Is...>::value &&
       (7 == rank) && is_default_map),
      reference_type>
  access(I0 i0, I1 i1, I2 i2, I3 i3, I4 i4, I5 i5, I6 i6, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, i4, i5, i6,
                                            extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, i4, i5, i6,
                                     extra...)
    return m_map.m_impl_handle[m_map.m_impl_offset(i0, i1, i2, i3, i4, i5, i6)];
  }

  template <typename I0, typename I1, typename I2, typename I3, typename I4,
            typename I5, typename I6, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION std::enable_if_t<
      (Kokkos::Impl::always_true<I0, I1, I2, I3, I4, I5, I6, Is...>::value &&
       (7 == rank) && !is_default_map),
      reference_type>
  access(I0 i0, I1 i1, I2 i2, I3 i3, I4 i4, I5 i5, I6 i6, Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, i4, i5, i6,
                                            extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, i4, i5, i6,
                                     extra...)
    return m_map.reference(i0, i1, i2, i3, i4, i5, i6);
  }

  //------------------------------
  // Rank 8

  template <typename I0, typename I1, typename I2, typename I3, typename I4,
            typename I5, typename I6, typename I7, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0, I1, I2, I3, I4, I5, I6,
                                                  I7, Is...>::value &&
                        (8 == rank) && is_default_map),
                       reference_type>
      access(I0 i0, I1 i1, I2 i2, I3 i3, I4 i4, I5 i5, I6 i6, I7 i7,
             Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, i4, i5, i6, i7,
                                            extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, i4, i5, i6,
                                     i7, extra...)
    return m_map
        .m_impl_handle[m_map.m_impl_offset(i0, i1, i2, i3, i4, i5, i6, i7)];
  }

  template <typename I0, typename I1, typename I2, typename I3, typename I4,
            typename I5, typename I6, typename I7, typename... Is>
  KOKKOS_FORCEINLINE_FUNCTION
      std::enable_if_t<(Kokkos::Impl::always_true<I0, I1, I2, I3, I4, I5, I6,
                                                  I7, Is...>::value &&
                        (8 == rank) && !is_default_map),
                       reference_type>
      access(I0 i0, I1 i1, I2 i2, I3 i3, I4 i4, I5 i5, I6 i6, I7 i7,
             Is... extra) const {
    check_access_member_function_valid_args(i0, i1, i2, i3, i4, i5, i6, i7,
                                            extra...);
    KOKKOS_IMPL_VIEW_OPERATOR_VERIFY(m_track, m_map, i0, i1, i2, i3, i4, i5, i6,
                                     i7, extra...)
    return m_map.reference(i0, i1, i2, i3, i4, i5, i6, i7);
  }

#undef KOKKOS_IMPL_VIEW_OPERATOR_VERIFY

  //----------------------------------------
  // Standard destructor, constructors, and assignment operators

  KOKKOS_DEFAULTED_FUNCTION
  ~View() = default;

  KOKKOS_DEFAULTED_FUNCTION
  View() = default;

  KOKKOS_FUNCTION
  View(const View& other) : m_track(other.m_track), m_map(other.m_map) {
    KOKKOS_IF_ON_HOST((hooks_policy::copy_construct(*this, other);))
  }

  KOKKOS_FUNCTION
  View(View&& other)
      : m_track{std::move(other.m_track)}, m_map{std::move(other.m_map)} {
    KOKKOS_IF_ON_HOST((hooks_policy::move_construct(*this, other);))
  }

  KOKKOS_FUNCTION
  View& operator=(const View& other) {
    m_map   = other.m_map;
    m_track = other.m_track;

    KOKKOS_IF_ON_HOST((hooks_policy::copy_assign(*this, other);))

    return *this;
  }

  KOKKOS_FUNCTION
  View& operator=(View&& other) {
    m_map   = std::move(other.m_map);
    m_track = std::move(other.m_track);

    KOKKOS_IF_ON_HOST((hooks_policy::move_assign(*this, other);))

    return *this;
  }

  //----------------------------------------
  // Compatible view copy constructor and assignment
  // may assign unmanaged from managed.

  template <class RT, class... RP>
  KOKKOS_INLINE_FUNCTION View(
      const View<RT, RP...>& rhs,
      std::enable_if_t<Kokkos::Impl::ViewMapping<
          traits, typename View<RT, RP...>::traits,
          typename traits::specialize>::is_assignable_data_type>* = nullptr)
      : m_track(rhs), m_map() {
    using SrcTraits = typename View<RT, RP...>::traits;
    using Mapping   = Kokkos::Impl::ViewMapping<traits, SrcTraits,
                                              typename traits::specialize>;
    static_assert(Mapping::is_assignable,
                  "Incompatible View copy construction");
    Mapping::assign(m_map, rhs.m_map, rhs.m_track.m_tracker);
  }

  template <class RT, class... RP>
  KOKKOS_INLINE_FUNCTION std::enable_if_t<
      Kokkos::Impl::ViewMapping<
          traits, typename View<RT, RP...>::traits,
          typename traits::specialize>::is_assignable_data_type,
      View>&
  operator=(const View<RT, RP...>& rhs) {
    using SrcTraits = typename View<RT, RP...>::traits;
    using Mapping   = Kokkos::Impl::ViewMapping<traits, SrcTraits,
                                              typename traits::specialize>;
    static_assert(Mapping::is_assignable, "Incompatible View copy assignment");
    Mapping::assign(m_map, rhs.m_map, rhs.m_track.m_tracker);
    m_track.assign(rhs);
    return *this;
  }

  //----------------------------------------
  // Compatible subview constructor
  // may assign unmanaged from managed.

  template <class RT, class... RP, class Arg0, class... Args>
  KOKKOS_INLINE_FUNCTION View(const View<RT, RP...>& src_view, const Arg0 arg0,
                              Args... args)
      : m_track(src_view), m_map() {
    using SrcType = View<RT, RP...>;

    using Mapping = Kokkos::Impl::ViewMapping<void, typename SrcType::traits,
                                              Arg0, Args...>;

    using DstType = typename Mapping::type;

    static_assert(
        Kokkos::Impl::ViewMapping<traits, typename DstType::traits,
                                  typename traits::specialize>::is_assignable,
        "Subview construction requires compatible view and subview arguments");

    Mapping::assign(m_map, src_view.m_map, arg0, args...);
  }

  //----------------------------------------
  // Allocation tracking properties

  KOKKOS_INLINE_FUNCTION
  int use_count() const { return m_track.m_tracker.use_count(); }

  inline const std::string label() const {
    return m_track.m_tracker
        .template get_label<typename traits::memory_space>();
  }

 public:
  //----------------------------------------
  // Allocation according to allocation properties and array layout

  template <class... P>
  explicit inline View(
      const Impl::ViewCtorProp<P...>& arg_prop,
      std::enable_if_t<!Impl::ViewCtorProp<P...>::has_pointer,
                       typename traits::array_layout> const& arg_layout)
      : m_track(), m_map() {
    // Copy the input allocation properties with possibly defaulted properties
    // We need to split it in two to avoid MSVC compiler errors
    auto prop_copy_tmp =
        Impl::with_properties_if_unset(arg_prop, std::string{});
    auto prop_copy = Impl::with_properties_if_unset(
        prop_copy_tmp, typename traits::device_type::memory_space{},
        typename traits::device_type::execution_space{});
    using alloc_prop = decltype(prop_copy);

    static_assert(traits::is_managed,
                  "View allocation constructor requires managed memory");

    if (alloc_prop::initialize &&
        !alloc_prop::execution_space::impl_is_initialized()) {
      // If initializing view data then
      // the execution space must be initialized.
      Kokkos::abort(
          "Constructing View and initializing data with uninitialized "
          "execution space");
    }

#ifdef KOKKOS_ENABLE_DEBUG_BOUNDS_CHECK
    if constexpr (std::is_same_v<typename traits::array_layout,
                                 Kokkos::LayoutLeft> ||
                  std::is_same_v<typename traits::array_layout,
                                 Kokkos::LayoutRight> ||
                  std::is_same_v<typename traits::array_layout,
                                 Kokkos::LayoutStride>) {
      size_t i0 = arg_layout.dimension[0];
      size_t i1 = arg_layout.dimension[1];
      size_t i2 = arg_layout.dimension[2];
      size_t i3 = arg_layout.dimension[3];
      size_t i4 = arg_layout.dimension[4];
      size_t i5 = arg_layout.dimension[5];
      size_t i6 = arg_layout.dimension[6];
      size_t i7 = arg_layout.dimension[7];

      const std::string& alloc_name =
          Impl::get_property<Impl::LabelTag>(prop_copy);
      Impl::runtime_check_rank(
          *this, std::is_same_v<typename traits::specialize, void>, i0, i1, i2,
          i3, i4, i5, i6, i7, alloc_name.c_str());
    }
#endif

    Kokkos::Impl::SharedAllocationRecord<>* record = m_map.allocate_shared(
        prop_copy, arg_layout, Impl::ViewCtorProp<P...>::has_execution_space);

    // Setup and initialization complete, start tracking
    m_track.m_tracker.assign_allocated_record_to_uninitialized(record);
  }

  KOKKOS_INLINE_FUNCTION
  void assign_data(pointer_type arg_data) {
    m_track.m_tracker.clear();
    m_map.assign_data(arg_data);
  }

  // Wrap memory according to properties and array layout
  template <class... P>
  explicit KOKKOS_INLINE_FUNCTION View(
      const Impl::ViewCtorProp<P...>& arg_prop,
      std::enable_if_t<Impl::ViewCtorProp<P...>::has_pointer,
                       typename traits::array_layout> const& arg_layout)
      : m_track()  // No memory tracking
        ,
        m_map(arg_prop, arg_layout) {
    static_assert(
        std::is_same_v<pointer_type,
                       typename Impl::ViewCtorProp<P...>::pointer_type>,
        "Constructing View to wrap user memory must supply matching pointer "
        "type");

#ifdef KOKKOS_ENABLE_DEBUG_BOUNDS_CHECK
    if constexpr (std::is_same_v<typename traits::array_layout,
                                 Kokkos::LayoutLeft> ||
                  std::is_same_v<typename traits::array_layout,
                                 Kokkos::LayoutRight> ||
                  std::is_same_v<typename traits::array_layout,
                                 Kokkos::LayoutStride>) {
      size_t i0 = arg_layout.dimension[0];
      size_t i1 = arg_layout.dimension[1];
      size_t i2 = arg_layout.dimension[2];
      size_t i3 = arg_layout.dimension[3];
      size_t i4 = arg_layout.dimension[4];
      size_t i5 = arg_layout.dimension[5];
      size_t i6 = arg_layout.dimension[6];
      size_t i7 = arg_layout.dimension[7];

      Impl::runtime_check_rank(
          *this, std::is_same_v<typename traits::specialize, void>, i0, i1, i2,
          i3, i4, i5, i6, i7, "UNMANAGED");
    }
#endif
  }

  // Simple dimension-only layout
  template <class... P>
  explicit inline View(
      const Impl::ViewCtorProp<P...>& arg_prop,
      std::enable_if_t<!Impl::ViewCtorProp<P...>::has_pointer, size_t> const
          arg_N0          = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N1 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N2 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N3 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N4 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N5 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N6 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N7 = KOKKOS_IMPL_CTOR_DEFAULT_ARG)
      : View(arg_prop,
             typename traits::array_layout(arg_N0, arg_N1, arg_N2, arg_N3,
                                           arg_N4, arg_N5, arg_N6, arg_N7)) {
    static_assert(traits::array_layout::is_extent_constructible,
                  "Layout is not constructible from extent arguments. Use "
                  "overload taking a layout object instead.");
  }

  template <class... P>
  explicit KOKKOS_INLINE_FUNCTION View(
      const Impl::ViewCtorProp<P...>& arg_prop,
      std::enable_if_t<Impl::ViewCtorProp<P...>::has_pointer, size_t> const
          arg_N0          = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N1 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N2 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N3 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N4 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N5 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N6 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N7 = KOKKOS_IMPL_CTOR_DEFAULT_ARG)
      : View(arg_prop,
             typename traits::array_layout(arg_N0, arg_N1, arg_N2, arg_N3,
                                           arg_N4, arg_N5, arg_N6, arg_N7)) {
    static_assert(traits::array_layout::is_extent_constructible,
                  "Layout is not constructible from extent arguments. Use "
                  "overload taking a layout object instead.");
  }

  // Allocate with label and layout
  template <typename Label>
  explicit inline View(
      const Label& arg_label,
      std::enable_if_t<Kokkos::Impl::is_view_label<Label>::value,
                       typename traits::array_layout> const& arg_layout)
      : View(Impl::ViewCtorProp<std::string>(arg_label), arg_layout) {}

  // Allocate label and layout, must disambiguate from subview constructor.
  template <typename Label>
  explicit inline View(
      const Label& arg_label,
      std::enable_if_t<Kokkos::Impl::is_view_label<Label>::value, const size_t>
          arg_N0          = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N1 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N2 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N3 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N4 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N5 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N6 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N7 = KOKKOS_IMPL_CTOR_DEFAULT_ARG)
      : View(Impl::ViewCtorProp<std::string>(arg_label),
             typename traits::array_layout(arg_N0, arg_N1, arg_N2, arg_N3,
                                           arg_N4, arg_N5, arg_N6, arg_N7)) {
    static_assert(traits::array_layout::is_extent_constructible,
                  "Layout is not constructible from extent arguments. Use "
                  "overload taking a layout object instead.");
  }

  // Construct view from ViewTracker and map
  // This should be the preferred method because future extensions may need to
  // use the ViewTracker class.
  template <class Traits>
  KOKKOS_INLINE_FUNCTION View(
      const view_tracker_type& track,
      const Kokkos::Impl::ViewMapping<Traits, typename Traits::specialize>& map)
      : m_track(track), m_map() {
    using Mapping =
        Kokkos::Impl::ViewMapping<traits, Traits, typename traits::specialize>;
    static_assert(Mapping::is_assignable,
                  "Incompatible View copy construction");
    Mapping::assign(m_map, map, track.m_tracker);
  }

  // Construct View from internal shared allocation tracker object and map
  // This is here for backwards compatibility for classes that derive from
  // Kokkos::View
  template <class Traits>
  KOKKOS_INLINE_FUNCTION View(
      const typename view_tracker_type::track_type& track,
      const Kokkos::Impl::ViewMapping<Traits, typename Traits::specialize>& map)
      : m_track(track), m_map() {
    using Mapping =
        Kokkos::Impl::ViewMapping<traits, Traits, typename traits::specialize>;
    static_assert(Mapping::is_assignable,
                  "Incompatible View copy construction");
    Mapping::assign(m_map, map, track);
  }

  //----------------------------------------
  // Memory span required to wrap these dimensions.
  static constexpr size_t required_allocation_size(
      typename traits::array_layout const& layout) {
    return map_type::memory_span(layout);
  }

  static constexpr size_t required_allocation_size(
      const size_t arg_N0 = 0, const size_t arg_N1 = 0, const size_t arg_N2 = 0,
      const size_t arg_N3 = 0, const size_t arg_N4 = 0, const size_t arg_N5 = 0,
      const size_t arg_N6 = 0, const size_t arg_N7 = 0) {
    static_assert(traits::array_layout::is_extent_constructible,
                  "Layout is not constructible from extent arguments. Use "
                  "overload taking a layout object instead.");
    return map_type::memory_span(typename traits::array_layout(
        arg_N0, arg_N1, arg_N2, arg_N3, arg_N4, arg_N5, arg_N6, arg_N7));
  }

  explicit KOKKOS_INLINE_FUNCTION View(
      pointer_type arg_ptr, const size_t arg_N0 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N1 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N2 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N3 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N4 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N5 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N6 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N7 = KOKKOS_IMPL_CTOR_DEFAULT_ARG)
      : View(Impl::ViewCtorProp<pointer_type>(arg_ptr),
             typename traits::array_layout(arg_N0, arg_N1, arg_N2, arg_N3,
                                           arg_N4, arg_N5, arg_N6, arg_N7)) {
    static_assert(traits::array_layout::is_extent_constructible,
                  "Layout is not constructible from extent arguments. Use "
                  "overload taking a layout object instead.");
  }

  explicit KOKKOS_INLINE_FUNCTION View(
      pointer_type arg_ptr, const typename traits::array_layout& arg_layout)
      : View(Impl::ViewCtorProp<pointer_type>(arg_ptr), arg_layout) {}

  //----------------------------------------
  // Shared scratch memory constructor

  static KOKKOS_INLINE_FUNCTION size_t
  shmem_size(const size_t arg_N0 = KOKKOS_INVALID_INDEX,
             const size_t arg_N1 = KOKKOS_INVALID_INDEX,
             const size_t arg_N2 = KOKKOS_INVALID_INDEX,
             const size_t arg_N3 = KOKKOS_INVALID_INDEX,
             const size_t arg_N4 = KOKKOS_INVALID_INDEX,
             const size_t arg_N5 = KOKKOS_INVALID_INDEX,
             const size_t arg_N6 = KOKKOS_INVALID_INDEX,
             const size_t arg_N7 = KOKKOS_INVALID_INDEX) {
    static_assert(traits::array_layout::is_extent_constructible,
                  "Layout is not constructible from extent arguments. Use "
                  "overload taking a layout object instead.");
    const size_t num_passed_args = Impl::count_valid_integers(
        arg_N0, arg_N1, arg_N2, arg_N3, arg_N4, arg_N5, arg_N6, arg_N7);

    if (std::is_void_v<typename traits::specialize> &&
        num_passed_args != rank_dynamic) {
      Kokkos::abort(
          "Kokkos::View::shmem_size() rank_dynamic != number of arguments.\n");
    }

    return View::shmem_size(typename traits::array_layout(
        arg_N0, arg_N1, arg_N2, arg_N3, arg_N4, arg_N5, arg_N6, arg_N7));
  }

 private:
  // Want to be able to align to minimum scratch alignment or sizeof or alignof
  // elements
  static constexpr size_t scratch_value_alignment =
      max({sizeof(typename traits::value_type),
           alignof(typename traits::value_type),
           static_cast<size_t>(
               traits::execution_space::scratch_memory_space::ALIGN)});

 public:
  static KOKKOS_INLINE_FUNCTION size_t
  shmem_size(typename traits::array_layout const& arg_layout) {
    return map_type::memory_span(arg_layout) + scratch_value_alignment;
  }

  explicit KOKKOS_INLINE_FUNCTION View(
      const typename traits::execution_space::scratch_memory_space& arg_space,
      const typename traits::array_layout& arg_layout)
      : View(Impl::ViewCtorProp<pointer_type>(reinterpret_cast<pointer_type>(
                 arg_space.get_shmem_aligned(map_type::memory_span(arg_layout),
                                             scratch_value_alignment))),
             arg_layout) {}

  explicit KOKKOS_INLINE_FUNCTION View(
      const typename traits::execution_space::scratch_memory_space& arg_space,
      const size_t arg_N0 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N1 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N2 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N3 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N4 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N5 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N6 = KOKKOS_IMPL_CTOR_DEFAULT_ARG,
      const size_t arg_N7 = KOKKOS_IMPL_CTOR_DEFAULT_ARG)
      : View(Impl::ViewCtorProp<pointer_type>(
                 reinterpret_cast<pointer_type>(arg_space.get_shmem_aligned(
                     map_type::memory_span(typename traits::array_layout(
                         arg_N0, arg_N1, arg_N2, arg_N3, arg_N4, arg_N5, arg_N6,
                         arg_N7)),
                     scratch_value_alignment))),
             typename traits::array_layout(arg_N0, arg_N1, arg_N2, arg_N3,
                                           arg_N4, arg_N5, arg_N6, arg_N7)) {
    static_assert(traits::array_layout::is_extent_constructible,
                  "Layout is not constructible from extent arguments. Use "
                  "overload taking a layout object instead.");
  }

  //----------------------------------------
  // MDSpan converting constructors
#ifdef KOKKOS_ENABLE_IMPL_MDSPAN
  template <typename U = typename Impl::MDSpanViewTraits<traits>::mdspan_type>
  KOKKOS_INLINE_FUNCTION
#ifndef KOKKOS_ENABLE_CXX17
      explicit(traits::is_managed)
#endif
          View(const typename Impl::MDSpanViewTraits<traits>::mdspan_type& mds,
               std::enable_if_t<
                   !std::is_same_v<Impl::UnsupportedKokkosArrayLayout, U>>* =
                   nullptr)
      : View(mds.data_handle(),
             Impl::array_layout_from_mapping<
                 typename traits::array_layout,
                 typename Impl::MDSpanViewTraits<traits>::mdspan_type>(
                 mds.mapping())) {
  }

  template <class ElementType, class ExtentsType, class LayoutType,
            class AccessorType>
  KOKKOS_INLINE_FUNCTION
#ifndef KOKKOS_ENABLE_CXX17
      explicit(!std::is_convertible_v<
               Kokkos::mdspan<ElementType, ExtentsType, LayoutType,
                              AccessorType>,
               typename Impl::MDSpanViewTraits<traits>::mdspan_type>)
#endif
          View(const Kokkos::mdspan<ElementType, ExtentsType, LayoutType,
                                    AccessorType>& mds)
      : View(typename Impl::MDSpanViewTraits<traits>::mdspan_type(mds)) {
  }

  //----------------------------------------
  // Conversion to MDSpan
  template <class OtherElementType, class OtherExtents, class OtherLayoutPolicy,
            class OtherAccessor,
            class ImplNaturalMDSpanType =
                typename Impl::MDSpanViewTraits<traits>::mdspan_type,
            typename = std::enable_if_t<std::conditional_t<
                std::is_same_v<Impl::UnsupportedKokkosArrayLayout,
                               ImplNaturalMDSpanType>,
                std::false_type,
                std::is_assignable<mdspan<OtherElementType, OtherExtents,
                                          OtherLayoutPolicy, OtherAccessor>,
                                   ImplNaturalMDSpanType>>::value>>
  KOKKOS_INLINE_FUNCTION constexpr operator mdspan<
      OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor>() {
    using mdspan_type = typename Impl::MDSpanViewTraits<traits>::mdspan_type;
    return mdspan_type{data(),
                       Impl::mapping_from_view_mapping<mdspan_type>(m_map)};
  }

  template <class OtherAccessorType = Impl::SpaceAwareAccessor<
                typename traits::memory_space,
                Kokkos::default_accessor<typename traits::value_type>>,
            typename = std::enable_if_t<std::is_assignable_v<
                typename traits::value_type*&,
                typename OtherAccessorType::data_handle_type>>>
  KOKKOS_INLINE_FUNCTION constexpr auto to_mdspan(
      const OtherAccessorType& other_accessor =
          typename Impl::MDSpanViewTraits<traits>::accessor_type()) {
    using mdspan_type = typename Impl::MDSpanViewTraits<traits>::mdspan_type;
    using ret_mdspan_type =
        mdspan<typename mdspan_type::element_type,
               typename mdspan_type::extents_type,
               typename mdspan_type::layout_type, OtherAccessorType>;
    return ret_mdspan_type{data(),
                           Impl::mapping_from_view_mapping<mdspan_type>(m_map),
                           other_accessor};
  }
#endif  // KOKKOS_ENABLE_IMPL_MDSPAN
};

template <typename D, class... P>
KOKKOS_INLINE_FUNCTION constexpr unsigned rank(const View<D, P...>&) {
  return View<D, P...>::rank();
}

namespace Impl {

template <typename ValueType, unsigned int Rank>
struct RankDataType {
  using type = typename RankDataType<ValueType, Rank - 1>::type*;
};

template <typename ValueType>
struct RankDataType<ValueType, 0> {
  using type = ValueType;
};

template <unsigned N, typename... Args>
KOKKOS_FUNCTION std::enable_if_t<
    N == View<Args...>::rank() &&
        std::is_same_v<typename ViewTraits<Args...>::specialize, void>,
    View<Args...>>
as_view_of_rank_n(View<Args...> v) {
  return v;
}

// Placeholder implementation to compile generic code for DynRankView; should
// never be called
template <unsigned N, typename T, typename... Args>
KOKKOS_FUNCTION std::enable_if_t<
    N != View<T, Args...>::rank() &&
        std::is_same_v<typename ViewTraits<T, Args...>::specialize, void>,
    View<typename RankDataType<typename View<T, Args...>::value_type, N>::type,
         Args...>>
as_view_of_rank_n(View<T, Args...>) {
  Kokkos::abort("Trying to get at a View of the wrong rank");
  return {};
}

template <typename Function, typename... Args>
void apply_to_view_of_static_rank(Function&& f, View<Args...> a) {
  f(a);
}

}  // namespace Impl

template <class D, class... P, class... Args>
KOKKOS_INLINE_FUNCTION auto subview(const View<D, P...>& src, Args... args) {
  static_assert(View<D, P...>::rank == sizeof...(Args),
                "subview requires one argument for each source View rank");

  return typename Kokkos::Impl::ViewMapping<
      void /* deduce subview type from source view traits */
      ,
      typename Impl::RemoveAlignedMemoryTrait<D, P...>::type,
      Args...>::type(src, args...);
}

#ifdef KOKKOS_ENABLE_DEPRECATED_CODE_4
template <class MemoryTraits, class D, class... P, class... Args>
KOKKOS_DEPRECATED KOKKOS_INLINE_FUNCTION auto subview(const View<D, P...>& src,
                                                      Args... args) {
  static_assert(View<D, P...>::rank == sizeof...(Args),
                "subview requires one argument for each source View rank");
  static_assert(Kokkos::is_memory_traits<MemoryTraits>::value);

  return typename Kokkos::Impl::ViewMapping<
      void /* deduce subview type from source view traits */
      ,
      typename Impl::RemoveAlignedMemoryTrait<D, P..., MemoryTraits>::type,
      Args...>::type(src, args...);
}
#endif

template <class V, class... Args>
using Subview = decltype(subview(std::declval<V>(), std::declval<Args>()...));

} /* namespace Kokkos */

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace Kokkos {

template <class LT, class... LP, class RT, class... RP>
KOKKOS_INLINE_FUNCTION bool operator==(const View<LT, LP...>& lhs,
                                       const View<RT, RP...>& rhs) {
  // Same data, layout, dimensions
  using lhs_traits = ViewTraits<LT, LP...>;
  using rhs_traits = ViewTraits<RT, RP...>;

  return std::is_same_v<typename lhs_traits::const_value_type,
                        typename rhs_traits::const_value_type> &&
         std::is_same_v<typename lhs_traits::array_layout,
                        typename rhs_traits::array_layout> &&
         std::is_same_v<typename lhs_traits::memory_space,
                        typename rhs_traits::memory_space> &&
         View<LT, LP...>::rank() == View<RT, RP...>::rank() &&
         lhs.data() == rhs.data() && lhs.span() == rhs.span() &&
         lhs.extent(0) == rhs.extent(0) && lhs.extent(1) == rhs.extent(1) &&
         lhs.extent(2) == rhs.extent(2) && lhs.extent(3) == rhs.extent(3) &&
         lhs.extent(4) == rhs.extent(4) && lhs.extent(5) == rhs.extent(5) &&
         lhs.extent(6) == rhs.extent(6) && lhs.extent(7) == rhs.extent(7);
}

template <class LT, class... LP, class RT, class... RP>
KOKKOS_INLINE_FUNCTION bool operator!=(const View<LT, LP...>& lhs,
                                       const View<RT, RP...>& rhs) {
  return !(operator==(lhs, rhs));
}

} /* namespace Kokkos */

#include <View/Kokkos_ViewCommonType.hpp>
#include <View/Kokkos_ViewUniformType.hpp>
#include <View/Kokkos_ViewAtomic.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif /* #ifndef KOKKOS_VIEWLEGACY_HPP */
