#pragma once

#include <vector>
#include <concepts>
#include <type_traits>
//
//
namespace common
{
template<typename vertex_t, typename... ctor_args_t>
concept graph_vertex = requires(ctor_args_t... args) {
    requires std::equality_comparable<vertex_t>;

    //{
    //    std::declval<vertex_t>().num_neighbours()
    //} -> std::same_as<std::size_t>;
    //
    //{
    //    std::declval<vertex_t>().emplace_neighbour(std::forward<ctor_args_t>(args)...)
    //} -> std::same_as<vertex_t&>;
};

template<typename value_t>
struct GraphVertex
{
protected:
    template<typename... ctor_args_t>
    [[nodiscard]] value_t& emplace_neighbour(ctor_args_t... args)
    {
        return neighbours.emplace_back(std::forward<ctor_args_t>(args)...);
    }
    std::vector<value_t> neighbours;
};
template<typename vertex_t>
requires graph_vertex<vertex_t>
class CGraph
{
public:
    [[nodiscard]] const vertex_t& root() const { return m_Root; }
private:
    vertex_t m_Root;
};
}	// namespace common
