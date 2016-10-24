//
// Created by Subhabrata Ghosh on 17/10/16.
//

#include "./includes/lock_env_jni.h"

#include "includes/common/common.h"
#include "includes/common/__env.h"
#include "includes/core/init_utils.h"

using namespace com::watergate::common;
using namespace com::watergate::core;

JNIEXPORT void JNICALL Java_com_watergate_library_LockEnv_create
        (JNIEnv *jniEnv, jobject obj, jstring filename, jstring appname) {
    try {
        jboolean isCopy = false;
        const char *c_fname = jniEnv->GetStringUTFChars(filename, &isCopy);
        const char *c_aname = jniEnv->GetStringUTFChars(appname, &isCopy);

        string fname = string(c_fname);
        CHECK_NOT_EMPTY(fname);
        string aname = string(c_aname);
        CHECK_NOT_EMPTY(aname);

        init_utils::create_env(fname, aname);

        jniEnv->ReleaseStringUTFChars(filename, c_fname);
        jniEnv->ReleaseStringUTFChars(appname, c_aname);
    } catch (const exception &e) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, "Unhandled exception occurred. [source=Java_com_watergate_library_LockEnv_create]");
    }
}

JNIEXPORT void JNICALL Java_com_watergate_library_LockEnv_dispose
        (JNIEnv *jniEnv, jobject obj) {
    try {
        init_utils::dispose();
    } catch (const exception &e) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, "Unhandled exception occurred. [source=Java_com_watergate_library_LockEnv_dispose]");
    }
}