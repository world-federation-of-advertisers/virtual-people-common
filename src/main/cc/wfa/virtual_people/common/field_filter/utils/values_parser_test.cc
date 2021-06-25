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

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "gmock/gmock.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "gtest/gtest.h"
#include "src/main/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
#include "src/test/cc/testutil/matchers.h"
#include "src/test/cc/testutil/status_macros.h"

namespace wfa_virtual_people {
namespace {

using ::testing::UnorderedElementsAre;
using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProtoB;

TEST(ValuesParserTest, TestInt32) {
  ASSERT_OK_AND_ASSIGN(
      ValuesParser<int32_t> parser, BuildValuesParser<int32_t>("1,2,1"));
  EXPECT_THAT(parser.values, UnorderedElementsAre(1, 2));
}

TEST(ValuesParserTest, TestInt32Invalid) {
  EXPECT_THAT(
      BuildValuesParser<int32_t>("1,a,1").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ValuesParserTest, TestInt64) {
  ASSERT_OK_AND_ASSIGN(
      ValuesParser<int64_t> parser, BuildValuesParser<int64_t>("1,2,1"));
  EXPECT_THAT(parser.values, UnorderedElementsAre(1, 2));
}

TEST(ValuesParserTest, TestInt64Invalid) {
  EXPECT_THAT(
      BuildValuesParser<int64_t>("1,a,1").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ValuesParserTest, TestUInt32) {
  ASSERT_OK_AND_ASSIGN(
      ValuesParser<uint32_t> parser, BuildValuesParser<uint32_t>("1,2,1"));
  EXPECT_THAT(parser.values, UnorderedElementsAre(1, 2));
}

TEST(ValuesParserTest, TestUInt32Invalid) {
  EXPECT_THAT(
      BuildValuesParser<uint32_t>("1,a,1").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ValuesParserTest, TestUInt64) {
  ASSERT_OK_AND_ASSIGN(
      ValuesParser<uint64_t> parser, BuildValuesParser<uint64_t>("1,2,1"));
  EXPECT_THAT(parser.values, UnorderedElementsAre(1, 2));
}

TEST(ValuesParserTest, TestUInt64Invalid) {
  EXPECT_THAT(
      BuildValuesParser<uint64_t>("1,a,1").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ValuesParserTest, TestBool) {
  ASSERT_OK_AND_ASSIGN(
      ValuesParser<bool> parser, BuildValuesParser<bool>("true,false,true"));
  EXPECT_THAT(parser.values, UnorderedElementsAre(true, false));
}

TEST(ValuesParserTest, TestBoolInvalid) {
  EXPECT_THAT(
      BuildValuesParser<bool>("true,a,false").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ValuesParserTest, TestString) {
  ASSERT_OK_AND_ASSIGN(
      ValuesParser<const std::string&> parser,
      BuildValuesParser<const std::string&>("a,b,a"));
  EXPECT_THAT(parser.values, UnorderedElementsAre("a", "b"));
}

TEST(ValuesParserTest, TestEnum) {
  ASSERT_OK_AND_ASSIGN(
      ValuesParser<const google::protobuf::EnumValueDescriptor*> parser_1,
      BuildEnumValuesParser(TestProtoB::TestEnum_descriptor(), "1,2,1"));
  EXPECT_THAT(parser_1.values, UnorderedElementsAre(1, 2));

  ASSERT_OK_AND_ASSIGN(
      ValuesParser<const google::protobuf::EnumValueDescriptor*> parser_2,
      BuildEnumValuesParser(TestProtoB::TestEnum_descriptor(),
                            "TEST_ENUM_1,TEST_ENUM_2,TEST_ENUM_1"));
  EXPECT_THAT(parser_2.values, UnorderedElementsAre(1, 2));
}

TEST(ValuesParserTest, TestEnumInvalid) {
  EXPECT_THAT(
      BuildEnumValuesParser(
          TestProtoB::TestEnum_descriptor(), "1,a,1").status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

}  // namespace
}  // namespace wfa_virtual_people
