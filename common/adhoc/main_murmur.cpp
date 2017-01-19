//
// Created by Subhabrata Ghosh on 27/10/16.
//

#include <unordered_map>
#include <fstream>

#include "common/includes/timer.h"
#include "common/includes/init_utils.h"
#include "common/includes/ext/murmur3.h"

#define KEY_COUNT 200000
#define DATA_FILENAME "/tmp/collision.dat"

using namespace com::wookler::reactfs::common;

typedef struct __key {
    char key[UCHAR_MAX];
    uint64_t id;
} key;

uint32_t find_bucket(uint32_t hash, uint32_t size) {
    uint8_t b[4];
    memset(b, 0, 4 * sizeof(uint8_t));
    memcpy(b, &hash, sizeof(uint32_t));

    uint16_t h = 0;
    memcpy(&h, b, sizeof(uint16_t));

    uint8_t ii = 32;
    while (h >= size) {
        h = bitset_utils::clear_uint32_bit(h, ii);
        ii--;
    }
    return h;
}

uint32_t find_index(uint32_t hash, uint32_t size) {
    uint8_t b[4];
    memset(b, 0, 4 * sizeof(uint8_t));
    memcpy(b, &hash, sizeof(uint32_t));

    uint16_t h = 0;
    memcpy(&h, b + 2, sizeof(uint16_t));

    uint8_t ii = 32;
    while (h >= size) {
        h = bitset_utils::clear_uint32_bit(h, ii);
        ii--;
    }
    return h;
}

int main(int argc, char **argv) {
    try {

        PRECONDITION(argc > 1);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);
        env_utils::create_env(configf);

        const __env *env = env_utils::get_env();
        CHECK_NOT_NULL(env);

        unordered_map<string, short> keys;

        uint32_t b_prime = common_utils::find_prime(100);
        uint32_t b_size = KEY_COUNT / b_prime / 2;
        b_size = common_utils::find_prime(b_size);

        LOG_DEBUG("Using bucket prime = %d bucket size = %d", b_prime, b_size);

        uint32_t seed = time_utils::now();
        seed = common_utils::find_prime(seed);

        string uuid;
        vector<vector<uint16_t>> rows(b_size);
        for (uint32_t ii = 0; ii < b_size; ii++) {
            rows[ii] = vector<uint16_t>(b_prime);
        }
        for (uint32_t ii = 0; ii < KEY_COUNT; ii++) {
            key k;
            memset(&k, 0, sizeof(key));
            if (ii % 32 == 0)
                uuid = common_utils::uuid();
            strncpy(k.key, uuid.c_str(), uuid.length());
            k.id = ii % 32;

            uint32_t t_hash = 0;
            MurmurHash3_x86_32(&k, sizeof(key), seed, &t_hash);

            uint32_t o1 = t_hash % b_size;
            uint32_t o2 = t_hash % b_prime;

            string kk = common_utils::format("%d:%d", o1, o2);
            unordered_map<string, short>::iterator iter = keys.find(kk);
            uint16_t count = 0;
            if (iter != keys.end()) {
                iter->second += 1;
                count = iter->second;
            } else {
                keys.insert({kk, 1});
                count = 1;
            }
            rows[o1][o2] = count;
        }

        LOG_DEBUG("Total key count = %d", keys.size());
        int c_count = 0;
        int nc_count = 0;
        int max_c = 0;
        for (auto iter = keys.begin(); iter != keys.end(); iter++) {
            if (iter->second > 1) {
                if (iter->second >= 4)
                    LOG_INFO("Key = %s Count = %d", iter->first.c_str(), iter->second);
                c_count += iter->second;
                if (iter->second > max_c) {
                    max_c = iter->second;
                }
            } else {
                nc_count++;
            }
        }
        LOG_DEBUG("Collision count = %d, No-collisions = %d, Max collisions = %d", c_count, nc_count, max_c);

        ofstream stream(DATA_FILENAME);
        for (uint32_t ii = 0; ii < rows.size(); ii++) {
            vector<uint16_t> v = rows[ii];
            for (uint32_t jj = 0; jj < v.size(); jj++) {
                if (jj != 0) {
                    stream << " ";
                }
                stream << v[jj];
            }
            stream << "\n";
        }
        stream.flush();
        stream.close();

        env_utils::dispose();

        exit(0);
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        exit(-1);
    } catch (...) {
        LOG_ERROR("Unhandled exception occurred.");
        exit(-2);
    }
}