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

import kotlin.test.assertFailsWith
import kotlin.test.assertFalse
import kotlin.test.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.JUnit4
import org.wfanet.virtualpeople.common.FieldFilterProto.Op
import org.wfanet.virtualpeople.common.fieldFilterProto
import org.wfanet.virtualpeople.common.test.TestProto
import org.wfanet.virtualpeople.common.test.TestProtoB.TestEnum
import org.wfanet.virtualpeople.common.test.testProto
import org.wfanet.virtualpeople.common.test.testProtoA
import org.wfanet.virtualpeople.common.test.testProtoB

@RunWith(JUnit4::class)
class AnyInFilterTest {

  @Test
  fun `filter without name should fail`() {
    val fieldFilter = fieldFilterProto {
      op = Op.ANY_IN
      value = "1,2,1"
    }
    assertFailsWith<IllegalStateException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `filter with singular field should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = Op.ANY_IN
      value = "1,2,1"
    }
    assertFailsWith<IllegalStateException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `filter with repeated field in the path should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "repeated_proto_a.b.int32_values"
      op = Op.ANY_IN
      value = "1,2,1"
    }
    assertFailsWith<IllegalStateException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `filter without value should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_values"
      op = Op.ANY_IN
    }
    assertFailsWith<IllegalStateException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `int32 any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_values"
      op = Op.ANY_IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Values.add(1)
          int32Values.add(3)
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Values.add(2)
          int32Values.add(3)
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int32Values.add(3) } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `int32 invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_values"
      op = Op.ANY_IN
      value = "1,a,1"
    }
    assertFailsWith<IllegalArgumentException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `int64 any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_values"
      op = Op.ANY_IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          int64Values.add(1)
          int64Values.add(3)
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          int64Values.add(2)
          int64Values.add(3)
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int64Values.add(3) } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `int64 invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_values"
      op = Op.ANY_IN
      value = "1,a,1"
    }
    assertFailsWith<IllegalArgumentException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `uint32 any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_values"
      op = Op.ANY_IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          uint32Values.add(1)
          uint32Values.add(3)
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          uint32Values.add(2)
          uint32Values.add(3)
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { uint32Values.add(3) } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `uint32 invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_values"
      op = Op.ANY_IN
      value = "1,a,1"
    }
    assertFailsWith<IllegalArgumentException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `uint64 any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_values"
      op = Op.ANY_IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          uint64Values.add(1)
          uint64Values.add(3)
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          uint64Values.add(2)
          uint64Values.add(3)
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { uint64Values.add(3) } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `uint64 invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_values"
      op = Op.ANY_IN
      value = "1,a,1"
    }
    assertFailsWith<IllegalArgumentException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `boolean any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.bool_values"
      op = Op.ANY_IN
      value = "true,true"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          boolValues.add(true)
          boolValues.add(false)
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { boolValues.add(false) } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {}
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `boolean invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.bool_values"
      op = Op.ANY_IN
      value = "true,a"
    }
    assertFailsWith<IllegalArgumentException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `enum name any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_values"
      op = Op.ANY_IN
      value = "TEST_ENUM_1,TEST_ENUM_2,TEST_ENUM_1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          enumValues.add(TestEnum.TEST_ENUM_1)
          enumValues.add(TestEnum.TEST_ENUM_3)
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          enumValues.add(TestEnum.TEST_ENUM_2)
          enumValues.add(TestEnum.TEST_ENUM_3)
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {
      a = testProtoA { b = testProtoB { enumValues.add(TestEnum.TEST_ENUM_3) } }
    }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `enum number any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_values"
      op = Op.ANY_IN
      value = "1,2,1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          enumValues.add(TestEnum.TEST_ENUM_1)
          enumValues.add(TestEnum.TEST_ENUM_3)
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          enumValues.add(TestEnum.TEST_ENUM_2)
          enumValues.add(TestEnum.TEST_ENUM_3)
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {
      a = testProtoA { b = testProtoB { enumValues.add(TestEnum.TEST_ENUM_3) } }
    }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `enum name and number mixed any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_values"
      op = Op.ANY_IN
      value = "TEST_ENUM_1,2,1,TEST_ENUM_2"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          enumValues.add(TestEnum.TEST_ENUM_1)
          enumValues.add(TestEnum.TEST_ENUM_3)
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          enumValues.add(TestEnum.TEST_ENUM_2)
          enumValues.add(TestEnum.TEST_ENUM_3)
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {
      a = testProtoA { b = testProtoB { enumValues.add(TestEnum.TEST_ENUM_3) } }
    }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `enum invalid value`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_values"
      op = Op.ANY_IN
      value = "a,TEST_ENUM_2,1"
    }
    assertFailsWith<IllegalStateException> {
      FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    }
  }

  @Test
  fun `string any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.string_values"
      op = Op.ANY_IN
      value = "a,b,a"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          stringValues.add("a")
          stringValues.add("c")
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          stringValues.add("b")
          stringValues.add("c")
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { stringValues.add("c") } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `empty string any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.string_values"
      op = Op.ANY_IN
      value = ""
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          stringValues.add("")
          stringValues.add("a")
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { stringValues.add("a") } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }

  @Test
  fun `quota string any in`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.string_values"
      op = Op.ANY_IN
      value = "\""
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          stringValues.add("\"")
          stringValues.add("a")
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { stringValues.add("a") } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }
}
