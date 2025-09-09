#ifndef MP_LIBRARY_SEGMENT_TREE_UTILITY_HPP
#define MP_LIBRARY_SEGMENT_TREE_UTILITY_HPP
#include <cstddef>
#include <utility>
namespace mrpython {
template <typename F, typename T>
auto call_merge_impl(F const& f, T const& lv, std::size_t ls, T const& rv,
                     std::size_t rs, int) -> decltype(f(lv, ls, rv, rs)) {
  return f(lv, ls, rv, rs);
}
template <typename F, typename T>
auto call_merge_impl(F const& f, T const& lv, std::size_t, T const& rv,
                     std::size_t, long) -> decltype(f(lv, rv)) {
  return f(lv, rv);
}
template <typename F, typename T>
decltype(auto) call_merge(F const& f, T const& lv, std::size_t ls, T const& rv,
                          std::size_t rs) {
  return call_merge_impl(f, lv, ls, rv, rs, 0);
}
}  // namespace mrpython
#endif  // MP_LIBRARY_SEGMENT_TREE_UTILITY_HPP
