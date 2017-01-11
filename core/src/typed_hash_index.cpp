//
// Created by Subhabrata Ghosh on 21/12/16.
//

#include "core/includes/typed_hash_index.h"

string com::wookler::reactfs::core::typed_index_base::create_index(uint64_t block_id, string block_uuid,
                                                                   string filename, uint32_t estimated_records,
                                                                   uint64_t start_index, bool overwrite) {
    return common_consts::EMPTY_STRING;
}

void com::wookler::reactfs::core::typed_index_base::open_index(uint64_t block_id, string block_uuid, string filename) {

}

void com::wookler::reactfs::core::typed_index_base::close() {

}

void com::wookler::reactfs::core::typed_index_base::commit(string txid) {

}

void com::wookler::reactfs::core::typed_index_base::force_rollback() {

}

void com::wookler::reactfs::core::typed_index_base::rollback(string txid) {

}

const uint64_t com::wookler::reactfs::core::typed_index_base::get_free_space() const {
    return 0;
}

const uint64_t com::wookler::reactfs::core::typed_index_base::get_used_space() const {
    return 0;
}

const __record_index_ptr *com::wookler::reactfs::core::typed_index_base::read_index(__index_key_set *index, bool all) {
    return nullptr;
}


const __record_index_ptr *com::wookler::reactfs::core::typed_index_base::write_index(__index_key_set *index,
                                                                                     uint64_t offset, uint64_t size,
                                                                                     string transaction_id) {
    return nullptr;
}

bool com::wookler::reactfs::core::typed_index_base::delete_index(__index_key_set *index, string transaction_id) {
    return false;
}

void com::wookler::reactfs::core::typed_index_base::sync(bool recreate) {

}
