// Copyright 2021 The Cross-Media Measurement Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_EQUAL_FILTER_H_
#define SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_EQUAL_FILTER_H_

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

// The implementation of field filter when op is EQUAL in @config.
class EqualFilter : public FieldFilter {
 public:
  // Always use FieldFilter::New.
  // Users should never call EqualFilter::New or any constructor directly.
  //
  // Returns error status if any of the following happens:
  // * @config.op is not EQUAL.
  // * @config.name is not set.
  // * Any field of the path represented by @config.name is repeated field.
  // * @config.value is not set.
  //
  // @config.value will be casted to the type of the field represented by
  // @config.name.
  static absl::StatusOr<std::unique_ptr<EqualFilter>> New(
      const google::protobuf::Descriptor* descriptor,
      const FieldFilterProto& config);

  EqualFilter(const EqualFilter&) = delete;
  EqualFilter& operator=(const EqualFilter&) = delete;

  virtual ~EqualFilter() = default;

  // Returns true when the field represented by @config.name in @message equals
  // to @config.value. Otherwise, returns false.
  virtual bool IsMatch(
      const google::protobuf::Message& message) const override = 0;

 protected:
  EqualFilter() = default;
};

}  // namespace wfa_virtual_people

#endif  // SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_EQUAL_FILTER_H_
