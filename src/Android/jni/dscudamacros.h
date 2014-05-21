#ifndef DSCUDA_MACROS_H
#define DSCUDA_MACROS_H
#include <jni.h>
#include <errno.h>
#include <android/log.h>


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Debug C Code", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Debug C Code", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"Debug C Code", __VA_ARGS__))
#define WARN(lv, fmt, args...) LOGE(fmt, ## args);
// if (lv <= dscudaWarnLevel()) fprintf(stderr, fmt, ## args);
#define WARNONCE(lv, fmt, args...) if (lv <= dscudaWarnLevel()) { \
        static int firstcall = 1;                                 \
        if (firstcall) {                                          \
            firstcall = 0;                                        \
            fprintf(stderr, fmt, ## args);                        \
        }                                                         \
    }
int dscudaWarnLevel(void);
void dscudaSetWarnLevel(int level);

#endif // DSCUDA_MACROS_H
