#include <jni.h>
#include <string>
#include <android/log.h>

#include "tinycap-lib.h"
#include "tinymix-lib.h"

#define LOG_TAG "natile-lib"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_ubiquitous_1pancake_callcapture_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_ubiquitous_1pancake_callcapture_CallRecordingService_tinymix(JNIEnv *env, jobject thiz) {
    const int card = 0;
    auto control = const_cast<char const*>("0");

    char* valueVal = const_cast<char*>("1");
    char** value = &valueVal;

    struct mixer *mixer = mixer_open(card);
    if (!mixer) {
        LOGE( "Failed to open mixer\n");
        return ENODEV;
    }

    int ret = tinymix_set_value(mixer,control,value,1);

    mixer_close(mixer);

    return ret;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_ubiquitous_1pancake_callcapture_CallRecordingService_stopTinycapCapturing(JNIEnv *env, jobject thiz) {
    stop_capturing();
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_ubiquitous_1pancake_callcapture_CallRecordingService_tinycap(JNIEnv *env, jobject thiz,
                                                                      jstring path) {
    FILE *file;
    struct wav_header header;
    unsigned int card = 0;
    unsigned int device = 0;
    unsigned int channels = 2;
    unsigned int rate = 44100;
    unsigned int bits = 16;
    unsigned int frames;
    unsigned int period_size = 1024;
    unsigned int period_count = 4;
    unsigned int cap_time = 0;
    enum pcm_format format;

    const char *raw_path = env->GetStringUTFChars(path, 0);
    file = fopen(raw_path, "wb");
    if (!file) {
        LOGE( "Unable to create file '%s'\n", raw_path);
        return 1;
    }

    header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = channels;
    header.sample_rate = rate;

    switch (bits) {
        case 32:
            format = PCM_FORMAT_S32_LE;
            break;
        case 24:
            format = PCM_FORMAT_S24_LE;
            break;
        case 16:
            format = PCM_FORMAT_S16_LE;
            break;
        default:
            (stderr, "%u bits is not supported.\n", bits);
            fclose(file);
            return 1;
    }

    header.bits_per_sample = pcm_format_to_bits(format);
    header.byte_rate = (header.bits_per_sample / 8) * channels * rate;
    header.block_align = channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;

    fseek(file, sizeof(struct wav_header), SEEK_SET);

    frames = capture_sample(file, card, device, header.num_channels,
                            header.sample_rate, format,
                            period_size, period_count, cap_time);
    LOGI("Captured %u frames\n", frames);

    header.data_sz = frames * header.block_align;
    header.riff_sz = header.data_sz + sizeof(header) - 8;
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(struct wav_header), 1, file);

    fclose(file);

    return 0;
}