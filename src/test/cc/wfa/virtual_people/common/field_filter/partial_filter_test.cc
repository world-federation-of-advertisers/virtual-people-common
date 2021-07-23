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

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "gmock/gmock.h"
#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "src/test/cc/testutil/matchers.h"
#include "src/test/cc/testutil/status_macros.h"
#include "src/test/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {
namespace {

using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProto;

TEST(PartialFilterTest, TestNoName) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        op: PARTIAL
        sub_filters { name: "int32_value" op: EQUAL value: "1" }
        sub_filters { name: "int64_value" op: EQUAL value: "1" }
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(PartialFilterTest, TestDisallowedRepeated) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b"
        op: PARTIAL
        sub_filters { name: "int32_values" op: EQUAL value: "1" }
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(PartialFilterTest, TestDisallowedRepeatedInThePath) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "repeated_proto_a.b"
        op: PARTIAL
        sub_filters { name: "int32_value" op: EQUAL value: "1" }
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(PartialFilterTest, TestNoSubFilters) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      name: "a.b" op: PARTIAL
                                                    )pb",
                                                    &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(PartialFilterTest, TestMatch) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b"
        op: PARTIAL
        sub_filters { name: "int32_value" op: EQUAL value: "1" }
        sub_filters { name: "int64_value" op: EQUAL value: "1" }
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { int32_value: 1 int64_value: 1 } }
      )pb",
      &test_proto));
  EXPECT_TRUE(field_filter->IsMatch(test_proto));
}

TEST(PartialFilterTest, TestNotMatch) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b"
        op: PARTIAL
        sub_filters { name: "int32_value" op: EQUAL value: "1" }
        sub_filters { name: "int64_value" op: EQUAL value: "1" }
      )pb",
      &field_filter_proto));
  ASSERT_OK_AND_ASSIGN(
      std::unique_ptr<FieldFilter> field_filter,
      FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto));

  TestProto test_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { int32_value: 1 int64_value: 2 } }
      )pb",
      &test_proto));
  EXPECT_FALSE(field_filter->IsMatch(test_proto));
}

}  // namespace
}  // namespace wfa_virtual_people
