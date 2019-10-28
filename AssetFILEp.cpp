/*
 *  Author: Seo Gangwon
 *  Date: 2019-01-10
 *  Contact: seogs12@naver.com
 */

#include <cerrno>
#include "AssetFILEp.h"

int _s_ndk_util::android_read(void *cookie, char *buf, int size) {
    return AAsset_read((AAsset *) cookie, buf, size);
}

int _s_ndk_util::android_write(void *cookie, const char *buf, int size) {
    return EACCES; // can't provide write access to the apk
}

fpos_t _s_ndk_util::android_seek(void *cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset *) cookie, offset, whence);
}

int _s_ndk_util::android_close(void *cookie) {
    AAsset_close((AAsset *) cookie);
    return 0;
}

FILE* _s_ndk_util::android_fopen(const char *filename, const char *mode, AAssetManager *assetManager) {
    if (mode[0] == 'w') return NULL;

    AAsset *asset = AAssetManager_open(assetManager, filename, 0);
    if (!asset) return NULL;

    return funopen(asset, android_read, android_write, android_seek, android_close);
}