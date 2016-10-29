//
// Created by Subhabrata Ghosh on 27/10/16.
//

#include "common/includes/timer.h"
#include "common/includes/alarm.h"
#include "common/includes/init_utils.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")

using namespace com::watergate::common;

#define COPY_FILE_PATH "/work/dev/wookler/ReactFS/common/test/data/test-conf.json"
#define COPY_DEST_PATH "test/copy"

int main(int argc, char **argv) {
    try {

        PRECONDITION(argc > 0);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);
        env_utils::create_env(configf);

        const __env *env = env_utils::get_env();
        CHECK_NOT_NULL(env);

        char dir[256];
        getcwd(dir, 256);

        LOG_DEBUG("Current directory [%s]", dir);

        Path p("/tmp/test/dir/file.txt");
        LOG_DEBUG("Parent dir [%s]", p.get_parent_dir().c_str());
        p = Path("who/cares/file.txt");
        LOG_DEBUG("Parent dir [%s]", p.get_parent_dir().c_str());
        const Path source(COPY_FILE_PATH);
        Path *dest = env->get_temp_dir(COPY_DEST_PATH, 0755);

        file_copy::copy(&source, dest);

        LOG_DEBUG("Copied file [source=%s] to [dest=%s]", source.get_path().c_str(), dest->get_path().c_str());

        vector<string> lines = file_copy::copy_lines(source);
        CHECK_NOT_EMPTY(lines);
        for (auto line : lines) {
            LOG_DEBUG("LINE = [%s]", line.c_str());
        }
        dest->remove();

        PRECONDITION(!dest->exists());
        LOG_DEBUG("Deleted file %s", dest->get_path().c_str());

        delete (dest);
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