//
// Created by Subhabrata Ghosh on 01/09/16.
//

#ifndef WATERGATE_FILEUTILS_H
#define WATERGATE_FILEUTILS_H

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

#include "common_utils.h"
#include "base_error.h"

#define CONST_DEFAULT_DIRECTORY "watergate"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define CONST_PATH_DELIM "\\"
#define CONST_PATH_ROOT "C:\\"
#else
#define CONST_PATH_DELIM "/"
#define CONST_PATH_ROOT "/"
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define DEFAULT_TEMP_DIR
#elif defined(__ANDROID__)
#define DEFAULT_TEMP_DIR "/data/local/tmp"
#else
#define DEFAULT_TEMP_DIR "/tmp"
#endif

#define SKIP_DOT_DIRS(name) { \
    if (name) { \
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) { \
            continue; \
        } \
    } \
}

using namespace com::watergate::common;

namespace com {
    namespace watergate {
        namespace common {
            class file_utils {
                static string TEMP_DIR;
            public:
                static void set_temp_path(string path) {
                    TEMP_DIR = path;
                }

                static string concat_path(const string path, const string dir) {
                    return common_utils::format("%s%s%s", path.data(), CONST_PATH_DELIM, dir.data());
                }

                static bool is_file(const char *path) {
                    struct stat buf;
                    stat(path, &buf);
                    return S_ISREG(buf.st_mode);
                }

                static bool is_dir(const char *path) {
                    struct stat buf;
                    stat(path, &buf);
                    return S_ISDIR(buf.st_mode);
                }

                static inline bool file_exists(const std::string &name) {
                    struct stat buffer;
                    return (stat(name.c_str(), &buffer) == 0);
                }

                static inline string cannonical_path(const string path) {
                    return path;
                }

                static bool remove_file(const string path, bool delete_self) {
                    assert(!path.empty());
                    cout << "Deleting " << path << " ...\n";
                    if (!file_exists(path)) {
                        return false;
                    }
                    if (is_dir(path.data())) {
                        const char *dname = path.c_str();
                        DIR *dir = opendir(dname);
                        struct dirent *dent;
                        if (dir != NULL) {
                            while ((dent = readdir(dir))) {
                                SKIP_DOT_DIRS(dent->d_name);
                                string rp = file_utils::concat_path(path, dent->d_name);
                                remove_file(rp, true);
                            }
                        }
                        closedir(dir);
                    }
                    if (delete_self) {
                        return (remove(path.data()) == 0);
                    }
                    return true;
                }

                static string create_directory(const string path, mode_t mode, bool overwrite) {
                    assert(!path.empty());

                    string p = cannonical_path(path);

                    if (file_exists(p.data())) {
                        if (overwrite) {
                            remove_file(p, true);
                        } else {
                            if (is_file(p.data())) {
                                throw
                                        BASE_ERROR("File exists with specified name. [%s]", p.data());
                            }
                            return p;
                        }
                    }
                    return create_dirs(p, mode);
                }

                static string create_directory(const string path, mode_t mode) {
                    return create_directory(path, mode, true);
                }

                static string create_temp_directory(const string name, mode_t mode, bool overwrite) {
                    assert(!name.empty());

                    string tp = TEMP_DIR;

                    string p = common_utils::format("%s%s%s%s%s", tp.data(), CONST_PATH_DELIM, CONST_DEFAULT_DIRECTORY,
                                                    CONST_PATH_DELIM, name.data());

                    p = cannonical_path(p);

                    if (!file_exists(p)) {
                        create_directory(p, mode, overwrite);
                    }

                    return p;
                }

                static string create_temp_directory(const string name, mode_t mode) {
                    return create_temp_directory(name, mode, true);
                }

                static string create_temp_file(string path, string ext, mode_t mode, bool overwrite) {
                    assert(!path.empty());

                    if (path.front() == '/') {
                        path = create_directory(path, mode, false);
                    } else {
                        path = create_temp_directory(path, mode, false);
                    }

                    string name = common_utils::uuid();

                    string file;
                    if (!ext.empty()) {
                        file = common_utils::format("%s%s%s.%s", path.data(), CONST_PATH_DELIM, name.data(),
                                                    ext.data());
                    } else {
                        file = common_utils::format("%s%s%s", path.data(), CONST_PATH_DELIM, name.data());
                    }

                    file = cannonical_path(file);

                    if (file_exists(file)) {
                        if (overwrite) {
                            remove_file(file, true);
                        } else {
                            throw BASE_ERROR("File already exists. [file=%s]", file.data());
                        }
                    }

                    return file;
                }

                static string create_temp_file(string path, string ext, mode_t mode) {
                    return create_temp_file(path, ext, mode, true);
                }

                static bool clean_directory(string path) {
                    assert(!path.empty());

                    if (is_dir(path.data())) {
                        remove_file(path, false);
                    }
                    return false;
                }

                static string create_dirs(string path, mode_t mode) {
                    assert(!path.empty());

                    path = common_utils::trim(path);
                    path = cannonical_path(path);

                    string s;

                    vector<string> parts = common_utils::split(path, '/');
                    if (parts.size() > 0) {
                        if (parts.size() == 1) {
                            string dir = parts[0];
                            if (!dir.empty()) {
                                int r = create_dir(dir, mode);
                                if (r == 0)
                                    s = dir;
                            }
                        } else {
                            string n_path;
                            char c = path.data()[0];
                            if (c == '/') {
                                n_path += "/";
                            }
                            for (uint32_t ii = 0; ii < parts.size(); ii++) {
                                if (parts[ii].empty())
                                    continue;
                                if (ii > 0)
                                    n_path += "/";
                                n_path += parts[ii];

                                int r = create_dir(n_path, mode);
                                if (r != 0) {
                                    if (!parts[ii].empty())
                                        throw BASE_ERROR("Error creating directory. [directory=%s",
                                                         parts[ii].data());
                                    else {
                                        throw BASE_ERROR("Error creating directory. Directory name is empty");
                                    }
                                }
                            }
                            s = n_path;
                        }
                    }

                    return s;
                }

                static int create_dir(string path, mode_t mode) {
                    assert(!path.empty());

                    struct stat st;
                    int status = 0;

                    if (stat(path.data(), &st) != 0) {
                        /* Directory does not exist. EEXIST for race condition */
                        if (mkdir(path.data(), mode) != 0 && errno != EEXIST)
                            status = -1;
                    } else if (!S_ISDIR(st.st_mode)) {
                        errno = ENOTDIR;
                        status = -1;
                    }

                    return (status);
                }
            };

            class Path {
            private:
                string *path;

            public:
                Path() {
                    path = new string();
                }

                ~Path() {
                    CHECK_AND_FREE(path);
                }

                Path(const string p) {
                    path = new string();
                    append(p);
                }

                const string get_path() const {
                    return *path;
                }

                const string *get_path_p() const {
                    return path;
                }

                const string append(string p) {
                    assert(!p.empty());

                    if (path->empty()) {
                        path = new string(p);
                    } else {
                        string np = file_utils::concat_path(*path, p);
                        path->clear();
                        path->append(np);
                    }

                    return *path;
                }

                bool exists() const {
                    if (!path->empty()) {
                        return file_utils::file_exists(path->c_str());
                    }
                    return false;
                }

                const string get_parent_dir() const {
                    vector<string> parts = common_utils::split(*path, '/');
                    if (parts.size() > 1) {
                        string ss = string(parts[0]);
                        for (uint32_t ii = 1; ii < parts.size() - 2; ii++) {
                            ss.append(parts[ii]);
                        }
                        return ss;
                    }
                    return EMPTY_STRING;
                }

                bool is_file() const {
                    if (!path->empty()) {
                        return file_utils::is_file(path->c_str());
                    }
                    return false;
                }

                bool is_directory() const {
                    if (!path->empty()) {
                        return file_utils::is_dir(path->c_str());
                    }
                    return false;
                }

                const string create(mode_t mode) {
                    if (!path->empty()) {
                        return file_utils::create_directory(*path, mode);
                    }
                    return *path;
                }

                bool remove() {
                    if (!path->empty()) {
                        return file_utils::remove_file(*path, true);
                    }
                    return false;
                }

                bool clear() {
                    if (!path->empty()) {
                        return file_utils::clean_directory(*path);
                    }
                    return false;
                }
            };
        }
    }
}

#endif //WATERGATE_FILEUTILS_H
