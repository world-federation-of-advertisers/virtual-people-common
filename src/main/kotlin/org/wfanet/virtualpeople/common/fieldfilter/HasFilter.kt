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
import com.google.protobuf.Descriptors.FieldDescriptor
import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.fieldfilter.utils.getFieldFromProto
import org.wfanet.virtualpeople.common.fieldfilter.utils.getParentMessageFromProto

/**
 * The implementation of [FieldFilter] when op is HAS in config.
 *
 * Always use [FieldFilter.create] Users should never construct a [HasFilter] directly.
 */
internal class HasFilter(descriptor: Descriptors.Descriptor, config: FieldFilterProto) :
  FieldFilter {

  private val fieldDescriptors: List<FieldDescriptor>

  init {
    if (config.op != FieldFilterProto.Op.HAS) {
      error("Op must be HAS. Input FieldFilterProto: $config")
    }
    if (!config.hasName()) {
      error("Name must be set. Input FieldFilterProto: $config")
    }
    fieldDescriptors = getFieldFromProto(descriptor, config.name, allowRepeated = true)
  }

  /**
   * Returns true if any of the following is true. 1. The field is not repeated, and is set in
   * [messageOrBuilder]. 2. The field is repeated, and is not empty in [messageOrBuilder].
   */
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    val parent = getParentMessageFromProto(messageOrBuilder, fieldDescriptors)
    val lastDescriptor = fieldDescriptors.last()
    return if (lastDescriptor.isRepeated) {
      parent.getRepeatedFieldCount(lastDescriptor) > 0
    } else {
      parent.hasField(lastDescriptor)
    }
  }
}
