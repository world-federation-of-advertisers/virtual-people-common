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

#include "wfa/virtual_people/common/field_filter/gt_filter.h"

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/measurement/common/macros.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"
#include "wfa/virtual_people/common/field_filter/utils/field_util.h"
#include "wfa/virtual_people/common/field_filter/utils/integer_comparator.h"

namespace wfa_virtual_people {

absl::StatusOr<std::unique_ptr<GtFilter>> GtFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  if (config.op() != FieldFilterProto::GT) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Op must be GT. Input FieldFilterProto: ", config.DebugString()));
  }
  if (!config.has_name()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Name must be set. Input FieldFilterProto: ", config.DebugString()));
  }
  if (!config.has_value()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Value must be set. Input FieldFilterProto: ", config.DebugString()));
  }
  ASSIGN_OR_RETURN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(descriptor, config.name()));
  ASSIGN_OR_RETURN(
      std::unique_ptr<IntegerComparator> comparator,
      IntegerComparator::New(std::move(field_descriptors), config.value()));
  return absl::make_unique<GtFilter>(std::move(comparator));
}

bool GtFilter::IsMatch(const google::protobuf::Message& message) const {
  return comparator_->Compare(message) == IntegerCompareResult::GREATER_THAN;
}

}  // namespace wfa_virtual_people
