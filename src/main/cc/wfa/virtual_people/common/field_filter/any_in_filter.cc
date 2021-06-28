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

#include "wfa/virtual_people/common/field_filter/any_in_filter.h"

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

// The implementation of AnyInFilter based on the type of field represented by
// @field_descriptors. The supported ValueTypes are
//   int32_t
//   int64_t
//   uint32_t
//   uint64_t
//   bool
//   const google::protobuf::EnumValueDescriptor*
//   const std::string&
template <typename ValueType>
class AnyInFilterImpl : public AnyInFilter {
 public:
  explicit AnyInFilterImpl(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
      ValuesParser<ValueType>&& parser):
      AnyInFilter(std::move(field_descriptors)),
      parser_(std::move(parser)) {}

  bool IsMatch(const google::protobuf::Message& message) const override;

 private:
  ValuesParser<ValueType> parser_;
};

template <typename ValueType>
bool AnyInFilterImpl<ValueType>::IsMatch(
    const google::protobuf::Message& message) const {
  int size = GetSizeOfRepeatedProto(message, field_descriptors_);
  for (int i = 0; i < size; ++i) {
    if (parser_.values.find(GetValueFromRepeatedProto<ValueType>(
            message, field_descriptors_, i)) != parser_.values.end()) {
      return true;
    }
  }
  return false;
}

template <>
bool AnyInFilterImpl<const google::protobuf::EnumValueDescriptor*>::IsMatch(
    const google::protobuf::Message& message) const {
  int size = GetSizeOfRepeatedProto(message, field_descriptors_);
  for (int i = 0; i < size; ++i) {
    if (parser_.values.find(GetValueFromRepeatedProto<
            const google::protobuf::EnumValueDescriptor*>(
            message, field_descriptors_, i)->number()) != parser_.values.end()) {
      return true;
    }
  }
  return false;
}

template <typename ValueType>
absl::StatusOr<std::unique_ptr<AnyInFilterImpl<ValueType>>> CreateFilter(
    std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
    absl::string_view values_str) {
  ASSIGN_OR_RETURN(
      ValuesParser<ValueType> parser,
      BuildValuesParser<ValueType>(values_str));
  return absl::make_unique<AnyInFilterImpl<ValueType>>(
      std::move(field_descriptors), std::move(parser));
}

template <>
absl::StatusOr<std::unique_ptr<AnyInFilterImpl<
    const google::protobuf::EnumValueDescriptor*>>> CreateFilter(
    std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
    absl::string_view values_str) {
  ASSIGN_OR_RETURN(
      ValuesParser<const google::protobuf::EnumValueDescriptor*> parser,
      BuildEnumValuesParser(
          field_descriptors.back()->enum_type(), values_str));
  return absl::make_unique<AnyInFilterImpl<
      const google::protobuf::EnumValueDescriptor*>>(
      std::move(field_descriptors), std::move(parser));
}

}  // namespace

absl::StatusOr<std::unique_ptr<AnyInFilter>> AnyInFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  if (config.op() != FieldFilterProto::ANY_IN) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Op must be ANY_IN. Input FieldFilterProto: ", config.DebugString()));
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
      GetFieldFromProto(descriptor, config.name(), /*allow_repeated = */true));

  if (!field_descriptors.back()->is_repeated()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Name must represent a repeated field. Input FieldFilterProto: ",
        config.DebugString()));
  }

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
          "Unsupported field type for ANY_IN filter. Input FieldFilterProto: ",
          config.DebugString()));
  }
}

}  // namespace wfa_virtual_people
