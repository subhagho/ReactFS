//
// Created by Subhabrata Ghosh on 17/10/16.
//

#include "./includes/lock_manager_jni.h"
#include "includes/common/common.h"
#include "includes/core/control_manager.h"
#include "includes/core/init_utils.h"

static control_manager *control_manager_inst = nullptr;

JNIEXPORT void JNICALL Java_com_watergate_library_LockControlManager_create
        (JNIEnv *jniEnv, jobject obj, jstring configpath) {
    try {
        const __env *env = init_utils::get_env();
        CHECK_NOT_NULL(env);
        CHECK_ENV_STATE(env);

        jboolean isCopy = false;
        const char *c_fname = jniEnv->GetStringUTFChars(configpath, &isCopy);
        string cpath = string(c_fname);
        CHECK_NOT_EMPTY(cpath);

        control_manager_inst = init_utils::init_control_manager(env, cpath);
        CHECK_NOT_NULL(control_manager_inst);
        CHECK_STATE_AVAILABLE(control_manager_inst->get_state());

        jniEnv->ReleaseStringUTFChars(configpath, c_fname);
    } catch (const exception &e) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlManager_create]");
    }
}

JNIEXPORT void JNICALL Java_com_watergate_library_LockControlManager_clearLocks
        (JNIEnv *jniEnv, jobject obj) {
    try {
        CHECK_NOT_NULL(control_manager_inst);
        control_manager_inst->clear_locks();
    } catch (const exception &e) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlManager_clearLocks]");
    }
}

JNIEXPORT void JNICALL Java_com_watergate_library_LockControlManager_dispose
        (JNIEnv *jniEnv, jobject obj) {
    try {
        CHECK_NOT_NULL(control_manager_inst);
        delete (control_manager_inst);
        control_manager_inst = nullptr;
    } catch (const exception &e) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlManager_dispose]");
    }
}