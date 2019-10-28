/*
 *  Author: Seo Gangwon
 *  Date: 2019-01-10
 *  Contact: seogs12@naver.com
 */

#ifndef CAMERA2NDK_ASSETFILEP_H
#define CAMERA2NDK_ASSETFILEP_H

#include <cstdio>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

// ------------------ asset to FILE* -------------------------------------------------------------------
namespace _s_ndk_util {
    int android_read(void *cookie, char *buf, int size);
    int android_write(void *cookie, const char *buf, int size);
    fpos_t android_seek(void *cookie, fpos_t offset, int whence);
    int android_close(void *cookie);

    FILE *android_fopen(const char *filename, const char *mode, AAssetManager *assetManager);
}

#endif //CAMERA2NDK_ASSETFILEP_H
