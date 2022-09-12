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
import org.wfanet.virtualpeople.common.copy

/**
 * The implementation of [FieldFilter] when op is NOT in config.
 *
 * Always use [FieldFilter.create]. Users should never construct a [NotFilter] directly.
 */
internal class NotFilter(descriptor: Descriptors.Descriptor, config: FieldFilterProto) :
  FieldFilter {
  /**
   * A field filter represents the AND of all the subFilters. The output of this NotFilter should be
   * the reverse of the output of AndFilter.
   */
  private val andFilter: FieldFilter

  /**
   * Returns error status if any of the following happens:
   * 1. [config.op] is not NOT.
   * 2. [config.subfilters] is empty.
   * 3. Any of [config.subFilters] is invalid to create a FieldFilter.
   */
  init {
    if (config.op != FieldFilterProto.Op.NOT) {
      error("Op must be NOT. Input FieldFilterProto: $config")
    }
    if (config.subFiltersCount == 0) {
      error("subFilters must be set when op is NOT. Input FieldFilterProto: $config")
    }

    val andFilterConfig = config.copy { op = FieldFilterProto.Op.AND }
    andFilter = FieldFilter.create(descriptor, andFilterConfig)
  }

  /** Returns false when all the [config.subFilters pass. Otherwise, returns true. */
  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    return !andFilter.matches(messageOrBuilder)
  }
}
