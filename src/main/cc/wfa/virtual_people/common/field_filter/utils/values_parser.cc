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

#include "wfa/virtual_people/common/field_filter/utils/values_parser.h"

#include "absl/container/flat_hash_set.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "common_cpp/macros/macros.h"
#include "google/protobuf/descriptor.h"
#include "wfa/virtual_people/common/field_filter/utils/type_convert_util.h"

namespace wfa_virtual_people {

absl::StatusOr<ParsedValues<const google::protobuf::EnumValueDescriptor*>>
ParseEnumValues(const google::protobuf::EnumDescriptor* descriptor,
                absl::string_view values_str) {
  std::vector<std::string> values_list = absl::StrSplit(values_str, ',');
  ParsedValues<const google::protobuf::EnumValueDescriptor*> parsed_values;
  for (const std::string& value_str : values_list) {
    ASSIGN_OR_RETURN(const google::protobuf::EnumValueDescriptor* value,
                     ConvertToEnum(descriptor, value_str));
    parsed_values.values.insert(value->number());
  }
  return parsed_values;
}

}  // namespace wfa_virtual_people
