#ifndef CPLAY_H
#define CPLAY_H

#include <QObject>

#include "HQCFfmpeg.h"

class CPlay : public QObject
{
    Q_OBJECT
public:
    explicit CPlay(QObject *parent = nullptr);
    ~CPlay();
signals:
private slots:
    void slotReceiveFrame(const cv::Mat& frame);
public:
    void SaveJpg(const QString& path);
    void SaveMp4(const QString& path,int time);
private:
    HQCCFfmpeg *p_HQCCFfmpeg = nullptr;
    cv::Mat m_Frame;
    cv::VideoWriter m_VideoWriter;
};

#endif // CPLAY_H
