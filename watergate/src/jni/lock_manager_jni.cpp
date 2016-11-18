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
// Created by Subhabrata Ghosh on 17/10/16.
//

#include "./includes/lock_manager_jni.h"
#include "common/includes/common.h"
#include "watergate/includes/control_manager.h"
#include "watergate/includes/init_utils.h"

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

        control_manager_inst = init_utils::init_control_manager(env, cpath, false);
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