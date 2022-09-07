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

import com.google.protobuf.Descriptors.Descriptor
import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.FieldFilterProto.Op
import org.wfanet.virtualpeople.common.fieldfilter.utils.convertMessageToFilter

/**
 * Kotlin implementation of FieldFilterProto.
 *
 * @param descriptor target protobuf message type this FieldFilter checks.
 * @param config checks that will be performed when calling IsMatch.
 *
 * This is the interface for all FieldFilter classes. Never add any behavior here.
 */
sealed interface FieldFilter {
  /** Returns true if the [messageOrBuilder] satisfies the condition described by the FieldFilter */
  fun matches(messageOrBuilder: MessageOrBuilder): Boolean

  companion object {
    /**
     * Always use the constructor of [FieldFilter] to get a [FieldFilter] object. Users should never
     * call the factory functions or the constructors of the derived classes.
     */
    fun create(descriptor: Descriptor, config: FieldFilterProto): FieldFilter {
      @Suppress("WHEN_ENUM_CAN_BE_NULL_IN_JAVA") // Proto enum fields are never null.
      return when (config.op) {
        Op.HAS -> HasFilter(descriptor, config)
        Op.EQUAL -> EqualFilter(descriptor, config)
        Op.AND -> AndFilter(descriptor, config)
        Op.OR -> OrFilter(descriptor, config)
        Op.ANY_IN -> AnyInFilter.create(descriptor, config)
        Op.PARTIAL -> PartialFilter(descriptor, config)
        Op.GT,
        Op.LT,
        Op.IN,
        Op.REGEXP,
        Op.NOT,
        Op.TRUE -> TODO("Not yet implemented")
        Op.UNRECOGNIZED,
        Op.INVALID -> error("Invalid OP")
      }
    }

    /**
     * Creates a [FieldFilter], which checks the equality of all the fields set in the input
     * [messageOrBuilder], including nested fields.
     *
     * Float and double fields are not supported. Any repeated field is not supported.
     */
    fun create(messageOrBuilder: MessageOrBuilder): FieldFilter {
      val config: FieldFilterProto = convertMessageToFilter(messageOrBuilder)
      return create(messageOrBuilder.descriptorForType, config)
    }
  }
}
