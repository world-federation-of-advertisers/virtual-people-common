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
    assertTrue(parseValue<Int>("1,2,1").containsAll(listOf(1, 2)))
    assertTrue(parseValue<Long>("1,2,1").containsAll(listOf(1, 2)))
    assertTrue(parseValue<Boolean>("true,true,false").containsAll(listOf(true, false)))
    assertTrue(parseValue<String>("a,b,a").containsAll(listOf("a", "b")))
    assertTrue(parseValue<String>("").containsAll(listOf("")))
    assertTrue(parseValue<String>(",a").containsAll(listOf("", "a")))
    assertTrue(parseValue<String>("\"a,b,a").containsAll(listOf("\"a", "a", "b")))
  }

  @Test
  fun `value parser invalid cases`() {
    assertFailsWith<IllegalArgumentException> { parseValue<Int>("1,a,1") }
    assertFailsWith<IllegalArgumentException> { parseValue<Long>("1,a,1") }
    assertFailsWith<IllegalArgumentException> { parseValue<Boolean>("true,a") }
  }

  @Test
  fun `enum value parser valid cases`() {
    val descriptor = TestProtoB.TestEnum.getDescriptor()
    assertTrue(parseEnumValues(descriptor, "1,2,1").containsAll(listOf(1, 2)))
    assertTrue(
      parseEnumValues(descriptor, "TEST_ENUM_1,TEST_ENUM_2,TEST_ENUM_1").containsAll(listOf(1, 2))
    )
    assertTrue(parseEnumValues(descriptor, "1,TEST_ENUM_2,TEST_ENUM_1,2").containsAll(listOf(1, 2)))
  }

  @Test
  fun `enum value parser invalid cases`() {
    val descriptor = TestProtoB.TestEnum.getDescriptor()
    assertFailsWith<IllegalStateException> { parseEnumValues(descriptor, "1,a,1") }
  }
}
