//
// Created by Subhabrata Ghosh on 17/10/16.
//

#include "./includes/lock_client_jni.h"
#include "includes/common/common.h"
#include "includes/core/control_def.h"
#include "includes/core/init_utils.h"

JNIEXPORT void JNICALL Java_com_watergate_library_LockControlClient_create
        (JNIEnv *jniEnv, jobject obj, jstring configpath) {
    try {
        const __env *env = init_utils::get_env();
        CHECK_NOT_NULL(env);
        CHECK_ENV_STATE(env);

        jboolean isCopy = false;
        const char *c_fname = jniEnv->GetStringUTFChars(configpath, &isCopy);
        string cpath = string(c_fname);
        CHECK_NOT_EMPTY(cpath);

        const control_client *client = init_utils::init_control_client(env, cpath);
        CHECK_NOT_NULL(client);
        CHECK_STATE_AVAILABLE(client->get_state());

        jniEnv->ReleaseStringUTFChars(configpath, c_fname);
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_create]");
    }
}

JNIEXPORT jstring JNICALL Java_com_watergate_library_LockControlClient_findLockByName
        (JNIEnv *jniEnv, jobject obj, jstring name, jint type) {
    try {
        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        jboolean isCopy = false;
        const char *c_name = jniEnv->GetStringUTFChars(name, &isCopy);
        string s_name = string(c_name);
        CHECK_NOT_EMPTY(s_name);

        resource_type_enum et = resource_def::parse_type(type);
        string lock = client->find_lock(s_name, et);
        jstring retval = nullptr;
        if (!IS_EMPTY(lock)) {
            retval = jniEnv->NewStringUTF(lock.c_str());
        }
        jniEnv->ReleaseStringUTFChars(name, c_name);

        return retval;
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_findLockByName]");
    }
    return nullptr;
}

JNIEXPORT jdouble JNICALL Java_com_watergate_library_LockControlClient_getQuota
        (JNIEnv *jniEnv, jobject obj, jstring name) {
    try {
        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        jboolean isCopy = false;
        const char *c_name = jniEnv->GetStringUTFChars(name, &isCopy);
        string s_name = string(c_name);
        CHECK_NOT_EMPTY(s_name);

        double retval = client->get_quota(s_name);

        jniEnv->ReleaseStringUTFChars(name, c_name);

        return retval;
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        return jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        return jniEnv->ThrowNew(e_class,
                                "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_findLockByName]");
    }
    return 0.0;
}

JNIEXPORT jint JNICALL Java_com_watergate_library_LockControlClient_lock__Ljava_lang_String_2ID
        (JNIEnv *jniEnv, jobject obj, jstring name, jint priority, jdouble quota) {
    try {
        pid_t pid = getpid();
        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        jboolean isCopy = false;
        const char *c_name = jniEnv->GetStringUTFChars(name, &isCopy);
        string s_name = string(c_name);
        CHECK_NOT_EMPTY(s_name);

        uint16_t retval = 0;
        int err = 0;
        _lock_state r = client->lock(s_name, priority, quota, &err);
        jniEnv->ReleaseStringUTFChars(name, c_name);

        if (r == Locked && err == 0) {
            retval = record_utils::get_lock_acquire_enum_int(Locked);
        } else if (r == Expired || err == ERR_CORE_CONTROL_TIMEOUT) {
            retval = record_utils::get_lock_acquire_enum_int(Timeout);
        } else if (r == QuotaReached) {
            retval = record_utils::get_lock_acquire_enum_int(QuotaReached);
        } else {
            throw BASE_ERROR("Lock call returned error. [return=%s][error=%d]",
                             record_utils::get_lock_acquire_enum_string(r).c_str(), err);
        }
        TRACE("[pid=%d] Lock stats = %s", pid, record_utils::get_lock_acquire_enum_string(r).c_str());
        return retval;
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        return jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        return jniEnv->ThrowNew(e_class,
                                "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_lock__Ljava_lang_String_2ID]");
    }
}

JNIEXPORT jint JNICALL Java_com_watergate_library_LockControlClient_lock__Ljava_lang_String_2IDJ
        (JNIEnv *jniEnv, jobject obj, jstring name, jint priority, jdouble quota, jlong timeout) {
    try {
        pid_t pid = getpid();

        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        jboolean isCopy = false;
        const char *c_name = jniEnv->GetStringUTFChars(name, &isCopy);
        string s_name = string(c_name);
        CHECK_NOT_EMPTY(s_name);

        uint16_t retval = 0;
        int err = 0;
        _lock_state r = client->lock(s_name, priority, quota, timeout, &err);
        jniEnv->ReleaseStringUTFChars(name, c_name);

        if (r == Locked && err == 0) {
            retval = record_utils::get_lock_acquire_enum_int(Locked);
        } else if (r == Expired || err == ERR_CORE_CONTROL_TIMEOUT) {
            retval = record_utils::get_lock_acquire_enum_int(Timeout);
        } else if (r == QuotaReached) {
            retval = record_utils::get_lock_acquire_enum_int(QuotaReached);
        } else {
            throw BASE_ERROR("Lock call returned error. [return=%s][error=%d]",
                             record_utils::get_lock_acquire_enum_string(r).c_str(), err);
        }
        TRACE("[pid=%d] Lock stats = %s", pid, record_utils::get_lock_acquire_enum_string(r).c_str());
        return retval;
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        return jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        return jniEnv->ThrowNew(e_class,
                                "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_lock__Ljava_lang_String_2IDJ]");
    }
}

JNIEXPORT void JNICALL Java_com_watergate_library_LockControlClient_register_1thread
        (JNIEnv *jniEnv, jobject obj, jstring name) {
    try {
        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        jboolean isCopy = false;
        const char *c_name = jniEnv->GetStringUTFChars(name, &isCopy);
        string s_name = string(c_name);
        CHECK_NOT_EMPTY(s_name);

        client->register_thread(s_name);

        jniEnv->ReleaseStringUTFChars(name, c_name);
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_register_1thread]");
    }
}

JNIEXPORT jboolean JNICALL Java_com_watergate_library_LockControlClient_release
        (JNIEnv *jniEnv, jobject obj, jstring name, jint priority) {
    try {
        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        jboolean isCopy = false;
        const char *c_name = jniEnv->GetStringUTFChars(name, &isCopy);
        string s_name = string(c_name);
        CHECK_NOT_EMPTY(s_name);

        jboolean r = client->release(s_name, priority);

        jniEnv->ReleaseStringUTFChars(name, c_name);

        return r;
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_release]");
    }
    return false;
}

JNIEXPORT jint JNICALL Java_com_watergate_library_LockControlClient_getControlState
        (JNIEnv *jniEnv, jobject obj) {
    try {
        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        const __state__ s = client->get_state();

        int r = s.get_state_int();

        return r;
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_getControlState]");
    }
    return 0;
}

JNIEXPORT jstring JNICALL Java_com_watergate_library_LockControlClient_getControlError
        (JNIEnv *jniEnv, jobject obj) {
    try {
        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        const __state__ s = client->get_state();

        jstring r = nullptr;
        if (s.has_error()) {
            const exception *e = s.get_error();
            if (NOT_NULL(e)) {
                const char *em = e->what();
                r = jniEnv->NewStringUTF(em);
            }
        }

        return r;
    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_getControlState]");
    }
    return nullptr;
}

JNIEXPORT void JNICALL Java_com_watergate_library_LockControlClient_dispose
        (JNIEnv *jniEnv, jobject obj) {

}

JNIEXPORT void JNICALL Java_com_watergate_library_LockControlClient_test_1assert
        (JNIEnv *jniEnv, jobject obj) {
    try {
        const control_client *client = init_utils::get_client();
        CHECK_NOT_NULL(client);

        client->test_assert();

    } catch (const exception &e) {
        LOG_WARN("Error getting lock. [error=%s]", e.what());
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class, e.what());
    } catch (...) {
        jclass e_class =
                jniEnv->FindClass("com/watergate/library/LockControlException");
        jniEnv->ThrowNew(e_class,
                         "Unhandled exception occurred. [source=Java_com_watergate_library_LockControlClient_test_1assert]");
    }
}