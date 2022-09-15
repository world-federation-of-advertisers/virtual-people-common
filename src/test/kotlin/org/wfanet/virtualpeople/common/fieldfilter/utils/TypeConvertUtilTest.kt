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

package org.wfanet.virtualpeople.common.fieldfilter.utils

import java.lang.NumberFormatException
import kotlin.test.assertEquals
import kotlin.test.assertFailsWith
import kotlin.test.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.JUnit4
import org.wfanet.virtualpeople.common.test.TestProtoB
import org.wfanet.virtualpeople.common.test.TestProtoB.TestEnum

@RunWith(JUnit4::class)
class TypeConvertUtilTest {

  @Test
  fun `type convert util valid cases`() {
    assertEquals(1, convertToNumeric<Int>("1"))
    assertEquals(1U, convertToNumeric("1"))
    assertEquals(-1, convertToNumeric("-1"))
    assertEquals(1L, convertToNumeric("1"))
    assertEquals(1UL, convertToNumeric("1"))
    assertEquals(-1L, convertToNumeric("-1"))
    assertEquals(1.1f, convertToNumeric("1.1"))
    assertEquals(-1.1f, convertToNumeric("-1.1"))
    assertEquals(1.1, convertToNumeric("1.1"))
    assertEquals(-1.1, convertToNumeric("-1.1"))
    assertEquals(true, convertToNumeric("True"))
    assertEquals(true, convertToNumeric("true"))
    assertEquals(false, convertToNumeric("False"))
    assertEquals(false, convertToNumeric("false"))
  }

  @Test
  fun `type convert util invalid cases`() {
    assertFailsWith<NumberFormatException> { convertToNumeric<Int>("a") }
    assertFailsWith<NumberFormatException> { convertToNumeric<Int>("") }
    assertFailsWith<NumberFormatException> { convertToNumeric<Long>("a") }
    assertFailsWith<NumberFormatException> { convertToNumeric<Long>("") }
    assertFailsWith<NumberFormatException> { convertToNumeric<Float>("a") }
    assertFailsWith<NumberFormatException> { convertToNumeric<Float>("") }
    assertFailsWith<NumberFormatException> { convertToNumeric<Double>("a") }
    assertFailsWith<NumberFormatException> { convertToNumeric<Double>("") }
    val exception1 = assertFailsWith<IllegalArgumentException> { convertToNumeric<Boolean>("foo") }
    assertTrue(exception1.message!!.contains("The string doesn't represent a boolean value"))
    val exception2 = assertFailsWith<IllegalArgumentException> { convertToNumeric<Boolean>("1") }
    assertTrue(exception2.message!!.contains("The string doesn't represent a boolean value"))

    /** Unsupported output type */
    val exception3 = assertFailsWith<IllegalStateException> { convertToNumeric<Char>("1") }
    assertTrue(exception3.message!!.contains("Unsupported data type"))
  }

  @Test
  fun `convert to enum valid cases`() {
    val fieldDescriptor = TestProtoB.getDescriptor().findFieldByName("enum_value")
    assertEquals(
      TestEnum.TEST_ENUM_1.valueDescriptor,
      convertToEnum(fieldDescriptor.enumType, "TEST_ENUM_1")
    )
    assertEquals(TestEnum.TEST_ENUM_1.valueDescriptor, convertToEnum(fieldDescriptor.enumType, "1"))
  }

  @Test
  fun `convert to enum invalid cases`() {
    val fieldDescriptor = TestProtoB.getDescriptor().findFieldByName("enum_value")
    val exception1 =
      assertFailsWith<IllegalStateException> {
        convertToEnum(fieldDescriptor.enumType, "TEST_ENUM_7")
      }
    assertTrue(exception1.message!!.contains("Not a valid enum name or integer"))
    val exception2 =
      assertFailsWith<IllegalStateException> { convertToEnum(fieldDescriptor.enumType, "7") }
    assertTrue(exception2.message!!.contains("Input cannot be converted to enum"))
  }
}
