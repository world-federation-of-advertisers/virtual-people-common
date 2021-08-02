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

#ifndef SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_NOT_FILTER_H_
#define SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_NOT_FILTER_H_

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

// The implementation of field filter when op is NOT in @config.
class NotFilter : public FieldFilter {
 public:
  // Always use FieldFilter::New.
  // Users should never call NotFilter::New or any constructor directly.
  //
  // Returns error status if any of the following happens:
  //    @config.op is not NOT.
  //    @config.sub_filters is empty.
  //    Any of @config.sub_filters is invalid to create a FieldFilter.
  static absl::StatusOr<std::unique_ptr<NotFilter>> New(
      const google::protobuf::Descriptor* descriptor,
      const FieldFilterProto& config);

  explicit NotFilter(std::unique_ptr<FieldFilter> and_filter)
      : and_filter_(std::move(and_filter)) {}

  NotFilter(const NotFilter&) = delete;
  NotFilter& operator=(const NotFilter&) = delete;

  // Returns false when all the @config.sub_filters pass.
  // Otherwise, returns true.
  bool IsMatch(const google::protobuf::Message& message) const override;

 private:
  // A field filter represents the AND of all the sub_filters.
  // The output of this NotFilter should be the reverse of the output of
  // and_filter_.
  std::unique_ptr<FieldFilter> and_filter_;
};

}  // namespace wfa_virtual_people

#endif  // SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_NOT_FILTER_H_
