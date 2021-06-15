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

#ifndef WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_LT_FILTER_H_
#define WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_LT_FILTER_H_

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"
#include "wfa/virtual_people/common/field_filter/utils/integer_comparator.h"

namespace wfa_virtual_people {

// The implementation of field filter when op is LT in @config.
class LtFilter : public FieldFilter {
 public:
  // Always use FieldFilter::New.
  // Users should never call LtFilter::New or any constructor directly.
  //
  // Returns error status if any of the following happens:
  // * @config.op is not LT.
  // * @config.name is not set.
  // * @config.name refers to a non-integer field.
  // * Any field of the path represented by @config.name is repeated field.
  // * @config.value is not set.
  //
  // @config.value will be casted to the type of the field represented by
  // @config.name.
  static absl::StatusOr<std::unique_ptr<LtFilter>> New(
      const google::protobuf::Descriptor* descriptor,
      const FieldFilterProto& config);

  explicit LtFilter(std::unique_ptr<IntegerComparator> comparator):
      comparator_(std::move(comparator)) {}

  LtFilter(const LtFilter&) = delete;
  LtFilter& operator=(const LtFilter&) = delete;

  // Returns true when the field represented by @config.name in @message is
  // less than @config.value. Otherwise, returns false.
  bool IsMatch(const google::protobuf::Message& message) const override;

 private:
  std::unique_ptr<IntegerComparator> comparator_;
};

}  // namespace wfa_virtual_people

#endif  // WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_LT_FILTER_H_
