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

// This function is required to test the FieldFilter still works when the
// FieldFilterProto is out of scope.
absl::StatusOr<std::unique_ptr<FieldFilter>> FilterFromProtoText(
    absl::string_view proto_text) {
  FieldFilterProto field_filter_proto;
  EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
      std::string(proto_text), &field_filter_proto));
  return FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto);
}

TEST(HasFilterTest, TestNoName) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      op: HAS
                                                    )pb",
                                                    &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(HasFilterTest, TestInvalidName) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "a.b.invalid_field" op: HAS
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(HasFilterTest, TestDisallowedRepeatedInThePath) {
  FieldFilterProto field_filter_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        name: "repeated_proto_a.b.int32_value" op: HAS
      )pb",
      &field_filter_proto));
  EXPECT_THAT(FieldFilter::New(TestProto().GetDescriptor(), field_filter_proto)
                  .status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(HasFilterTest, TestNonRepeated) {
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<FieldFilter> field_filter,
                       FilterFromProtoText(R"pb(
                         name: "a.b.int32_value" op: HAS
                       )pb"));

  TestProto test_proto_1;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int32_value: 1 } }
                                                    )pb",
                                                    &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int64_value: 1 } }
                                                    )pb",
                                                    &test_proto_2));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));
}

TEST(HasFilterTest, TestRepeated) {
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<FieldFilter> field_filter,
                       FilterFromProtoText(R"pb(
                         name: "a.b.int32_values" op: HAS
                       )pb"));

  TestProto test_proto_1;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { int32_values: 1 } }
      )pb",
      &test_proto_1));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_1));

  TestProto test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { int32_values: 1 int32_values: 2 } }
      )pb",
      &test_proto_2));
  EXPECT_TRUE(field_filter->IsMatch(test_proto_2));

  TestProto test_proto_3;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(R"pb(
                                                      a { b { int32_value: 1 } }
                                                    )pb",
                                                    &test_proto_3));
  EXPECT_FALSE(field_filter->IsMatch(test_proto_3));

  TestProto test_proto_4;
  EXPECT_FALSE(field_filter->IsMatch(test_proto_4));
}

}  // namespace
}  // namespace wfa_virtual_people
