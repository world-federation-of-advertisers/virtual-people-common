// Copyright 2022 The Cross-Media Measurement Authors
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

package org.wfanet.virtualpeople.common.fieldfilter

import java.lang.IllegalStateException
import kotlin.test.assertFailsWith
import kotlin.test.assertFalse
import kotlin.test.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.JUnit4
import org.wfanet.virtualpeople.common.FieldFilterProto.Op
import org.wfanet.virtualpeople.common.fieldFilterProto
import org.wfanet.virtualpeople.common.test.*

@RunWith(JUnit4::class)
class InFilterTest {

  @Test
  fun `filter without name should fail`() {
    val fieldFilter = fieldFilterProto {
      op = Op.IN
      value = "1,2,1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Name must be set"))
  }

  @Test
  fun `filter with repeated field should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_values"
      op = Op.IN
      value = "1,2,1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Repeated field is not allowed"))
  }

  @Test
  fun `filter with repeated field in the path should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "repeated_proto_a.b.int32_value"
      op = Op.IN
      value = "1,2,1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Repeated field is not allowed in the path"))
  }

  @Test
  fun `filter without value should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = Op.IN
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Value must be set"))
  }
  @Test
  fun `int32 in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = Op.IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { int32Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int32Value = 2 } } }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int32Value = 3 } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `int32 not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = Op.IN
      value = "0"
    }

    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
    assertFalse(filter.matches(testProto.toBuilder()))
  }

  @Test
  fun `int32 invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = Op.IN
      value = "1,a,1"
    }
    val exception =
      assertFailsWith<NumberFormatException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("For input string: \"a\""))
  }

  @Test
  fun `int64 in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_value"
      op = Op.IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { int64Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int64Value = 2 } } }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int64Value = 3 } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `int64 not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_value"
      op = Op.IN
      value = "0"
    }

    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
    assertFalse(filter.matches(testProto.toBuilder()))
  }

  @Test
  fun `int64 invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_value"
      op = Op.IN
      value = "1,a,1"
    }

    val exception =
      assertFailsWith<NumberFormatException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("For input string: \"a\""))
  }

  @Test
  fun `uint32 in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_value"
      op = Op.IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { uint32Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { uint32Value = 2 } } }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { uint32Value = 3 } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `uint32 not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_value"
      op = Op.IN
      value = "0"
    }

    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
    assertFalse(filter.matches(testProto.toBuilder()))
  }

  @Test
  fun `uint32 invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_value"
      op = Op.IN
      value = "1,a,1"
    }

    val exception =
      assertFailsWith<NumberFormatException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("For input string: \"a\""))
  }

  @Test
  fun `uint64 in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_value"
      op = Op.IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { uint64Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { uint64Value = 2 } } }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { uint64Value = 3 } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `uint64 not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_value"
      op = Op.IN
      value = "0"
    }

    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
    assertFalse(filter.matches(testProto.toBuilder()))
  }

  @Test
  fun `uint64 invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_value"
      op = Op.IN
      value = "1,a,1"
    }

    val exception =
      assertFailsWith<NumberFormatException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("For input string: \"a\""))
  }

  @Test
  fun `boolean in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.bool_value"
      op = Op.IN
      value = "true,true"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { boolValue = true } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { boolValue = false } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))
  }

  @Test
  fun `boolean not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.bool_value"
      op = Op.IN
      value = "false"
    }

    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
    assertFalse(filter.matches(testProto.toBuilder()))
  }

  @Test
  fun `boolean invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.bool_value"
      op = Op.IN
      value = "true,a"
    }
    val exception =
      assertFailsWith<IllegalArgumentException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("The string doesn't represent a boolean value"))
  }

  @Test
  fun `enum name in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_value"
      op = Op.IN
      value = "TEST_ENUM_1,TEST_ENUM_2,TEST_ENUM_1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_1 } }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_2 } }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_3 } }
    }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `enum number in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_value"
      op = Op.IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_1 } }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_2 } }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_3 } }
    }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `enum name and number mixed in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_value"
      op = Op.IN
      value = "TEST_ENUM_1,2,1,TEST_ENUM_2"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_1 } }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_2 } }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestProtoB.TestEnum.TEST_ENUM_3 } }
    }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `enum not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_value"
      op = Op.IN
      value = "INVALID"
    }

    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
    assertFalse(filter.matches(testProto.toBuilder()))
  }

  @Test
  fun `enum invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_value"
      op = Op.IN
      value = "1,a,1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Not a valid enum name or integer"))
  }

  @Test
  fun `string in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.string_value"
      op = Op.IN
      value = "a,b,a"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { stringValue = "a" } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { stringValue = "b" } } }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { stringValue = "c" } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `quota string in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.string_value"
      op = Op.IN
      value = "\""
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { stringValue = "\"" } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { stringValue = "a" } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {}
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `string not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.string_value"
      op = Op.IN
      value = ""
    }

    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
    assertFalse(filter.matches(testProto.toBuilder()))
  }
}
