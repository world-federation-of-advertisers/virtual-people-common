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

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"

namespace wfa_virtual_people {

absl::StatusOr<std::unique_ptr<FieldFilter>> FieldFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  switch(config.op()) {
    case FieldFilterProto::HAS:
      return absl::UnimplementedError("HAS field filter is not implemented.");
    case FieldFilterProto::EQUAL:
      return absl::UnimplementedError("EQUAL field filter is not implemented.");
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
      return absl::UnimplementedError("OR field filter is not implemented.");
    case FieldFilterProto::AND:
      return absl::UnimplementedError("AND field filter is not implemented.");
    case FieldFilterProto::NOT:
      return absl::UnimplementedError("NOT field filter is not implemented.");
    case FieldFilterProto::PARTIAL:
      return absl::UnimplementedError(
          "PARTIAL field filter is not implemented.");
    case FieldFilterProto::TRUE:
      return absl::UnimplementedError("TRUE field filter is not implemented.");
    case FieldFilterProto::ANY_IN:
      return absl::UnimplementedError(
          "ANY_IN field filter is not implemented.");
    default:
      return absl::InvalidArgumentError("Invalid op in field filter.");
  }
}

}  // namespace wfa_virtual_people
