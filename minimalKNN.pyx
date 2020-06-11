#!/usr/bin/env cython
# -*- coding: utf-8 -*-
from libcpp.vector cimport vector as vec
from libcpp.set cimport set as cset
from libcpp.list cimport list as clist
from numpy cimport ndarray
import numpy as np


cdef extern from 'minimalKNN.h' namespace 'minimalKNN':

  ## a three-dimensional vertex.
  ctypedef struct vertex:
    double x
    double y
    double z
  ## a list of three-dimensional vertices.
  ctypedef vec[vertex] vertices

  ## a unique sequential number of a vertex.
  ctypedef long node_t
  ## a list of node numbers.
  ctypedef vec[node_t] node_list

  ## a edge specified by a pair of node numbers.
  ctypedef struct edge:
    node_t u
    node_t v
  ## a unique set of edges.
  ctypedef cset[edge] graph

  ## k-NN builder
  cdef cppclass kNNBuilder:
    ##kNNBuilder()
    kNNBuilder(const vertices, const int)
    const void print_vertices() const
    const void print_nng(const int) const
    const vertices get_vertices() const
    const graph neighbor_graph(const int) const


cdef extern from *:
  pass


def construct_kNN(ndarray pool, int graph_size = 10):
  cdef vertices V
  for v in pool: V.push_back(vertex(v[0], v[1], v[2]))
  cdef kNNBuilder* pkNN = new kNNBuilder(V, graph_size)
  cdef graph g = pkNN.neighbor_graph(1)
  return [(e.u,e.v) for e in g]


def simple_solver_test(int n_size=200, int n_neighbor=1,
                       int graph_size=10):
  ''' Functional test with a simple situation.

  Put `group` line segments composed of `frame` elementary segments in the
  2000 x 2000 x 2000 space. The loci, lengths, and directions of the line
  segments are randomly selected. The line segments are slightly affected
  by disturbance. The degree of the disturbance is given by `scatter`.
  Additionally, `jammer` obstacle segments are appended.

  This function calls the `solve` routine, which will search line segments
  in the space. The function prints the starting and terminal points of the
  all the elementary line segments, the starting and terminal points of the
  detected line segments, the default search parameter, the number of the
  detected line segments, and the elapsed time in milliseconds.

  Parameters:
    group (int): the number of line segments.
    frame (int): the number of elements composing a line segment.
    jammer (int): the number of obstacle segments.
    scatter (double): the scatter of line segments.
  '''
  from datetime import datetime
  t0 = datetime.now()
  cdef vertices V
  obj = np.random.uniform(-1,1,size=(n_size,3))
  for v in obj: V.push_back(vertex(v[0], v[1], v[2]))
  cdef kNNBuilder* pkNN = new kNNBuilder(V, graph_size)
  cdef graph g = pkNN.neighbor_graph(n_neighbor)
  t1 = datetime.now()
  print('# found {} segments'.format(g.size()))
  print('# elapsed time: {}ms'.format((t1-t0).total_seconds()*1e3))

  import matplotlib.pyplot as plt
  from mpl_toolkits.mplot3d import Axes3D
  from mpl_toolkits.mplot3d.art3d import Line3DCollection as lc
  fig = plt.figure()
  ax  = fig.add_subplot(111, projection='3d')
  ax.scatter(obj[:,0],obj[:,1],obj[:,2])
  edges = lc([(obj[e.u,:],obj[e.v,:]) for e in g], color=(1,0,0,0.5))
  ax.add_collection(edges)
  plt.show()
