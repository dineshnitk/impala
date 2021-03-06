/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _HLLUNION_INTERNAL_HPP_
#define _HLLUNION_INTERNAL_HPP_

#include "hll.hpp"

#include "HllSketchImpl.hpp"
#include "HllArray.hpp"
#include "HllUtil.hpp"

#include <stdexcept>
#include <string>

namespace datasketches {

template<typename A>
hll_union_alloc<A>::hll_union_alloc(const int lg_max_k):
  lg_max_k(HllUtil<A>::checkLgK(lg_max_k)),
  gadget(lg_max_k, target_hll_type::HLL_8)
{}

template<typename A>
hll_union_alloc<A> hll_union_alloc<A>::deserialize(const void* bytes, size_t len) {
  hll_sketch_alloc<A> sk(hll_sketch_alloc<A>::deserialize(bytes, len));
  // we're using the sketch's lg_config_k to initialize the union so
  // we can initialize the Union with it as long as it's HLL_8.
  hll_union_alloc<A> hllUnion(sk.get_lg_config_k());
  if (sk.get_target_type() == HLL_8) {
    std::swap(hllUnion.gadget.sketch_impl, sk.sketch_impl);
  } else {
    hllUnion.update(sk);
  }
  return hllUnion;
}

template<typename A>
hll_union_alloc<A> hll_union_alloc<A>::deserialize(std::istream& is) {
  hll_sketch_alloc<A> sk(hll_sketch_alloc<A>::deserialize(is));
  // we're using the sketch's lg_config_k to initialize the union so
  // we can initialize the Union with it as long as it's HLL_8.
  hll_union_alloc<A> hllUnion(sk.get_lg_config_k());
  if (sk.get_target_type() == HLL_8) {    
    std::swap(hllUnion.gadget.sketch_impl, sk.sketch_impl);
  } else {
    hllUnion.update(sk);
  }
  return hllUnion;
}

template<typename A>
static std::ostream& operator<<(std::ostream& os, const hll_union_alloc<A>& hllUnion) {
  return hllUnion.to_string(os, true, true, false, false);
}

template<typename A>
hll_sketch_alloc<A> hll_union_alloc<A>::get_result(target_hll_type target_type) const {
  return hll_sketch_alloc<A>(gadget, target_type);
}

template<typename A>
void hll_union_alloc<A>::update(const hll_sketch_alloc<A>& sketch) {
  if (sketch.is_empty()) return;
  union_impl(sketch, lg_max_k);
}

template<typename A>
void hll_union_alloc<A>::update(hll_sketch_alloc<A>&& sketch) {
  if (sketch.is_empty()) return;
  if (gadget.is_empty() && sketch.get_target_type() == HLL_8 && sketch.get_lg_config_k() <= lg_max_k) {
    if (sketch.get_current_mode() == HLL || sketch.get_lg_config_k() == lg_max_k) {
      gadget = std::move(sketch);
    }
  }
  union_impl(sketch, lg_max_k);
}

template<typename A>
void hll_union_alloc<A>::update(const std::string& datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const uint64_t datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const uint32_t datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const uint16_t datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const uint8_t datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const int64_t datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const int32_t datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const int16_t datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const int8_t datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const double datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const float datum) {
  gadget.update(datum);
}

template<typename A>
void hll_union_alloc<A>::update(const void* data, const size_t length_bytes) {
  gadget.update(data, length_bytes);
}

template<typename A>
void hll_union_alloc<A>::coupon_update(const int coupon) {
  if (coupon == HllUtil<A>::EMPTY) { return; }
  HllSketchImpl<A>* result = gadget.sketch_impl->coupon_update(coupon);
  if (result != gadget.sketch_impl) {
    if (gadget.sketch_impl != nullptr) { gadget.sketch_impl->get_deleter()(gadget.sketch_impl); }
    gadget.sketch_impl = result;
  }
}

template<typename A>
vector_u8<A> hll_union_alloc<A>::serialize_compact() const {
  return gadget.serialize_compact();
}

template<typename A>
vector_u8<A> hll_union_alloc<A>::serialize_updatable() const {
  return gadget.serialize_updatable();
}

template<typename A>
void hll_union_alloc<A>::serialize_compact(std::ostream& os) const {
  return gadget.serialize_compact(os);
}

template<typename A>
void hll_union_alloc<A>::serialize_updatable(std::ostream& os) const {
  return gadget.serialize_updatable(os);
}

template<typename A>
std::ostream& hll_union_alloc<A>::to_string(std::ostream& os, const bool summary,
                                  const bool detail, const bool aux_detail, const bool all) const {
  return gadget.to_string(os, summary, detail, aux_detail, all);
}

template<typename A>
std::string hll_union_alloc<A>::to_string(const bool summary, const bool detail,
                                   const bool aux_detail, const bool all) const {
  return gadget.to_string(summary, detail, aux_detail, all);
}

template<typename A>
double hll_union_alloc<A>::get_estimate() const {
  return gadget.get_estimate();
}

template<typename A>
double hll_union_alloc<A>::get_composite_estimate() const {
  return gadget.get_composite_estimate();
}

template<typename A>
double hll_union_alloc<A>::get_lower_bound(const int num_std_dev) const {
  return gadget.get_lower_bound(num_std_dev);
}

template<typename A>
double hll_union_alloc<A>::get_upper_bound(const int num_std_dev) const {
  return gadget.get_upper_bound(num_std_dev);
}

template<typename A>
int hll_union_alloc<A>::get_compact_serialization_bytes() const {
  return gadget.get_compact_serialization_bytes();
}

template<typename A>
int hll_union_alloc<A>::get_updatable_serialization_bytes() const {
  return gadget.get_updatable_serialization_bytes();
}

template<typename A>
int hll_union_alloc<A>::get_lg_config_k() const {
  return gadget.get_lg_config_k();
}

template<typename A>
void hll_union_alloc<A>::reset() {
  gadget.reset();
}

template<typename A>
bool hll_union_alloc<A>::is_compact() const {
  return gadget.is_compact();
}

template<typename A>
bool hll_union_alloc<A>::is_empty() const {
  return gadget.is_empty();
}

template<typename A>
bool hll_union_alloc<A>::is_out_of_order_flag() const {
  return gadget.is_out_of_order_flag();
}

template<typename A>
hll_mode hll_union_alloc<A>::get_current_mode() const {
  return gadget.get_current_mode();
}

template<typename A>
bool hll_union_alloc<A>::is_estimation_mode() const {
  return gadget.is_estimation_mode();
}

template<typename A>
int hll_union_alloc<A>::get_serialization_version() const {
  return HllUtil<A>::SER_VER;
}

template<typename A>
target_hll_type hll_union_alloc<A>::get_target_type() const {
  return target_hll_type::HLL_8;
}

template<typename A>
int hll_union_alloc<A>::get_max_serialization_bytes(const int lg_k) {
  return hll_sketch_alloc<A>::get_max_updatable_serialization_bytes(lg_k, target_hll_type::HLL_8);
}

template<typename A>
double hll_union_alloc<A>::get_rel_err(const bool upper_bound, const bool unioned,
                           const int lg_config_k, const int num_std_dev) {
  return HllUtil<A>::getRelErr(upper_bound, unioned, lg_config_k, num_std_dev);
}

template<typename A>
HllSketchImpl<A>* hll_union_alloc<A>::copy_or_downsample(const HllSketchImpl<A>* src_impl, const int tgt_lg_k) {
  if (src_impl->getCurMode() != HLL) {
    throw std::logic_error("Attempt to downsample non-HLL sketch");
  }
  const HllArray<A>* src = static_cast<const HllArray<A>*>(src_impl);
  const int src_lg_k = src->getLgConfigK();
  if (src_lg_k <= tgt_lg_k) {
    return src->copyAs(HLL_8);
  }
  typedef typename std::allocator_traits<A>::template rebind_alloc<Hll8Array<A>> hll8Alloc;
  Hll8Array<A>* tgtHllArr = new (hll8Alloc().allocate(1)) Hll8Array<A>(tgt_lg_k, false);
  tgtHllArr->mergeHll(*src);
  //both of these are required for isomorphism
  tgtHllArr->putHipAccum(src->getHipAccum());
  tgtHllArr->putOutOfOrderFlag(src->isOutOfOrderFlag());
  return tgtHllArr;
}

template<typename A>
inline HllSketchImpl<A>* hll_union_alloc<A>::leak_free_coupon_update(HllSketchImpl<A>* impl, const int coupon) {
  HllSketchImpl<A>* result = impl->couponUpdate(coupon);
  if (result != impl) {
    impl->get_deleter()(impl);
  }
  return result;
}

template<typename A>
void hll_union_alloc<A>::union_impl(const hll_sketch_alloc<A>& sketch, const int lg_max_k) {
  const HllSketchImpl<A>* src_impl = sketch.sketch_impl; //default
  HllSketchImpl<A>* dst_impl = gadget.sketch_impl; //default
  if (src_impl->getCurMode() == LIST || src_impl->getCurMode() == SET) {
    if (dst_impl->isEmpty() && src_impl->getLgConfigK() == dst_impl->getLgConfigK()) {
      dst_impl = src_impl->copyAs(HLL_8);
      gadget.sketch_impl->get_deleter()(gadget.sketch_impl); // gadget replaced
    } else {
      const CouponList<A>* src = static_cast<const CouponList<A>*>(src_impl);
      for (auto coupon: *src) {
        dst_impl = leak_free_coupon_update(dst_impl, coupon); //assignment required
      }
    }
  } else if (!dst_impl->isEmpty()) { // src is HLL
    if (dst_impl->getCurMode() == LIST || dst_impl->getCurMode() == SET) {
      // swap so that src is LIST or SET, tgt is HLL
      // use lg_max_k because LIST has effective K of 2^26
      const CouponList<A>* src = static_cast<const CouponList<A>*>(dst_impl);
      dst_impl = copy_or_downsample(src_impl, lg_max_k);
      static_cast<Hll8Array<A>*>(dst_impl)->mergeList(*src);
      gadget.sketch_impl->get_deleter()(gadget.sketch_impl); // gadget replaced
    } else { // gadget is HLL
      if (src_impl->getLgConfigK() < dst_impl->getLgConfigK()) {
        dst_impl = copy_or_downsample(dst_impl, sketch.get_lg_config_k());
        gadget.sketch_impl->get_deleter()(gadget.sketch_impl); // gadget replaced
      }
      const HllArray<A>* src = static_cast<const HllArray<A>*>(src_impl);
      static_cast<Hll8Array<A>*>(dst_impl)->mergeHll(*src);
      dst_impl->putOutOfOrderFlag(true);
    }
  } else { // src is HLL, gadget is empty
    dst_impl = copy_or_downsample(src_impl, lg_max_k);
    gadget.sketch_impl->get_deleter()(gadget.sketch_impl); // gadget replaced
  }
  gadget.sketch_impl = dst_impl;
}

}

#endif // _HLLUNION_INTERNAL_HPP_
