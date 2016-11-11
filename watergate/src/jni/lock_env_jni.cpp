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

#include "./includes/lock_env_jni.h"

#include "common/includes/common.h"
#include "common/includes/__env.h"
#include "watergate/includes/init_utils.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::watergate::core;

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