#!/usr/bin/env cython
# -*- coding: utf-8 -*-
from libcpp.vector cimport vector as vec
from libcpp.list cimport list as clist
from numpy cimport ndarray
import numpy as np


cdef extern from 'minimalKNN.h' namespace 'minimalKNN':

  ctypedef long node_t
  ctypedef vec[node_t] node_list
  ctypedef double cost_t
  ctypedef struct vertex:
    double x
    double y
    double z
  ctypedef vec[vertex] vertices


cdef extern from *:
  pass


def simple_solver_test():
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
  t1 = datetime.now()
  print('# found {} segments'.format(0))
  print('# elapsed time: {}ms'.format((t1-t0).total_seconds()*1e3))
