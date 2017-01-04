#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

#include "common/includes/init_utils.h"
#include "types/includes/type_defs.h"

using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;

#define DATASET_SIZE 1024 * 1024 * 1024

class test_types {
    bool malloced = false;
    char *c = nullptr;
    int *i = nullptr;
    CHARBUFF s = nullptr;
    vector<CHARBUFF> *vs = nullptr;
    unordered_map<string, double *> *map = nullptr;

public:
    test_types(bool malloced) {
        this->malloced = malloced;
    }

    ~test_types() {
        if (malloced) {
            FREE_PTR(c);
            FREE_PTR(i);
        }
        if (NOT_NULL(vs)) {
            for (CHARBUFF ss : *vs) {
                FREE_PTR(ss);
            }
            CHECK_AND_FREE(vs);
        }
        if (malloced) {
            unordered_map<string, double *>::iterator iter;
            for (iter = map->begin(); iter != map->end(); iter++) {
                FREE_PTR(iter->second);
            }
        }
        CHECK_AND_FREE(map);
    }

    void set_c(char *c) {
        this->c = c;
    }

    void set_c(char c) {
        this->c = (char *) malloc(sizeof(char));
        CHECK_ALLOC(this->c, TYPE_NAME(char));
        *this->c = c;
    }

    char *get_c() {
        return this->c;
    }

    void set_i(int *i) {
        this->i = i;
    }

    void set_i(int i) {
        this->i = (int *) malloc(sizeof(int));
        CHECK_ALLOC(this->i, TYPE_NAME(int));
        *this->i = i;
    }

    int *get_i() {
        return this->i;
    }

    void set_s(CHARBUFF s) {
        this->s = s;
    }

    void set_s(string s) {
        int size = s.length() * sizeof(char);
        this->s = (CHARBUFF) malloc(size + 1);
        CHECK_ALLOC(this->s, TYPE_NAME(CHARBUFF));
        memset(this->s, 0, size + 1);
        strncpy(this->s, s.c_str(), s.length());
    }

    CHARBUFF get_s() {
        return this->s;
    }

    void set_vs(vector<CHARBUFF> *vs) {
        this->vs = vs;
    }

    void add_vs(string s) {
        if (IS_NULL(vs)) {
            vs = new vector<CHARBUFF>();
            CHECK_ALLOC(vs, TYPE_NAME(vector));
        }
        int size = s.length() * sizeof(char);
        CHARBUFF ns = (CHARBUFF) malloc(size + 1);
        CHECK_ALLOC(ns, TYPE_NAME(CHARBUFF));
        memset(ns, 0, size + 1);
        strncpy(ns, s.c_str(), s.length());
        vs->push_back(ns);
    }

    vector<CHARBUFF> *get_vs() {
        return this->vs;
    }

    void set_map(unordered_map<string, double *> *map) {
        this->map = map;
    }

    void add_map(string key, double d) {
        if (IS_NULL(map)) {
            map = new unordered_map<string, double *>();
            CHECK_ALLOC(map, TYPE_NAME(unoredered_map));
        }
        double *dd = (double *) malloc(sizeof(double));
        CHECK_ALLOC(dd, TYPE_NAME(double));
        *dd = d;

        map->insert({key, dd});
    }

    unordered_map<string, double *> *get_map() {
        return this->map;
    };
};

test_types *create_new(int index) {
    test_types *tt = new test_types(true);
    CHECK_ALLOC(tt, TYPE_NAME(test_types));
    char c = (char) index;
    tt->set_c(c);
    tt->set_i(index);
    string uuid = common_utils::uuid();
    tt->set_s(uuid);


    for (int ii = 0; ii < index + 1; ii++) {
        double d = ((double) time_utils::now()) / (ii + 1);
        string k = common_utils::format("%s_%d_%d", uuid.c_str(), index, ii);
        tt->add_map(k, d);

        tt->add_vs(k);
    }
    return tt;
}

int
main(const int argc, const char **argv) {
    int r_size = 10;
    /** check for the right # of arguments **/
    if (argc == 2) {
        try {
            const char *cf = argv[1];
            CHECK_NOT_NULL(cf);

            string configf(cf);
            env_utils::create_env(configf);

            void *dataptr = malloc(sizeof(BYTE) * DATASET_SIZE);
            CHECK_ALLOC(dataptr, TYPE_NAME(BYTE));
            memset(dataptr, 0, sizeof(BYTE) * DATASET_SIZE);

            __native_type *nt_c = new __native_type(nullptr, 0, "c", __type_def_enum::TYPE_CHAR);
            CHECK_ALLOC(nt_c, TYPE_NAME(__native_type));

            __native_type *nt_i = new __native_type(nullptr, 0, "i", __type_def_enum::TYPE_INTEGER);
            CHECK_ALLOC(nt_i, TYPE_NAME(__native_type));

            __native_type *nt_s = new __native_type(nullptr, 0, "s", __type_def_enum::TYPE_STRING);
            CHECK_ALLOC(nt_s, TYPE_NAME(__native_type));

            __list_type *lt = new __list_type(nullptr, 0, "vs", __type_def_enum::TYPE_STRING, nullptr);
            CHECK_ALLOC(lt, TYPE_NAME(__list_type));

            __map_type *mt = new __map_type(nullptr, 0, "map", __type_def_enum::TYPE_STRING,
                                            __type_def_enum::TYPE_DOUBLE,
                                            nullptr);
            CHECK_ALLOC(mt, TYPE_NAME(__map_type));

            vector<test_types *> *values = new vector<test_types *>();
            CHECK_ALLOC(values, TYPE_NAME(vector));
            for (int ii = 0; ii < r_size; ii++) {
                test_types *tt = create_new(ii);
                values->push_back(tt);
            }
            uint32_t offset = 0;
            for (int ii = 0; ii < r_size; ii++) {
                test_types *tt = (*values)[ii];
                offset += nt_c->get_type_handler(__record_mode::RM_WRITE)->write(dataptr, tt->get_c(), offset, DATASET_SIZE);
                offset += nt_i->get_type_handler(__record_mode::RM_WRITE)->write(dataptr, tt->get_i(), offset, DATASET_SIZE);
                offset += nt_s->get_type_handler(__record_mode::RM_WRITE)->write(dataptr, tt->get_s(), offset, DATASET_SIZE);
                offset += lt->get_type_handler(__record_mode::RM_WRITE)->write(dataptr, tt->get_vs(), offset, DATASET_SIZE);
                offset += mt->get_type_handler(__record_mode::RM_WRITE)->write(dataptr, tt->get_map(), offset, DATASET_SIZE);
            }

            offset = 0;
            for (int ii = 0; ii < r_size; ii++) {
                test_types *tt = new test_types(false);
                char *c = nullptr;
                offset += nt_c->get_type_handler(__record_mode::RM_READ)->read(dataptr, &c, offset, DATASET_SIZE);
                CHECK_NOT_NULL(c);
                tt->set_c(c);
                int *i = nullptr;
                offset += nt_i->get_type_handler(__record_mode::RM_READ)->read(dataptr, &i, offset, DATASET_SIZE);
                CHECK_NOT_NULL(i);
                tt->set_i(i);
                CHARBUFF s = nullptr;
                offset += nt_s->get_type_handler(__record_mode::RM_READ)->read(dataptr, &s, offset, DATASET_SIZE);
                CHECK_NOT_NULL(s);
                tt->set_s(s);
                vector<CHARBUFF> *vs = nullptr;
                offset += lt->get_type_handler(__record_mode::RM_READ)->read(dataptr, &vs, offset, DATASET_SIZE);
                CHECK_NOT_NULL(vs);
                tt->set_vs(vs);
                unordered_map<string, double *> *map = nullptr;
                offset += mt->get_type_handler(__record_mode::RM_READ)->read(dataptr, &map, offset, DATASET_SIZE);
                CHECK_NOT_NULL(map);
                tt->set_map(map);
                LOG_DEBUG("Read[%d] = [%d, %d, %s, %d, %d]", ii, *(tt->get_c()), *(tt->get_i()), tt->get_s(),
                          tt->get_vs()->size(),
                          tt->get_map()->size());
            }

            for (int ii = 0; ii < r_size; ii++) {
                test_types *tt = (*values)[ii];
                CHECK_AND_FREE(tt);
            }
            CHECK_AND_FREE(values);

            env_utils::dispose();
        } catch (const exception &e) {
            LOG_ERROR("Error occurred. [error=%s]", e.what());
            return (EXIT_FAILURE);
        }
    } else {
        cout << "Usage: " << argv[0] << " <config file> <schema file> \n";
        /** exit with failure condition **/
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
