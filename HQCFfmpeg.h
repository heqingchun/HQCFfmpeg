#ifndef HQCCFFMPEG_H
#define HQCCFFMPEG_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include <opencv2/opencv.hpp>

#include "HQCThread.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libavformat/version.h>
    #include <libavutil/time.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/imgutils.h>
}

class HQCCFfmpeg : public HQCCThread
{
    Q_OBJECT
public:
    HQCCFfmpeg(const QString& path);
    ~HQCCFfmpeg();
signals:
    void signalReceiveFrame(const cv::Mat& frame);
protected:
    void Init() override;
    void Work() override;
private:
    AVFormatContext *p_AVFormatContext = nullptr;
    AVPacket *p_AVPacket = nullptr;
    AVFrame *p_AVFrame = nullptr;
    AVFrame *p_AVFrameRGB = nullptr;
    AVCodec *p_AVCodec = nullptr;
    AVCodecContext *p_AVCodecContext = nullptr;
    AVDictionary *p_AVDictionary = nullptr;
    struct SwsContext* p_SwsContext = nullptr;
    int videoStreamIndex = -1;
    AVPixelFormat m_AVPixelFormat;
    uint8_t* buffer = nullptr;
    QString m_VideoPath;
    bool m_InitSuccess = false;
};

#endif // HQCCFFMPEG_H
