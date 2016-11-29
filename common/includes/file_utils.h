/* 
 * Copyright [2016] [Subhabrata Ghosh] 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 *       limitations under the License. 
 * 
 */
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
#include <fstream>

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

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Utility class to perform file/directory related operations.
                 */
                class file_utils {
                    /// Default path to the temp directory
                    static string TEMP_DIR;
                public:
                    /*!
                     * Set the path to the default temp directory.
                     *
                     * @param path - Path to temp directory
                     */
                    static void set_temp_path(string path) {
                        TEMP_DIR = path;
                    }

                    /*!
                     * Concatenate the specified file/folder name to the end of the path.
                     *
                     * @param path - Base path
                     * @param dir - File/directory to concatenate.
                     *
                     * @return - Concatenated path.
                     */
                    static string concat_path(const string path, const string dir) {
                        return common_utils::format("%s%s%s", path.c_str(), CONST_PATH_DELIM, dir.c_str());
                    }

                    /*!
                     * Check if the specified path represents a file
                     *
                     * @param path - Path to check
                     * @return - Is file?
                     */
                    static bool is_file(const char *path) {
                        CHECK_NOT_NULL(path);
                        if (file_exists(string(path))) {
                            struct stat buf;
                            stat(path, &buf);
                            return S_ISREG(buf.st_mode);
                        }
                        return false;
                    }

                    /*!
                     * Check if the specified path represents a directory
                     *
                     * @param path - Path to check
                     * @return - Is directory?
                     */
                    static bool is_dir(const char *path) {
                        CHECK_NOT_NULL(path);
                        if (file_exists(string(path))) {
                            struct stat buf;
                            stat(path, &buf);
                            return S_ISDIR(buf.st_mode);
                        }
                        return false;
                    }

                    /*!
                     * Check if the specified path exists, as a file or a directory.
                     *
                     * @param name - Path to check.
                     * @return - Exists?
                     */
                    static inline bool file_exists(const std::string &name) {
                        struct stat buffer;
                        return (stat(name.c_str(), &buffer) == 0);
                    }

                    /*!
                     * Get the resolved/normalized path for the specified path. Will only resolve/normalize
                     * if the specified path exists.
                     *
                     * @param path - Path to resolve/normalize.
                     * @return - Normalized path.
                     */
                    static inline string cannonical_path(const string &path) {
                        if (!IS_EMPTY(path)) {
                            if (file_exists(path)) {
                                char buff[SIZE_MAX_PATH];
                                memset(buff, 0, SIZE_MAX_PATH);
                                realpath(path.c_str(), buff);
                                string p(buff);
                                if (!IS_EMPTY(p)) {
                                    return p;
                                }
                            }
                        }
                        return path;
                    }

                    /*!
                     * Remove the specified path. If the path is a directory, will also remove all the contents.
                     *
                     * @param path - Path to remove.
                     * @param delete_self - Delete the base directory, applicable if path is a directory.
                     * @return - Has been deleted?
                     */
                    static bool remove_file(const string &path, bool delete_self) {
                        PRECONDITION(!path.empty());
                        cout << "Deleting " << path << " ...\n";
                        if (!file_exists(path)) {
                            return false;
                        }
                        if (is_dir(path.c_str())) {
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
                            return (remove(path.c_str()) == 0);
                        }
                        return true;
                    }

                    /*!
                     * Get the contents of the specified directory.
                     *
                     * @param path - Path of directory.
                     * @param r - Vector of content paths.
                     */
                    static void get_dir_content(const string &path, vector <string> *r) {
                        DIR *dir = opendir(path.c_str());
                        struct dirent *dent;
                        if (dir != NULL) {
                            while ((dent = readdir(dir))) {
                                SKIP_DOT_DIRS(dent->d_name);
                                string rp = file_utils::concat_path(path, dent->d_name);
                                r->push_back(rp);
                            }
                        }
                    }

                    /*!
                     * Check if the specified directory is empty.
                     *
                     * @param path - Path of directory.
                     * @return - Is empty?
                     */
                    static bool is_dir_empty(const string &path) {
                        if (!is_dir(path.c_str())) {
                            return false;
                        }
                        uint32_t count = 0;
                        DIR *dir = opendir(path.c_str());
                        struct dirent *dent;
                        if (dir != NULL) {
                            while ((dent = readdir(dir))) {
                                SKIP_DOT_DIRS(dent->d_name);
                                count++;
                            }
                        }
                        return (count > 0);
                    }

                    /*!
                     * Create a new directory at the specified path.
                     *
                     * @param path - Path of directory.
                     * @param mode - Mode to create the directory with.
                     * @param overwrite - Overwrite the directory, if it already exists.
                     * @return - Normalized path of the directory.
                     */
                    static string create_directory(const string &path, mode_t mode, bool overwrite) {
                        PRECONDITION(!path.empty());

                        string p = cannonical_path(path);

                        if (file_exists(p.c_str())) {
                            if (overwrite) {
                                remove_file(p, true);
                            } else {
                                if (is_file(p.c_str())) {
                                    throw
                                            BASE_ERROR("File exists with specified name. [%s]", p.c_str());
                                }
                                return p;
                            }
                        }
                        return create_dirs(p, mode);
                    }

                    /*!
                     * Create a new directory at the specified path.
                     *
                     * @param path - Path of directory.
                     * @param mode - Mode to create the directory with.
                     * @return - Normalized path of the directory.
                     */
                    static string create_directory(const string &path, mode_t mode) {
                        return create_directory(path, mode, true);
                    }

                    /*!
                     * Create a temporary directory with the specified sub-path.
                     *
                     * @param name - Sub-path to create under the temp directory.
                     * @param mode - Mode to create the directory with.
                     * @param overwrite - Overwrite, if the directory already exists.
                     * @return - Normalized path of the directory.
                     */
                    static string create_temp_directory(const string &name, mode_t mode, bool overwrite) {
                        PRECONDITION(!name.empty());

                        string tp = TEMP_DIR;

                        string p = common_utils::format("%s%s%s%s%s", tp.c_str(), CONST_PATH_DELIM,
                                                        CONST_DEFAULT_DIRECTORY,
                                                        CONST_PATH_DELIM, name.c_str());

                        p = cannonical_path(p);

                        if (!file_exists(p)) {
                            create_directory(p, mode, overwrite);
                        }

                        return p;
                    }

                    /*!
                     * Create a temporary directory with the specified sub-path.
                     *
                     * @param name - Sub-path to create under the temp directory.
                     * @param mode - Mode to create the directory with.
                     * @return - Normalized path of the directory.
                     */
                    static string create_temp_directory(const string &name, mode_t mode) {
                        return create_temp_directory(name, mode, true);
                    }

                    /*!
                     * Create a new file under the temporary directory space.
                     * This call will not actually create a new file, but generate the path and return it.
                     *
                     * @param ext - Extension of the the file.
                     * @param mode - Mode to create the file with.
                     * @param overwrite - Overwrite file, if exists.
                     * @param filename - Filename to use (default = UUID)
                     * @param path - Sub-path under the temp space. (default = EMPTY_STRING)
                     * @return - Path to created file.
                     */
                    static string
                    create_temp_file(string ext, mode_t mode, bool overwrite, string filename = EMPTY_STRING,
                                     string path = EMPTY_STRING) {
                        PRECONDITION(!path.empty());

                        if (!IS_EMPTY(path)) {
                            path = create_temp_directory(path, mode, false);
                        } else {
                            path = string(TEMP_DIR);
                        }

                        string name = common_utils::uuid();
                        if (!IS_EMPTY(filename)) {
                            name = filename;
                        }

                        string file;
                        if (!ext.empty()) {
                            file = common_utils::format("%s%s%s.%s", path.c_str(), CONST_PATH_DELIM, name.c_str(),
                                                        ext.c_str());
                        } else {
                            file = common_utils::format("%s%s%s", path.c_str(), CONST_PATH_DELIM, name.c_str());
                        }

                        file = cannonical_path(file);

                        if (file_exists(file)) {
                            if (overwrite) {
                                remove_file(file, true);
                            } else {
                                throw BASE_ERROR("File already exists. [file=%s]", file.c_str());
                            }
                        }

                        return file;
                    }

                    /*!
                     * Create a new file under the temporary directory space.
                     * This call will not actually create a new file, but generate the path and return it.
                     *
                     * @param ext - Extension of the the file.
                     * @param mode - Mode to create the file with.
                     * @param filename - Filename to use (default = UUID)
                     * @param path - Sub-path under the temp space. (default = EMPTY_STRING)
                     * @return - Path to created file.
                     */
                    static string
                    create_temp_file(string ext, mode_t mode, string filename = EMPTY_STRING,
                                     string path = EMPTY_STRING) {
                        return create_temp_file(ext, mode, true, filename, path);
                    }

                    /*!
                     * Clear the contents of the specified directory. Will not delete the
                     * directory itself.
                     *
                     * @param path - Path to the directory.
                     * @return - Has been cleared?
                     */
                    static bool clean_directory(string path) {
                        PRECONDITION(!path.empty());

                        if (is_dir(path.c_str())) {
                            remove_file(path, false);
                        }
                        return false;
                    }

                    /*!
                     * Make directories recursively. Will find the highest level directory missing and
                     * create all the sub-folders.
                     *
                     * @param path - Directory path to create.
                     * @param mode - Mode to create the directory(s) with.
                     * @return - Path of the leaf directory created.
                     */
                    static string create_dirs(string path, mode_t mode) {
                        PRECONDITION(!path.empty());

                        path = string_utils::trim(path);
                        path = cannonical_path(path);

                        string s;
                        vector <string> parts;

                        string_utils::split(path, '/', &parts);
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
                                char c = path.c_str()[0];
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
                                                             parts[ii].c_str());
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

                    /*!
                     * Create the leaf directory. Assumes all parents already exists.
                     *
                     * @param path - Path to the directory.
                     * @param mode - Mode to create the directory with.
                     * @return - Create status.
                     */
                    static int create_dir(string path, mode_t mode) {
                        PRECONDITION(!path.empty());

                        struct stat st;
                        int status = 0;

                        if (stat(path.c_str(), &st) != 0) {
                            /* Directory does not exist. EEXIST for race condition */

                            if (mkdir(path.c_str(), mode) != 0 && errno != EEXIST)
                                status = -1;
                        } else if (!S_ISDIR(st.st_mode)) {
                            errno = ENOTDIR;
                            status = -1;
                        }

                        return (status);
                    }

                    /*!
                     * Get the size of the specified file.
                     *
                     * @param path - Path to the file.
                     * @return - Size, if exists, else -1
                     */
                    static off_t get_file_size(string path) {
                        PRECONDITION(!IS_EMPTY(path));
                        PRECONDITION(file_exists(path));
                        PRECONDITION(is_file(path.c_str()));
                        struct stat st;

                        if (stat(path.c_str(), &st) == 0)
                            return st.st_size;

                        return -1;
                    }
                };

                /*!
                 * Class represents a file system path.
                 */
                class Path {
                private:
                    /// Path string
                    string *path = new string();

                public:
                    /*!<constructor
                     * Default empty constructor.
                     */
                    Path() {
                    }

                    /*!<constructor
                     * Initialize the path with the specified path string.
                     *
                     * @param p - Path string.
                     */
                    Path(const string p) {
                        append(p);
                    }

                    /*!<destructor
                     * Default desctructor.
                     */
                    ~Path() {
                        CHECK_AND_FREE(this->path);
                    }

                    /*!
                     * Get the path string for this path.
                     *
                     * @return - Path string.
                     */
                    const string get_path() const {
                        return *path;
                    }

                    /*!
                    * Get the path string for this path.
                    *
                    * @return - Path string pointer.
                    */
                    const string *get_path_p() const {
                        return path;
                    }

                    /*!
                     * Append the specified path string to this path.
                     *
                     * @param p - Path string to append.
                     * @return - Appened path string.
                     */
                    const string append(const string p) {
                        CHECK_NOT_EMPTY(p);

                        if (IS_EMPTY_P(path)) {
                            path->append(p);
                        } else {
                            string sp = string(*path);
                            string np = file_utils::concat_path(sp, p);
                            path->clear();
                            path->append(np);
                        }

                        return *path;
                    }

                    /*!
                     * Check if this path exists in the file system.
                     *
                     * @return - Path exists?
                     */
                    bool exists() const {
                        if (!path->empty()) {
                            return file_utils::file_exists(path->c_str());
                        }
                        return false;
                    }

                    /*!
                     * Get the parent directory for this path.
                     *
                     * @return - Parent directory, or empty string if root.
                     */
                    const string get_parent_dir() const {
                        vector <string> parts;
                        string_utils::split(*path, '/', &parts);
                        if (parts.size() > 1) {
                            string ss = string();
                            if (!IS_EMPTY(parts[0])) {
                                ss.append(parts[0]);
                            }
                            for (uint32_t ii = 1; ii < parts.size() - 1; ii++) {
                                ss.append("/");
                                ss.append(parts[ii]);
                            }
                            return ss;
                        }
                        return EMPTY_STRING;
                    }

                    /*!
                     * Get the filename of this path, basically the leaf node, can be a file or directory.
                     *
                     * @return - File name
                     */
                    const string get_filename() const {
                        vector <string> parts;
                        string_utils::split(*path, '/', &parts);
                        if (parts.size() > 0) {
                            return string(parts[parts.size() - 1]);
                        }
                        return EMPTY_STRING;
                    }

                    /*!
                     * Get the file extension for this path. Will perform a file check if the check parameter is true.
                     *
                     * @param check - Check if the file exists and is a file.
                     * @return - File extension.
                     */
                    const string get_extension(bool check = false) const {
                        if (check && !is_file()) {
                            return EMPTY_STRING;
                        }
                        const string file = get_filename();
                        if (!IS_EMPTY(file)) {
                            vector <string> parts;
                            string_utils::split(file, '.', &parts);
                            if (parts.size() > 0) {
                                return string(parts[parts.size() - 1]);
                            }
                        }
                        return EMPTY_STRING;
                    }

                    /*!
                     * Check if the file pointed to by this path is a file.
                     *
                     * @return - Is file?
                     */
                    bool is_file() const {
                        if (!path->empty()) {
                            return file_utils::is_file(path->c_str());
                        }
                        return false;
                    }

                    /*!
                     * Check if the file pointed to by this path is a directory.
                     *
                     * @return - Is directory?
                     */
                    bool is_directory() const {
                        if (!path->empty()) {
                            return file_utils::is_dir(path->c_str());
                        }
                        return false;
                    }

                    /*!
                     * Create the directories represented by this path on the file system.
                     *
                     * @param mode - Mode to create the directories with.
                     * @return - Path to directory.
                     */
                    const string create(mode_t mode) {
                        if (!path->empty()) {
                            return file_utils::create_directory(*path, mode);
                        }
                        return *path;
                    }

                    /*!
                     * Remove the file/directory represented by this path, if directory, will remove all
                     * contents too.
                     *
                     * @return - Has been removed?
                     */
                    bool remove() {
                        if (!path->empty()) {
                            return file_utils::remove_file(*path, true);
                        }
                        return false;
                    }

                    /*!
                     * Clear the contents of this directory.
                     *
                     * @return - Has contents been cleared?
                     */
                    bool clear() {
                        if (!path->empty() && is_directory()) {
                            return file_utils::clean_directory(*path);
                        }
                        return false;
                    }
                };

                /*!
                 * Utility methods to help with copying files.
                 */
                class file_copy {
                public:
                    /*!
                     * Copy the contents of the source file to the specified destination file.
                     *
                     * @param source - Source file to copy data from.
                     * @param dest - Destination file path.
                     * @return - Number of bytes copied.
                     */
                    static uint64_t copy(const Path *source, Path *dest) {
                        PRECONDITION(source->exists());
                        if (dest->exists()) {
                            dest->remove();
                        }

                        std::ifstream src(source->get_path().c_str(), std::ios::binary);
                        std::ofstream dst(dest->get_path().c_str(), std::ios::binary);

                        dst << src.rdbuf();

                        uint64_t pos = dst.tellp();

                        return pos;
                    }

                };

                /*!
                 * Utility functions for file contents.
                 */
                class file_helper {
                public:
                    /*!
                     * Read the content of the file into a vector of lines. Will read the file line-by-line.
                     *
                     * @param source - Source file to read from.
                     * @param lines - Vector to copy the lines to.
                     * @param empty_lines - Read empty lines?
                     * @return
                     */
                    static void copy_lines(const Path& source, vector <string> *lines, bool empty_lines = true) {
                        std::ifstream infile(source.get_path());
                        std::string line;
                        while (std::getline(infile, line)) {
                            if (IS_EMPTY(line) && !empty_lines) {
                                continue;
                            }
                            lines->push_back(line);
                        }
                    }

                    /*!
                     * Read the content of the file into a vector of lines. Will read the file line-by-line.
                     *
                     * @param file - File to read.
                     * @param v - Vector to copy the lines to.
                     * @param empty_lines - Read empty lines?
                     * @return - Number of lines read.
                     */
                    static uint64_t readlines(ifstream *file, vector <string> *v, bool empty_lines = true) {
                        if (!file->is_open()) {
                            throw BASE_ERROR("Input stream is not open.");
                        }
                        string line;
                        while (getline(*file, line)) {
                            if (IS_EMPTY(line) && !empty_lines) {
                                continue;
                            }
                            v->push_back(line);
                        }
                        if (file->bad()) {
                            throw BASE_ERROR("Error while reading file.");
                        }
                        return v->size();
                    }
                };
            }
        }
    }
}

#endif //WATERGATE_FILEUTILS_H
