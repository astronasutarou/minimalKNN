/**
 * @file minimalKNN.h
 * @brief header fiel for the MinimalKNN library
 * @author Ryou Ohsawa
 * @year 2020
 */
#ifndef __MINIMALKNN_H_INCLUDE
#define __MINIMALKNN_H_INCLUDE

#include <functional>
#include <algorithm>
#include <utility>
#include <limits>
#include <vector>
#include <random>
#include <set>


namespace minimalKNN {
  constexpr bool __debug__ = true;

  class kNNSet;
  class kNNBuilder;

  typedef long node_t;
  typedef std::vector<node_t> node_list;

  typedef double cost_t;
  typedef struct rated_node {
    node_t i;
    cost_t cost;
    const bool operator<(const rated_node& rhs) const
    { return cost < rhs.cost; }
    const bool operator==(const node_t& rhs) const
    { return i == rhs; }
    const bool operator==(const rated_node& rhs) const
    { return i == rhs.i; }
  } rated_node;

  typedef struct vertex {
    double x;
    double y;
    double z;
  } vertex;
  typedef std::vector<vertex> vertices;

  typedef struct edge {
    node_t u;
    node_t v;
    const bool operator<(const edge& rhs) const
    { return (u==rhs.u)?v<rhs.v:u<rhs.u; }
  } edge;
  typedef std::vector<edge> edges;
  typedef std::set<edge> graph;

  typedef std::vector<kNNSet> kNNGraph;

  class kNNSet {
  public:
    kNNSet()
      : kNNSet(std::numeric_limits<long>::max())
    {}

    kNNSet(const size_t k)
      : graph_size(k), max_cost(std::numeric_limits<double>::max())
    {}

    kNNSet(const kNNSet& obj)
      : graph_size(obj.graph_size), max_cost(obj.max_cost), B(obj.B)
    {}

    kNNSet(kNNSet&& obj)
      : graph_size(obj.graph_size), max_cost(obj.max_cost), B(std::move(obj.B))
    {}

    bool
    insert(const node_t& node, const cost_t& cost)
    {
      if (cost == 0) return false;
      if (cost < max_cost || B.size() < graph_size) {
        if (B.find({node,cost})!=B.end()) return false;
        B.insert(rated_node{node, cost});
        if (B.size() > graph_size) B.erase(prev(B.end()));
        max_cost = prev(B.end())->cost;
        return true;
      }
      else {
        return false;
      }
    }

    void
    print() const
    {
      for (const auto& e: B)
        printf("[%2ld (%.2f)]", e.i, e.cost);
      printf("\n");
    }

    const std::set<rated_node>::const_iterator
    find(const node_t& node) const
    {
      return B.find(rated_node{node, 0.0});
    }

    std::set<rated_node>::iterator
    begin() const
    {
      return B.begin();
    }

    std::set<rated_node>::iterator
    end() const
    {
      return B.end();
    }

    const node_list
    neighbors() const
    {
      node_list ret;
      for (auto& s: B) ret.push_back(s.i);
      return ret;
    }

  private:
    const size_t graph_size;
    double max_cost;
    std::set<rated_node> B;
  };


  class kNNBuilder {
  public:
    kNNBuilder(const size_t k, const vertices v)
      : n_vertices(v.size()), graph_size((k<n_vertices)?k:n_vertices), V(v)
    {
      std::mt19937 gen;
      gen.seed(std::random_device{}());

      // initialization procedure
      node_list r(n_vertices);
      std::iota(r.begin(), r.end(), 0);
      for (size_t i=0; i<n_vertices; i++) {
        kNNSet b(graph_size);
        std::shuffle(r.begin(), r.end(), gen);
        for (size_t j=0; j<graph_size; j++)
          b.insert(r[j], calc_cost(i,r[j]));
        Bk.push_back(b);
      }

      size_t updated(0);
      for (size_t c=0; c<15; c++) {
        kNNGraph Rk = reverse(Bk);
        kNNGraph Mk = merge(Bk, Rk);
        for (size_t i=0; i<n_vertices; i++) {
          for (auto& v: Mk[i]) {
            for (auto& u: Mk[v.i]) {
              cost_t&& c = calc_cost(i, u.i);
              updated += Bk[i].insert(u.i, c);
            }
          }
        }
        printf("# %ld updates in iteration %ld.\n", updated, c);
        if (updated==0) break;
        updated = 0;
      }
    }


    const void
    show_vertices() const
    {
      for (size_t i=0; i<n_vertices; i++)
        printf("%f %f %f\n", V[i].x, V[i].y, V[i].z);
    }

    const void
    neighbor_graph() const
    { neighbor_graph(graph_size); }

    const void
    neighbor_graph(const size_t n_neibors) const
    {
      graph G;
      for (size_t i=0; i<n_vertices; i++) {
        size_t N = 0;
        for (auto& node: Bk[i]) {
          node_t u = ((node_t)i<node.i)?i:node.i;
          node_t v = ((node_t)i<node.i)?node.i:i;
          G.insert({u, v});
          N++;
          if (N==n_neibors) break;
        }
      }
      for (auto& g: G) {
        printf("%f %f %f ",  V[g.u].x, V[g.u].y, V[g.u].z);
        printf("%f %f %f\n", V[g.v].x, V[g.v].y, V[g.v].z);
      }
      printf("# graph size: %ld\n", G.size());
    }

  private:
    const kNNGraph
    reverse(kNNGraph& Bk) const
    {
      kNNGraph Rk;
      for (size_t i=0; i<n_vertices; i++)
        Rk.push_back(kNNSet());
      for (size_t i=0; i<n_vertices; i++) {
        for (auto& node: Bk[i]) {
          Rk[node.i].insert(i, node.cost);
        }
      }
      return Rk;
    }


    const kNNGraph
    merge(const kNNGraph& Bk, const kNNGraph& Rk) const
    {
      kNNGraph Mk;
      for (size_t i=0; i<n_vertices; i++) {
        Mk.push_back(kNNSet());
        for (auto& node: Bk[i]) Mk[i].insert(node.i, node.cost);
        for (auto& node: Rk[i]) Mk[i].insert(node.i, node.cost);
      }
      return Mk;
    }

    const cost_t
    euclidean_sq(const vertex& v, const vertex& w) const
    {
      return std::pow(v.x-w.x,2)+std::pow(v.y-w.y,2)+std::pow(v.z-w.z,2);
    }

    const cost_t
    manhattan(const vertex& v, const vertex& w) const
    {
      return std::abs(v.x-w.x)+std::abs(v.y-w.y)+std::abs(v.z-w.z);
    }

    const cost_t
    calc_cost(const node_t& i, const node_t& j) const
    {
      return euclidean_sq(V[i], V[j]);
    }

    const size_t n_vertices;
    const size_t graph_size;
    vertices V;
    kNNGraph Bk;
  };
}

#endif  // __MINIMALKNN_H_INCLUDE
