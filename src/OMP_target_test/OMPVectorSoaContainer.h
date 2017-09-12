////////////////////////////////////////////////////////////////////////////////
//// This file is distributed under the University of Illinois/NCSA Open Source
//// License.  See LICENSE file in top directory for details.
////
//// Copyright (c) 2016 Jeongnim Kim and QMCPACK developers.
////
//// File developed by: Ye Luo, yeluo@anl.gov, Argonne National Laboratory.
////
//// File created by: Ye Luo, yeluo@anl.gov, Argonne National Laboratory.
//////////////////////////////////////////////////////////////////////////////////

#ifndef QMCPLUSPLUS_OMP_VECTOR_SOA_HPP
#define QMCPLUSPLUS_OMP_VECTOR_SOA_HPP

#include "OhmmsSoA/VectorSoaContainer.h"

namespace qmcplusplus
{

template<typename T,  unsigned D>
class OMPVectorSoaContainer:public VectorSoaContainer<T,D>
{
  typedef VectorSoaContainer<T,D> __base;

  private:
  size_t device_id;
  T * vec_ptr;

  public:
  inline OMPVectorSoaContainer(size_t size = 0, size_t id = 0): device_id(id), vec_ptr(nullptr)
  {
    resize(size);
  }

  inline void resize(size_t size)
  {
    if(size!=__base::size())
    {
      if(__base::size()!=0)
      {
        #pragma omp target exit data map(delete:vec_ptr) device(device_id)
        vec_ptr = nullptr;
      }
      __base::resize(size);
      if(size>0)
      {
        vec_ptr = __base::data();
        #pragma omp target enter data map(alloc:vec_ptr[0:__base::nAllocated]) device(device_id)
        std::cout << "mapped already? " << omp_target_is_present(vec_ptr,0) << " address " << vec_ptr << " size " << __base::nAllocated << std::endl;
        exit(1);
      }
    }
  }

  inline void update_to_device() const
  {
    #pragma omp target update to(vec_ptr[0:__base::nAllocated]) device(device_id)
  }

  inline void update_from_device() const 
  {
    #pragma omp target update from(vec_ptr[0:__base::nAllocated]) device(device_id)
  }

  inline ~OMPVectorSoaContainer()
  {
    #pragma omp target exit data map(delete:vec_ptr) device(device_id)
  }

};

}
#endif
