//
// Created by Subhabrata Ghosh on 27/10/16.
//

#include "common/includes/log_utils.h"
#include "common/includes/__env.h"
#include "common/includes/init_utils.h"
#include "core/includes/base_block.h"
#include "core/includes/base_indexed_block.h"
#include "core/includes/block_utils.h"
#include "core/includes/node_env.h"
#include "common/includes/shared_lock_utils.h"

#define REUSE_PARENT_ID ULONG_MAX
#define REUSE_BLOCK_FILE "block_reused.raw"
#define REUSE_BLOCK_ID ULONG_MAX

#define REUSE_BLOCK_INDEX_FILE "block_reused.indexed"
#define REUSE_BLOCK_INDEX_ID (ULONG_MAX - 1)

#define REUSE_BLOCK_FILE_COMP "/tmp/block_reused.comp"
#define REUSE_BLOCK_ID_COMP 1025

#define DEFAULT_BLOCK_SIZE 1024 * 1024 * 32
#define RECORD_START_INDEX 100000
#define COUNT_RECORDS 500

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

typedef struct {
    uint64_t timestamp;
    char value[128];
    uint32_t index;
} _test_typed;

void test_basic() {
    node_client_env *c_env = node_init_client::get_client_env();
    string s = block_utils::get_block_dir(c_env->get_mount_client(), DEFAULT_BLOCK_SIZE);
    Path p(s);
    p.append(REUSE_BLOCK_FILE);

    LOG_DEBUG("Using block file [%s]", p.get_path().c_str());

    string uuid = block_utils::create_new_block(REUSE_BLOCK_ID, REUSE_PARENT_ID, p.get_path(), __block_def::BASIC,
                                                __block_usage::PRIMARY,
                                                DEFAULT_BLOCK_SIZE,
                                                sizeof(_test_typed), RECORD_START_INDEX, true);
    POSTCONDITION(!IS_EMPTY(uuid));

    base_block *block = new base_block();
    block->open(REUSE_BLOCK_ID, p.get_path());
    POSTCONDITION(block->get_block_state() == __state_enum::Available);

    string txid = block->start_transaction();
    POSTCONDITION(!IS_EMPTY(txid));
    int count = 0;
    vector<uint64_t> to_deleted;

    for (int ii = 0; ii < COUNT_RECORDS; ii++) {
        _test_typed t;
        t.index = ii;
        t.timestamp = time_utils::now() + (ii * 10000);
        string uuid = common_utils::uuid();
        strncpy(t.value, uuid.c_str(), uuid.length());

        if (block->get_free_space() < sizeof(_test_typed))
            break;
        uint64_t index = block->write(&t, sizeof(_test_typed), txid);
        POSTCONDITION(index >= 0);
        if (ii % 5 == 0) {
            to_deleted.push_back(index);
        }
        count++;
    }
    block->commit(txid);
    LOG_INFO("Written [%d] records to block. [used bytes=%lu]", count, block->get_used_space());


    CHECK_AND_FREE(block);
    block = new base_block();
    block->open(REUSE_BLOCK_ID, p.get_path());
    POSTCONDITION(block->get_block_state() == __state_enum::Available);

    txid = block->start_transaction();
    POSTCONDITION(!IS_EMPTY(txid));
    int d_count = 0;
    for (uint64_t index : to_deleted) {
        if (block->delete_record(index, txid)) {
            d_count++;
        }
    }


    block->commit(txid);
    LOG_INFO("Deleted [%d] records...", d_count);

    int fetched = 0;
    vector<shared_read_ptr> r;
    uint32_t offset = 0;
    while (offset <= count) {
        uint32_t r_count = block->read((RECORD_START_INDEX + offset), 5, &r);
        if (r_count > 0) {
            for (int ii = 0; ii < r.size(); ii++) {
                const void *ptr = r[ii].get()->get_data_ptr();
                CHECK_NOT_NULL(ptr);
                POSTCONDITION(r[ii].get()->get_size() == sizeof(_test_typed));
                const _test_typed *t = static_cast<const _test_typed *>(ptr);
                LOG_DEBUG("[index=%d][uuid=%s]", t->index, t->value);
            }
            fetched += r.size();
            r.clear();

            offset += 5;
        } else
            break;
    }

    LOG_INFO("Fetched [%d] records...", fetched);

    fetched = 0;
    offset = 0;
    while (true) {
        uint32_t r_count = block->read((RECORD_START_INDEX + offset), 20, &r, __record_state::R_DELETED);
        if (r_count > 0) {
            for (int ii = 0; ii < r.size(); ii++) {
                const void *ptr = r[ii].get()->get_data_ptr();
                CHECK_NOT_NULL(ptr);
                POSTCONDITION(r[ii].get()->get_size() == sizeof(_test_typed));
                const _test_typed *t = static_cast<const _test_typed *>(ptr);
                LOG_DEBUG("[index=%d][uuid=%s]", t->index, t->value);
            }
            fetched += r.size();
            r.clear();
            offset += 20;
        } else
            break;
    }

    block->check_block_sanity();
    block->dump_block_state();

    CHECK_AND_FREE(block);
}

void test_indexed() {
    node_client_env *c_env = node_init_client::get_client_env();
    string s = block_utils::get_block_dir(c_env->get_mount_client(), DEFAULT_BLOCK_SIZE);
    Path p(s);
    p.append(REUSE_BLOCK_INDEX_FILE);

    LOG_DEBUG("Using block file [%s]", p.get_path().c_str());

    string uuid = block_utils::create_new_block(REUSE_BLOCK_INDEX_ID, REUSE_PARENT_ID, p.get_path(),
                                                __block_def::INDEXED,
                                                __block_usage::PRIMARY,
                                                DEFAULT_BLOCK_SIZE,
                                                sizeof(_test_typed), RECORD_START_INDEX, true);
    POSTCONDITION(!IS_EMPTY(uuid));

    base_block *block = new base_indexed_block();
    block->open(REUSE_BLOCK_INDEX_ID, p.get_path());
    POSTCONDITION(block->get_block_state() == __state_enum::Available);

    string txid = block->start_transaction();
    POSTCONDITION(!IS_EMPTY(txid));
    int count = 0;
    vector<uint64_t> to_deleted;

    for (int ii = 0; ii < COUNT_RECORDS; ii++) {
        _test_typed t;
        t.index = ii;
        t.timestamp = time_utils::now() + (ii * 10000);
        string uuid = common_utils::uuid();
        strncpy(t.value, uuid.c_str(), uuid.length());

        if (block->get_free_space() < sizeof(_test_typed))
            break;
        uint64_t index = block->write(&t, sizeof(_test_typed), txid);
        POSTCONDITION(index >= 0);
        if (ii % 5 == 0) {
            to_deleted.push_back(index);
        }
        count++;
    }
    block->commit(txid);
    LOG_INFO("Written [%d] records to block. [used bytes=%lu]", count, block->get_used_space());


    CHECK_AND_FREE(block);
    block = new base_indexed_block();
    block->open(REUSE_BLOCK_INDEX_ID, p.get_path());
    POSTCONDITION(block->get_block_state() == __state_enum::Available);

    txid = block->start_transaction();
    POSTCONDITION(!IS_EMPTY(txid));
    int d_count = 0;
    for (uint64_t index : to_deleted) {
        if (block->delete_record(index, txid)) {
            d_count++;
        }
    }


    block->commit(txid);
    LOG_INFO("Deleted [%d] records...", d_count);

    int fetched = 0;
    vector<shared_read_ptr> r;
    uint32_t offset = 0;
    while (offset <= count) {
        uint32_t r_count = block->read((RECORD_START_INDEX + offset), 5, &r);
        if (r_count > 0) {
            for (int ii = 0; ii < r.size(); ii++) {
                const void *ptr = r[ii].get()->get_data_ptr();
                CHECK_NOT_NULL(ptr);
                POSTCONDITION(r[ii].get()->get_size() == sizeof(_test_typed));
                const _test_typed *t = static_cast<const _test_typed *>(ptr);
                LOG_DEBUG("[index=%d][uuid=%s]", t->index, t->value);
            }
            fetched += r.size();
            r.clear();

            offset += 5;
        } else
            break;
    }

    LOG_INFO("Fetched [%d] records...", fetched);

    fetched = 0;
    offset = 0;
    while (true) {
        uint32_t r_count = block->read((RECORD_START_INDEX + offset), 20, &r, __record_state::R_DELETED);
        if (r_count > 0) {
            for (int ii = 0; ii < r.size(); ii++) {
                const void *ptr = r[ii].get()->get_data_ptr();
                CHECK_NOT_NULL(ptr);
                POSTCONDITION(r[ii].get()->get_size() == sizeof(_test_typed));
                const _test_typed *t = static_cast<const _test_typed *>(ptr);
                LOG_DEBUG("[index=%d][uuid=%s]", t->index, t->value);
            }
            fetched += r.size();
            r.clear();
            offset += 20;
        } else
            break;
    }

    block->check_block_sanity();
    block->dump_block_state();

    CHECK_AND_FREE(block);
}

void test_compressed() {

}

int main(int argc, char **argv) {
    try {

        PRECONDITION(argc > 0);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);

        node_init_client::create_node_env(configf);
        node_client_env *c_env = node_init_client::get_client_env();
        CHECK_NOT_NULL(c_env);

        test_basic();
        test_indexed();
        test_compressed();

        node_init_client::shutdown();

        exit(0);
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        exit(-1);
    } catch (...) {
        LOG_ERROR("Unhandled exception occurred.");
        exit(-2);
    }
}