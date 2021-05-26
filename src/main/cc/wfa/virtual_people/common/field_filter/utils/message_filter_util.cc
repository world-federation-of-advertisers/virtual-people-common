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

#include "wfa/virtual_people/common/field_filter/utils/message_filter_util.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/measurement/common/macros.h"

namespace wfa_virtual_people {

absl::StatusOr<FieldFilterProto> ConvertMessageToFilter(
    const google::protobuf::Message& message) {
  FieldFilterProto filter;
  filter.set_op(FieldFilterProto::AND);

  const google::protobuf::Reflection* reflection = message.GetReflection();
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors;
  reflection->ListFields(message, &field_descriptors);

  for (const google::protobuf::FieldDescriptor* field_descriptor :
          field_descriptors) {
    if (field_descriptor->is_repeated()) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Repeated field in message when converting to FieldFilterProto: ",
          message.DebugString()));
    }
    FieldFilterProto* sub_filter = filter.add_sub_filters();
    sub_filter->set_name(field_descriptor->name());
    switch (field_descriptor->cpp_type()) {
      case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
        {
          sub_filter->set_op(FieldFilterProto::EQUAL);
          sub_filter->set_value(
              std::to_string(reflection->GetInt32(message, field_descriptor)));
          break;
        }
      case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
        {
          sub_filter->set_op(FieldFilterProto::EQUAL);
          sub_filter->set_value(
              std::to_string(reflection->GetInt64(message, field_descriptor)));
          break;
        }
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
        {
          sub_filter->set_op(FieldFilterProto::EQUAL);
          sub_filter->set_value(
              std::to_string(reflection->GetUInt32(message, field_descriptor)));
          break;
        }
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
        {
          sub_filter->set_op(FieldFilterProto::EQUAL);
          sub_filter->set_value(
              std::to_string(reflection->GetUInt64(message, field_descriptor)));
          break;
        }
      case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
        {
          sub_filter->set_op(FieldFilterProto::EQUAL);
          if (reflection->GetBool(message, field_descriptor)) {
            sub_filter->set_value("true");
          } else {
            sub_filter->set_value("false");
          }
          break;
        }
      case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        {
          sub_filter->set_op(FieldFilterProto::EQUAL);
          sub_filter->set_value(
              reflection->GetEnum(message, field_descriptor)->name());
          break;
        }
      case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
        {
          sub_filter->set_op(FieldFilterProto::EQUAL);
          sub_filter->set_value(
              reflection->GetString(message, field_descriptor));
          break;
        }
      case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        {
          sub_filter->set_op(FieldFilterProto::PARTIAL);
          ASSIGN_OR_RETURN(
              *sub_filter->add_sub_filters(),
              ConvertMessageToFilter(
                  reflection->GetMessage(message, field_descriptor)));
          break;
        }
      default:
        return absl::InvalidArgumentError(absl::StrCat(
            "Unsupported field type converting to FieldFilterProto from: ",
            message.DebugString()));
    }
  }
  return filter;
}

}  // namespace wfa_virtual_people
