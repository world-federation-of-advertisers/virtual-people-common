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

#ifndef SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_OR_FILTER_H_
#define SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_OR_FILTER_H_

#include <memory>
#include <utility>
#include <vector>

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

// The implementation of field filter when op is OR in @config.
class OrFilter : public FieldFilter {
 public:
  // Always use FieldFilter::New.
  // Users should never call OrFilter::New or any constructor directly.
  //
  // Returns error status if any of the following happens:
  //    @config.op is not OR.
  //    @config.sub_filters is empty.
  //    Any of @config.sub_filters is invalid to create a FieldFilter.
  static absl::StatusOr<std::unique_ptr<OrFilter>> New(
      const google::protobuf::Descriptor* descriptor,
      const FieldFilterProto& config);

  explicit OrFilter(std::vector<std::unique_ptr<FieldFilter>>&& sub_filters)
      : sub_filters_(std::move(sub_filters)) {}

  OrFilter(const OrFilter&) = delete;
  OrFilter& operator=(const OrFilter&) = delete;

  // Returns true when any of the sub_filters passes. Otherwise, returns false.
  bool IsMatch(const google::protobuf::Message& message) const override;

 private:
  std::vector<std::unique_ptr<FieldFilter>> sub_filters_;
};

}  // namespace wfa_virtual_people

#endif  // SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_OR_FILTER_H_
