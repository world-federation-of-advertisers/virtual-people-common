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

@RunWith(JUnit4::class)
class ValuesParserTest {

  @Test
  fun `value parser valid cases`() {
    assertEquals(parseValue<Int>("1,2,1"), setOf(1, 2))
    assertEquals(parseValue<Long>("1,2,1"), setOf(1L, 2L))
    assertEquals(parseValue<Boolean>("true,true,false"), setOf(true, false))
    assertEquals(parseValue<String>("a,b,a"), setOf("a", "b"))
    assertEquals(parseValue<String>(""), setOf(""))
    assertEquals(parseValue<String>(",a"), setOf("", "a"))
    assertEquals(parseValue<String>("\"a,b,a"), setOf("\"a", "a", "b"))
  }

  @Test
  fun `value parser invalid cases`() {
    assertFailsWith<NumberFormatException> { parseValue<Int>("1,a,1") }
    assertFailsWith<NumberFormatException> { parseValue<Long>("1,a,1") }
    val exception = assertFailsWith<IllegalArgumentException> { parseValue<Boolean>("true,a") }
    assertTrue(exception.message!!.contains("The string doesn't represent a boolean value"))
  }

  @Test
  fun `enum value parser valid cases`() {
    val descriptor = TestProtoB.TestEnum.getDescriptor()
    assertEquals(parseEnumValues(descriptor, "1,2,1"), setOf(1, 2))
    assertEquals(parseEnumValues(descriptor, "TEST_ENUM_1,TEST_ENUM_2,TEST_ENUM_1"), setOf(1, 2))
    assertEquals(parseEnumValues(descriptor, "1,TEST_ENUM_2,TEST_ENUM_1,2"), setOf(1, 2))
  }

  @Test
  fun `enum value parser invalid cases`() {
    val descriptor = TestProtoB.TestEnum.getDescriptor()
    val exception = assertFailsWith<IllegalStateException> { parseEnumValues(descriptor, "1,a,1") }
    assertTrue(exception.message!!.contains("Not a valid enum name or integer"))
  }
}
