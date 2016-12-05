//
// Created by Subhabrata Ghosh on 21/11/16.
//

#ifndef REACTFS_MAPPED_DATA_H
#define REACTFS_MAPPED_DATA_H

#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "common.h"
#include "common_utils.h"
#include "base_error.h"
#include "log_utils.h"
#include "fmstream.h"

REACTFS_NS_COMMON
                class mapped_data {
                protected:
                    string filename;
                    void *base_ptr = nullptr;
                public:
                    mapped_data(string filename, uint64_t size) {
                        PRECONDITION(!IS_EMPTY(filename));
                        this->filename = filename;
                    }

                    mapped_data(string filename) {
                        PRECONDITION(!IS_EMPTY(filename));
                        this->filename = filename;
                    }

                    virtual ~mapped_data() {
                        base_ptr = nullptr;
                    }

                    void *get_base_ptr() {
                        CHECK_NOT_NULL(base_ptr);
                        return base_ptr;
                    }
                };

                class file_mapped_data : public mapped_data {
                private:
                    fmstream *stream;

                    void open_file(string filename) {
                        stream = new fmstream(filename.c_str());
                        CHECK_ALLOC(stream, TYPE_NAME(fmstream));
                        base_ptr = stream->data();
                        CHECK_NOT_NULL(base_ptr);

                        LOG_DEBUG("Opened mapped file. [file=%s]", filename.c_str());
                    }

                    void create_file(string filename, uint64_t size) {
                        stream = new fmstream(filename.c_str(), size);
                        CHECK_ALLOC(stream, TYPE_NAME(fmstream));
                        base_ptr = stream->data();
                        CHECK_NOT_NULL(base_ptr);

                        memset(base_ptr, 0, size);

                        LOG_DEBUG("Created new mapped file. [file=%s][size=%lu]", filename.c_str(), size);
                    }

                public:
                    file_mapped_data(string filename, uint64_t size, bool overwrite = false) : mapped_data(filename,
                                                                                                           size) {
                        Path p(this->filename);
                        if (p.exists()) {
                            if (overwrite) {
                                p.remove();
                                create_file(this->filename, size);
                            } else {
                                open_file(this->filename);
                            }
                        } else {
                            create_file(this->filename, size);
                        }
                    }

                    file_mapped_data(string filename) : mapped_data(filename) {
                        Path p(filename);
                        if (!p.exists()) {
                            throw BASE_ERROR("Specified mapped file does not exist. [file=%s]", filename.c_str());
                        }
                        open_file(filename);
                    }

                    ~file_mapped_data() override {
                        if (NOT_NULL(stream)) {
                            if (stream->is_open()) {
                                stream->flush();
                                stream->clear();
                            }
                            CHECK_AND_FREE(stream);
                        }
                    }

                    void flush() {
                        CHECK_NOT_NULL(stream);
                        PRECONDITION(stream->is_open());
                        stream->flush();
                    }
                };

                class shm_mapped_data : public mapped_data {
                private:
                    int shm_fd = -1;
                    bool is_server = false;
                public:
                    shm_mapped_data(string filename, uint64_t size, bool is_server = false) : mapped_data(filename,
                                                                                                          size) {
                        mode_t mode = O_CREAT | O_RDWR;
                        if (!is_server) {
                            mode = O_RDWR;
                        }
                        shm_fd = shm_open(this->filename.c_str(), mode, DEFAULT_RESOURCE_MODE);
                        if (shm_fd < 0) {
                            base_error te = BASE_ERROR(
                                    "Error creating shared memory handle. [name=%s][error=%s]",
                                    this->filename.c_str(), strerror(errno));
                            LOG_ERROR(te.what());
                            throw te;
                        }
                        ftruncate(shm_fd, size);
                        LOG_DEBUG("Sizing shared memory segment. [name=%s][size=%d]", this->filename.c_str(), size);

                        /* now map the shared memory segment in the address space of the process */
                        base_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                        if (base_ptr == MAP_FAILED) {
                            base_error te = BASE_ERROR(
                                    "Error mapping shared memory segment. [name=%s][error=%s]",
                                    this->filename.c_str(), strerror(errno));
                            LOG_ERROR(te.what());
                            throw te;
                        }
                        this->is_server = is_server;
                    }

                    ~shm_mapped_data() override {
                        if (shm_fd >= 0 && NOT_NULL(base_ptr)) {
                            if (is_server) {
                                close(shm_fd);
                                LOG_DEBUG("Closing SHM handle. [handle=%s][response=%s]", filename.c_str(),
                                          strerror(errno));
                                shm_unlink(filename.c_str());
                                LOG_DEBUG("Un-linking SHM handle. [handle=%s][response=%s]", filename.c_str(),
                                          strerror(errno));
                            } else {
                                close(shm_fd);
                                LOG_DEBUG("Closing SHM handle. [handle=%s][response=%s]", filename.c_str(),
                                          strerror(errno));
                            }
                        }
                    }
                };
REACTFS_NS_COMMON_END
#endif //REACTFS_MAPPED_DATA_H
