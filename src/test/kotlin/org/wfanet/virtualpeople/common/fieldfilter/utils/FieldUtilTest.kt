// Copyright 2020 The Cross-Media Measurement Authors
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

package org.wfanet.virtualpeople.common.fieldfilter.utils

import com.google.common.truth.extensions.proto.ProtoTruth.assertThat
import com.google.protobuf.Descriptors.EnumValueDescriptor
import com.google.protobuf.Message
import com.google.protobuf.Message.Builder
import com.google.protobuf.MessageOrBuilder
import kotlin.test.assertEquals
import kotlin.test.assertFailsWith
import kotlin.test.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.JUnit4
import org.wfanet.virtualpeople.common.test.TestProto
import org.wfanet.virtualpeople.common.test.TestProtoB
import org.wfanet.virtualpeople.common.test.testProto
import org.wfanet.virtualpeople.common.test.testProtoA
import org.wfanet.virtualpeople.common.test.testProtoB

@RunWith(JUnit4::class)
class FieldUtilTest {

  /**
   * Helper class to test getFieldAndValue. testProto1 and testProto2 are either a TestProto or
   * TestProto.Builder
   */
  fun testGetFieldAndValue(testProto1: MessageOrBuilder, testProto2: MessageOrBuilder) {
    // Test int32.
    val int32ValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.int32_value")
    assertEquals(ProtoFieldValue(true, 2), getValueFromProto(testProto2, int32ValueFields))
    // Test int64.
    val int64ValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.int64_value")
    assertEquals(ProtoFieldValue(true, 2L), getValueFromProto(testProto2, int64ValueFields))
    // Test uint32.
    val uint32ValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.uint32_value")
    assertEquals(ProtoFieldValue(true, 2), getValueFromProto(testProto2, uint32ValueFields))
    // Test uint64.
    val uint64ValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.uint64_value")
    assertEquals(ProtoFieldValue(true, 2L), getValueFromProto(testProto2, uint64ValueFields))
    // Test float.
    val floatValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.float_value")
    assertEquals(ProtoFieldValue(true, 2.0f), getValueFromProto(testProto2, floatValueFields))
    // Test double.
    val doubleValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.double_value")
    assertEquals(ProtoFieldValue(true, 2.0), getValueFromProto(testProto2, doubleValueFields))
    // Test bool.
    val booleanValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.bool_value")
    assertEquals(
      ProtoFieldValue(isSet = true, value = false),
      getValueFromProto(testProto2, booleanValueFields)
    )
    // Test enum.
    val enumValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.enum_value")
    assertEquals(
      ProtoFieldValue(true, TestProtoB.TestEnum.TEST_ENUM_2.valueDescriptor),
      getValueFromProto<EnumValueDescriptor>(testProto2, enumValueFields)
    )
    // Test string.
    val stringValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b.string_value")
    assertEquals(ProtoFieldValue(true, "string2"), getValueFromProto(testProto2, stringValueFields))
  }

  @Test
  fun `get field and value`() {
    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 1
          int64Value = 1
          uint32Value = 1
          uint64Value = 1
          floatValue = 1.0F
          doubleValue = 1.0
          boolValue = true
          enumValue = TestProtoB.TestEnum.TEST_ENUM_1
          stringValue = "string1"
        }
      }
    }

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 2
          int64Value = 2
          uint32Value = 2
          uint64Value = 2
          floatValue = 2.0F
          doubleValue = 2.0
          boolValue = false
          enumValue = TestProtoB.TestEnum.TEST_ENUM_2
          stringValue = "string2"
        }
      }
    }

    val messageValueFields = getFieldFromProto(testProto1.descriptorForType, "a.b")

    testGetFieldAndValue(testProto1, testProto2)
    // Test message.
    assertEquals(
      ProtoFieldValue(true, testProto2.a.b),
      getValueFromProto(testProto2, messageValueFields)
    )

    testGetFieldAndValue(testProto1, testProto2.toBuilder())
    // Test message.
    assertEquals(
      ProtoFieldValue(true, testProto2.toBuilder().a.b),
      getValueFromProto(testProto2.toBuilder(), messageValueFields)
    )

    testGetFieldAndValue(testProto1.toBuilder(), testProto2.toBuilder())
    // Test message.
    assertEquals(
      ProtoFieldValue(true, testProto2.toBuilder().a.b),
      getValueFromProto(testProto2.toBuilder(), messageValueFields)
    )
  }

  @Test
  fun `get value for unset field`() {
    val testProto = testProto {}

    // Test int32.
    val int32ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.int32_value")
    assertEquals(ProtoFieldValue(false, 0), getValueFromProto(testProto, int32ValueFields))
    // Test int64.
    val int64ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.int64_value")
    assertEquals(ProtoFieldValue(false, 0L), getValueFromProto(testProto, int64ValueFields))
    // Test uint32.
    val uint32ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.uint32_value")
    assertEquals(ProtoFieldValue(false, 0), getValueFromProto(testProto, uint32ValueFields))
    // Test uint64.
    val uint64ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.uint64_value")
    assertEquals(ProtoFieldValue(false, 0L), getValueFromProto(testProto, uint64ValueFields))
    // Test float.
    val floatValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.float_value")
    assertEquals(ProtoFieldValue(false, 0.0f), getValueFromProto(testProto, floatValueFields))
    // Test double.
    val doubleValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.double_value")
    assertEquals(ProtoFieldValue(false, 0.0), getValueFromProto(testProto, doubleValueFields))
    // Test bool.
    val booleanValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.bool_value")
    assertEquals(
      ProtoFieldValue(isSet = false, value = false),
      getValueFromProto(testProto, booleanValueFields)
    )
    // Test enum.
    val enumValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.enum_value")
    assertEquals(
      ProtoFieldValue(false, TestProtoB.TestEnum.INVALID.valueDescriptor),
      getValueFromProto<EnumValueDescriptor>(testProto, enumValueFields)
    )
    // Test string.
    val stringValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.string_value")
    assertEquals(ProtoFieldValue(false, ""), getValueFromProto(testProto, stringValueFields))
    // Test message.
    val messageValueFields = getFieldFromProto(testProto.descriptorForType, "a.b")
    assertEquals(
      ProtoFieldValue(false, testProtoB {}),
      getValueFromProto(testProto, messageValueFields)
    )
  }

  @Test
  fun `get field and set value`() {
    val testProtoB =
      testProtoB {
          int32Value = 3
          int64Value = 3
          uint32Value = 3
          uint64Value = 3
          floatValue = 3.0F
          doubleValue = 3.0
          boolValue = true
          enumValue = TestProtoB.TestEnum.TEST_ENUM_1
          stringValue = "string3"
        }
        .toBuilder()

    // Test int32.
    val int32ValueFields = getFieldFromProto(testProtoB.descriptorForType, "int32_value")
    setValueToProtoBuilder(testProtoB, int32ValueFields, 4)

    // Test int64.
    val int64ValueFields = getFieldFromProto(testProtoB.descriptorForType, "int64_value")
    setValueToProtoBuilder(testProtoB, int64ValueFields, 4L)
    // Test uint32.
    val uint32ValueFields = getFieldFromProto(testProtoB.descriptorForType, "uint32_value")
    setValueToProtoBuilder(testProtoB, uint32ValueFields, 4)
    // Test uint64.
    val uint64ValueFields = getFieldFromProto(testProtoB.descriptorForType, "uint64_value")
    setValueToProtoBuilder(testProtoB, uint64ValueFields, 4L)
    // Test float.
    val floatValueFields = getFieldFromProto(testProtoB.descriptorForType, "float_value")
    setValueToProtoBuilder(testProtoB, floatValueFields, 4.0f)
    // Test double.
    val doubleValueFields = getFieldFromProto(testProtoB.descriptorForType, "double_value")
    setValueToProtoBuilder(testProtoB, doubleValueFields, 4.0)
    // Test bool.
    val booleanValueFields = getFieldFromProto(testProtoB.descriptorForType, "bool_value")
    setValueToProtoBuilder(testProtoB, booleanValueFields, false)
    // Test enum.
    val enumValueFields = getFieldFromProto(testProtoB.descriptorForType, "enum_value")
    setValueToProtoBuilder(
      testProtoB,
      enumValueFields,
      TestProtoB.TestEnum.TEST_ENUM_2.valueDescriptor
    )
    // Test string.
    val stringValueFields = getFieldFromProto(testProtoB.descriptorForType, "string_value")
    setValueToProtoBuilder(testProtoB, stringValueFields, "string4")

    assertThat(testProtoB.build())
      .isEqualTo(
        testProtoB {
          int32Value = 4
          int64Value = 4
          uint32Value = 4
          uint64Value = 4
          floatValue = 4.0F
          doubleValue = 4.0
          boolValue = false
          enumValue = TestProtoB.TestEnum.TEST_ENUM_2
          stringValue = "string4"
        }
      )
  }

  @Test
  fun `get field and set nested value`() {
    val testProto =
      testProto {
          a = testProtoA {
            b = testProtoB {
              int32Value = 5
              int64Value = 5
              uint32Value = 5
              uint64Value = 5
              floatValue = 5.0F
              doubleValue = 5.0
              boolValue = true
              enumValue = TestProtoB.TestEnum.TEST_ENUM_1
              stringValue = "string5"
            }
          }
        }
        .toBuilder()

    // Test int32.
    val int32ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.int32_value")
    setValueToProtoBuilder(testProto, int32ValueFields, 6)
    // Test int64.
    val int64ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.int64_value")
    setValueToProtoBuilder(testProto, int64ValueFields, 6L)
    // Test uint32.
    val uint32ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.uint32_value")
    setValueToProtoBuilder(testProto, uint32ValueFields, 6)
    // Test uint64.
    val uint64ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.uint64_value")
    setValueToProtoBuilder(testProto, uint64ValueFields, 6L)
    // Test float.
    val floatValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.float_value")
    setValueToProtoBuilder(testProto, floatValueFields, 6.0f)
    // Test double.
    val doubleValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.double_value")
    setValueToProtoBuilder(testProto, doubleValueFields, 6.0)
    // Test bool.
    val booleanValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.bool_value")
    setValueToProtoBuilder(testProto, booleanValueFields, false)
    // Test enum.
    val enumValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.enum_value")
    setValueToProtoBuilder(
      testProto,
      enumValueFields,
      TestProtoB.TestEnum.TEST_ENUM_2.valueDescriptor
    )
    // Test string.
    val stringValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.string_value")
    setValueToProtoBuilder(testProto, stringValueFields, "string6")

    assertThat(testProto.build())
      .isEqualTo(
        testProto {
          a = testProtoA {
            b = testProtoB {
              int32Value = 6
              int64Value = 6
              uint32Value = 6
              uint64Value = 6
              floatValue = 6.0F
              doubleValue = 6.0
              boolValue = false
              enumValue = TestProtoB.TestEnum.TEST_ENUM_2
              stringValue = "string6"
            }
          }
        }
      )
  }

  @Test
  fun `get field and set value for unset parent message`() {
    val testProto = TestProto.newBuilder()

    // Test int32.
    val int32ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.int32_value")
    setValueToProtoBuilder(testProto, int32ValueFields, 6)
    // Test int64.
    val int64ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.int64_value")
    setValueToProtoBuilder(testProto, int64ValueFields, 6L)
    // Test uint32.
    val uint32ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.uint32_value")
    setValueToProtoBuilder(testProto, uint32ValueFields, 6)
    // Test uint64.
    val uint64ValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.uint64_value")
    setValueToProtoBuilder(testProto, uint64ValueFields, 6L)
    // Test float.
    val floatValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.float_value")
    setValueToProtoBuilder(testProto, floatValueFields, 6.0f)
    // Test double.
    val doubleValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.double_value")
    setValueToProtoBuilder(testProto, doubleValueFields, 6.0)
    // Test bool.
    val booleanValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.bool_value")
    setValueToProtoBuilder(testProto, booleanValueFields, false)
    // Test enum.
    val enumValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.enum_value")
    setValueToProtoBuilder(
      testProto,
      enumValueFields,
      TestProtoB.TestEnum.TEST_ENUM_2.valueDescriptor
    )
    // Test string.
    val stringValueFields = getFieldFromProto(testProto.descriptorForType, "a.b.string_value")
    setValueToProtoBuilder(testProto, stringValueFields, "string6")

    assertThat(testProto.build())
      .isEqualTo(
        testProto {
          a = testProtoA {
            b = testProtoB {
              int32Value = 6
              int64Value = 6
              uint32Value = 6
              uint64Value = 6
              floatValue = 6.0F
              doubleValue = 6.0
              boolValue = false
              enumValue = TestProtoB.TestEnum.TEST_ENUM_2
              stringValue = "string6"
            }
          }
        }
      )
  }

  @Test
  fun `invald field name should throw`() {
    val testProto = testProto { a = testProtoA { b = testProtoB { int32Value = 6 } } }

    assertFailsWith<IllegalStateException> { getFieldFromProto(testProto.descriptorForType, "a.c") }
  }

  @Test
  fun `invald sub message name should throw`() {
    val testProto = testProto { a = testProtoA { b = testProtoB { int32Value = 6 } } }

    assertFailsWith<UnsupportedOperationException> {
      getFieldFromProto(testProto.descriptorForType, "a.b.int64_value.c")
    }
  }

  @Test
  fun `get parent message`() {
    val valueFields = getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_value")

    val testProto = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 7
          int64Value = 7
          uint32Value = 7
          uint64Value = 7
          floatValue = 7.0F
          doubleValue = 7.0
          boolValue = true
          enumValue = TestProtoB.TestEnum.TEST_ENUM_1
          stringValue = "string7"
        }
      }
    }

    val parent = getParentMessageFromProto(testProto, valueFields)
    val parentBuilder = getParentMessageFromProto(testProto.toBuilder(), valueFields)

    assertTrue { parent is Message }
    assertTrue { parentBuilder is Builder }
    assertThat(parent as Message).isEqualTo(testProto.a.b)
    assertThat((parentBuilder as Builder).build()).isEqualTo(testProto.a.b)
  }

  @Test
  fun `repeated field is disallowed unless it is the last field and allowRepeated is true`() {
    /** Any repeated field in the path except the last field is disallowed. */
    assertFailsWith<UnsupportedOperationException> {
      getFieldFromProto(TestProto.getDescriptor(), "repeated_proto_a.b.int64_value.")
    }
    /** Any repeated field in the path except the last field is disallowed. */
    assertFailsWith<UnsupportedOperationException> {
      getFieldFromProto(
        TestProto.getDescriptor(),
        "repeated_proto_a.b.int64_value.",
        allowRepeated = true
      )
    }

    /** Last field is disallowed to be repeated if @allow_repeated is set to false. */
    assertFailsWith<IllegalStateException> {
      getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_values")
    }

    /** Last field is allowed to be repeated if @allow_repeated is set to true. */
    getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_values", allowRepeated = true)
  }

  @Test
  fun `allow repeated and get parent message`() {
    val fields =
      getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_values", allowRepeated = true)

    val testProto = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 8
          int64Value = 8
          uint32Value = 8
          uint64Value = 8
          floatValue = 8.0F
          doubleValue = 8.0
          boolValue = true
          enumValue = TestProtoB.TestEnum.TEST_ENUM_1
          stringValue = "string8"
          int32Values.add(1)
          int32Values.add(2)
        }
      }
    }

    assertThat(getParentMessageFromProto(testProto, fields) as Message).isEqualTo(testProto.a.b)
  }

  @Test
  fun `get parent message for parent not set`() {
    val fields = getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_value")
    val testProto = testProto {}

    assertThat(getParentMessageFromProto(testProto, fields) as Message).isEqualTo(testProto.a.b)
  }

  @Test
  fun `get size of repeated proto`() {
    val fields =
      getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_values", allowRepeated = true)
    val testProto = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Values.add(1)
          int32Values.add(2)
        }
      }
    }

    assertEquals(2, getSizeOfRepeatedProto(testProto, fields))
  }

  @Test
  fun `get size of repeated proto empty field`() {
    val fields =
      getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_values", allowRepeated = true)
    val testProto = testProto {}

    assertEquals(0, getSizeOfRepeatedProto(testProto, fields))
  }

  @Test
  fun `get repeated filed and value`() {
    val testProto = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Values.add(1)
          int32Values.add(2)
          int64Values.add(1L)
          int64Values.add(2L)
          uint32Values.add(1)
          uint32Values.add(2)
          uint64Values.add(1L)
          uint64Values.add(2L)
          floatValues.add(1.0f)
          floatValues.add(2.0f)
          doubleValues.add(1.0)
          doubleValues.add(2.0)
          boolValues.add(true)
          boolValues.add(false)
          enumValues.add(TestProtoB.TestEnum.TEST_ENUM_1)
          enumValues.add(TestProtoB.TestEnum.TEST_ENUM_2)
          stringValues.add("string1")
          stringValues.add("string2")
        }
      }
      repeatedProtoA.add(testProtoA { b = testProtoB { int32Value = 1 } })
      repeatedProtoA.add(testProtoA { b = testProtoB { int32Value = 2 } })
    }

    // Test int32.
    val int32ValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.int32_values", allowRepeated = true)
    assertEquals(2, getValueFromRepeatedProto(testProto, int32ValueFields, 1))
    // Test int64.
    val int64ValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.int64_values", allowRepeated = true)
    assertEquals(2L, getValueFromRepeatedProto(testProto, int64ValueFields, 1))
    // Test uint32.
    val uint32ValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.uint32_values", allowRepeated = true)
    assertEquals(2, getValueFromRepeatedProto(testProto, uint32ValueFields, 1))
    // Test uint64.
    val uint64ValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.uint64_values", allowRepeated = true)
    assertEquals(2L, getValueFromRepeatedProto(testProto, uint64ValueFields, 1))
    // Test float.
    val floatValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.float_values", allowRepeated = true)
    assertEquals(2.0f, getValueFromRepeatedProto(testProto, floatValueFields, 1))
    // Test double.
    val doubleValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.double_values", allowRepeated = true)
    assertEquals(2.0, getValueFromRepeatedProto(testProto, doubleValueFields, 1))
    // Test bool.
    val booleanValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.bool_values", allowRepeated = true)
    assertEquals(false, getValueFromRepeatedProto(testProto, booleanValueFields, 1))
    // Test enum.
    val enumValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.enum_values", allowRepeated = true)
    assertEquals(
      TestProtoB.TestEnum.TEST_ENUM_2.valueDescriptor,
      getValueFromRepeatedProto<EnumValueDescriptor>(testProto, enumValueFields, 1)
    )
    // Test string.
    val stringValueFields =
      getFieldFromProto(testProto.descriptorForType, "a.b.string_values", allowRepeated = true)
    assertEquals("string2", getValueFromRepeatedProto(testProto, stringValueFields, 1))
    // Test message.
    val messageValueFields =
      getFieldFromProto(testProto.descriptorForType, "repeated_proto_a", allowRepeated = true)
    assertEquals(
      testProto.getRepeatedProtoA(1),
      getValueFromRepeatedProto(testProto, messageValueFields, 1)
    )
  }
}
