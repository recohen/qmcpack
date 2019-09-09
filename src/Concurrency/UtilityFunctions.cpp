////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source
// License.  See LICENSE file in top directory for details.
//
// Copyright (c) 2019 QMCPACK developers.
//
// File developed by:
// Peter Doak, doakpw@ornl.gov, Oak Ridge National Lab
//
// File created by:
// Peter Doak, doakpw@ornl.gov, Oak Ridge National Lab
////////////////////////////////////////////////////////////////////////////////

#ifndef QMCPLUSPLUS_CONCURRENCY_INFO_HPP
#define QMCPLUSPLUS_CONCURRENCY_INFO_HPP
/** @file
 *  @brief Abstraction of information on threading environment
 */

#include <omp.h>
#include <thread>

namespace qmcplusplus
{
namespace Concurrency
{
using qmcplusplus::Threading;

template<Threading TT = Threading::OPENMP>
class OverrideMaxThreads;

template<>
class OverrideMaxThreads<Threading::OPENMP>
{
private:
  int original_max_threads_;
public:
  OverrideMaxThreads(int max_threads)
  {
    original_max_threads_ = omp_get_max_threads();
    omp_set_max_threads(max_threads);
  }
  
  ~OverrideMaxThreads()
    {
      omp_set_max_threads(original_max_threads_);
    }
}

template<Threading TT>
class OverrideMaxThreads
{
  OverrideMaxThreads(int max_threads) {}
}

}
}
