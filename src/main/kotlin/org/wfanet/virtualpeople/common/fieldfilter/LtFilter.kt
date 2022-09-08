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

import com.google.protobuf.Descriptors
import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.fieldfilter.utils.IntegerComparator
import org.wfanet.virtualpeople.common.fieldfilter.utils.IntegerCompareResult
import org.wfanet.virtualpeople.common.fieldfilter.utils.getFieldFromProto

/**
 * The implementation of [FieldFilter] when op is LT in config.
 *
 * Always use [FieldFilter.create]. Users should never construct a [LtFilter] directly.
 */
internal class LtFilter(descriptor: Descriptors.Descriptor, config: FieldFilterProto) :
  FieldFilter {

  private val comparator: IntegerComparator

  /**
   * Returns error status if any of the following happens:
   * 1. [config.op] is not LT.
   * 2. [config.name] is not set.
   * 3. [config.name] refers to a non-integer field.
   * 4. Any field of the path represented by [config.name] is repeated field.
   * 5. [config.value] is not set.
   *
   * [config.value] will be cast to the type of the field represented by [config.name].
   */
  init {
    if (config.op != FieldFilterProto.Op.LT) {
      error("Op must be LT. Input FieldFilterProto: $config")
    }
    if (!config.hasName()) {
      error("Name must be set. Input FieldFilterProto: $config")
    }
    if (!config.hasValue()) {
      error("Value must be set. Input FieldFilterProto: $config")
    }
    val fieldDescriptors = getFieldFromProto(descriptor, config.name)
    comparator = IntegerComparator.new(fieldDescriptors, config.value)
  }

  /**
   * Returns true when the field represented by [config.name] in [messageOrBuilder] is less than
   * [config.value]. Otherwise, returns false. Returns false if the field is not set.
   */
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    return comparator.compare(messageOrBuilder) == IntegerCompareResult.LESS_THAN
  }
}
