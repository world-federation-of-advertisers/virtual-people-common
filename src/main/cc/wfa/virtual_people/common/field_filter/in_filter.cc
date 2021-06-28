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

#include "wfa/virtual_people/common/field_filter/in_filter.h"

#include "absl/container/flat_hash_set.h"
#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/measurement/common/macros.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"
#include "wfa/virtual_people/common/field_filter/utils/field_util.h"
#include "wfa/virtual_people/common/field_filter/utils/values_parser.h"

namespace wfa_virtual_people {

namespace {

// The implementation of InFilter based on the type of field represented by
// @field_descriptors. The supported ValueTypes are
//   int32_t
//   int64_t
//   uint32_t
//   uint64_t
//   bool
//   const google::protobuf::EnumValueDescriptor*
//   const std::string&
template <typename ValueType>
class InFilterImpl : public InFilter {
 public:
  explicit InFilterImpl(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
      ValuesParser<ValueType>&& parser):
      InFilter(std::move(field_descriptors)),
      parser_(std::move(parser)) {}

  bool IsMatch(const google::protobuf::Message& message) const override;

 private:
  ValuesParser<ValueType> parser_;
};

template <typename ValueType>
bool InFilterImpl<ValueType>::IsMatch(
    const google::protobuf::Message& message) const {
  ValueType value = GetValueFromProto<ValueType>(message, field_descriptors_);
  return parser_.values.find(value) != parser_.values.end();
}

template <>
bool InFilterImpl<const google::protobuf::EnumValueDescriptor*>::IsMatch(
    const google::protobuf::Message& message) const {
  const google::protobuf::EnumValueDescriptor* value =
      GetValueFromProto<const google::protobuf::EnumValueDescriptor*>(
          message, field_descriptors_);
  return parser_.values.find(value->number()) != parser_.values.end();
}

template <typename ValueType>
absl::StatusOr<std::unique_ptr<InFilterImpl<ValueType>>> CreateFilter(
    std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
    absl::string_view values_str) {
  ASSIGN_OR_RETURN(
      ValuesParser<ValueType> parser,
      BuildValuesParser<ValueType>(values_str));
  return absl::make_unique<InFilterImpl<ValueType>>(
      std::move(field_descriptors), std::move(parser));
}

template <>
absl::StatusOr<std::unique_ptr<InFilterImpl<
    const google::protobuf::EnumValueDescriptor*>>> CreateFilter(
    std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
    absl::string_view values_str) {
  ASSIGN_OR_RETURN(
      ValuesParser<const google::protobuf::EnumValueDescriptor*> parser,
      BuildEnumValuesParser(
          field_descriptors.back()->enum_type(), values_str));
  return absl::make_unique<InFilterImpl<
      const google::protobuf::EnumValueDescriptor*>>(
      std::move(field_descriptors), std::move(parser));
}

}  // namespace

absl::StatusOr<std::unique_ptr<InFilter>> InFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  if (config.op() != FieldFilterProto::IN) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Op must be IN. Input FieldFilterProto: ", config.DebugString()));
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

  switch (field_descriptors.back()->cpp_type()) {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
      return CreateFilter<int32_t>(
          std::move(field_descriptors), config.value());
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
      return CreateFilter<int64_t>(
          std::move(field_descriptors), config.value());
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
      return CreateFilter<uint32_t>(
          std::move(field_descriptors), config.value());
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
      return CreateFilter<uint64_t>(
          std::move(field_descriptors), config.value());
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
      return CreateFilter<bool>(
          std::move(field_descriptors), config.value());
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
      return CreateFilter<const google::protobuf::EnumValueDescriptor*>(
          std::move(field_descriptors), config.value());
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
      return CreateFilter<const std::string&>(
          std::move(field_descriptors), config.value());
    default:
      return absl::InvalidArgumentError(absl::StrCat(
          "Unsupported field type for IN filter. Input FieldFilterProto: ",
          config.DebugString()));
  }
}

}  // namespace wfa_virtual_people
