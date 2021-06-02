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

#include "wfa/virtual_people/common/field_filter/field_filter.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/measurement/common/macros.h"
#include "wfa/virtual_people/common/field_filter/and_filter.h"
#include "wfa/virtual_people/common/field_filter/equal_filter.h"
#include "wfa/virtual_people/common/field_filter/not_filter.h"
#include "wfa/virtual_people/common/field_filter/or_filter.h"
#include "wfa/virtual_people/common/field_filter/partial_filter.h"
#include "wfa/virtual_people/common/field_filter/true_filter.h"
#include "wfa/virtual_people/common/field_filter/utils/message_filter_util.h"

namespace wfa_virtual_people {

absl::StatusOr<std::unique_ptr<FieldFilter>> FieldFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  switch (config.op()) {
    case FieldFilterProto::HAS:
      return absl::UnimplementedError("HAS field filter is not implemented.");
    case FieldFilterProto::EQUAL:
      return EqualFilter::New(descriptor, config);
    case FieldFilterProto::GT:
      return absl::UnimplementedError("GT field filter is not implemented.");
    case FieldFilterProto::LT:
      return absl::UnimplementedError("LT field filter is not implemented.");
    case FieldFilterProto::IN:
      return absl::UnimplementedError("IN field filter is not implemented.");
    case FieldFilterProto::REGEXP:
      return absl::UnimplementedError(
          "REGEXP field filter is not implemented.");
    case FieldFilterProto::OR:
      return OrFilter::New(descriptor, config);
    case FieldFilterProto::AND:
      return AndFilter::New(descriptor, config);
    case FieldFilterProto::NOT:
      return NotFilter::New(descriptor, config);
    case FieldFilterProto::PARTIAL:
      return PartialFilter::New(descriptor, config);
    case FieldFilterProto::TRUE:
      return TrueFilter::New(config);
    case FieldFilterProto::ANY_IN:
      return absl::UnimplementedError(
          "ANY_IN field filter is not implemented.");
    default:
      return absl::InvalidArgumentError("Invalid op in field filter.");
  }
}

absl::StatusOr<std::unique_ptr<FieldFilter>> FieldFilter::New(
    const google::protobuf::Message& message) {
  ASSIGN_OR_RETURN(FieldFilterProto config, ConvertMessageToFilter(message));
  return FieldFilter::New(message.GetDescriptor(), config);
}

}  // namespace wfa_virtual_people
