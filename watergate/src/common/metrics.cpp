//
// Created by Subhabrata Ghosh on 04/10/16.
//

#include "includes/common/metrics.h"

mutex com::watergate::common::metrics_utils::g_lock;

__state__ com::watergate::common::metrics_utils::state;

unordered_map<string, __metric *> *com::watergate::common::metrics_utils::metrics = new unordered_map<string, __metric *>();