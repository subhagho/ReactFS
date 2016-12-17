//
// Created by Subhabrata Ghosh on 12/12/16.
//

#include "core/includes/typed_block.h"

string com::wookler::reactfs::core::typed_block::create(uint64_t block_id, uint64_t parent_id, string filename,
                                                        __block_usage usage, uint64_t block_size, uint64_t start_index,
                                                        uint32_t est_record_size, bool overwrite) {
    CHECK_NOT_NULL(this->datetype);

    string uuid = __create_block(block_id, parent_id, filename, usage, __block_def::INDEXED,
                                 block_size, start_index, overwrite);
    uint32_t estimated_records = (block_size / est_record_size);

    index_ptr = new base_block_index();
    CHECK_ALLOC(index_ptr, TYPE_NAME(base_block_index));

    index_ptr->create_index(header->block_id, header->block_uid, this->filename, estimated_records,
                            header->start_index, overwrite);

    void *ptr = common_utils::increment_data_ptr(base_ptr, sizeof(__block_header));
    uint64_t *type_header_size = static_cast<uint64_t *>(ptr);

    ptr = common_utils::increment_data_ptr(ptr, sizeof(uint64_t));
    *type_header_size = this->datetype->write(ptr, 0);

    close();

    POSTCONDITION(!IS_EMPTY(uuid));
    return uuid;
}

void com::wookler::reactfs::core::typed_block::open(uint64_t block_id, string filename) {
    void *ptr = __open_block(block_id, filename);
    POSTCONDITION(NOT_NULL(ptr));

    index_ptr = new base_block_index();
    CHECK_ALLOC(index_ptr, TYPE_NAME(base_block_index));
    index_ptr->open_index(header->block_id, header->block_uid, this->filename);

    ptr = common_utils::increment_data_ptr(base_ptr, sizeof(__block_header));
    uint64_t *type_header_size = static_cast<uint64_t *>(ptr);

    this->read_ptr = common_utils::increment_data_ptr(ptr, (sizeof(uint64_t) + *type_header_size));
    void *bptr = get_data_ptr();
    CHECK_NOT_NULL(bptr);

    if (header->write_state == __write_state::WRITABLE) {
        write_ptr = bptr;
    } else {
        write_ptr = nullptr;
    }

    state.set_state(__state_enum::Available);
}