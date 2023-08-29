#include "Play.h"

CPlay::CPlay(QObject *parent) : QObject(parent){
    qRegisterMetaType<cv::Mat>("cv::Mat");
    p_HQCCFfmpeg = new HQCCFfmpeg("rtsp://admin:yoseen2018@192.168.192.101:554/Streaming/Channels/1");
    connect(p_HQCCFfmpeg,&HQCCFfmpeg::signalReceiveFrame,this,&CPlay::slotReceiveFrame,Qt::DirectConnection);
    p_HQCCFfmpeg->start();
}
CPlay::~CPlay(){
    if(p_HQCCFfmpeg) p_HQCCFfmpeg->deleteLater();
}
void CPlay::slotReceiveFrame(const cv::Mat& frame){
    if(!frame.empty()){
        m_Frame.release();
        m_Frame = frame;
    }
}
void CPlay::SaveJpg(const QString& path){
    if(!m_Frame.empty()){
        cv::imwrite(path.toStdString(),m_Frame);
        qDebug()<<"可见光图片保存成功";
    }else{
        qDebug()<<__FUNCTION__<<"可见光图片保存失败:m_Frame为空";
    }
}
void CPlay::SaveMp4(const QString& path,int time){
    if(m_Frame.empty()){
        qDebug()<<__FUNCTION__<<"可见光视频录制失败:m_Frame为空";
        return;
    }
    if(!m_VideoWriter.open(path.toStdString(),cv::VideoWriter::fourcc('h','2','6','4'),25,cv::Size(m_Frame.cols,m_Frame.rows))){
        qDebug()<<"可见光视频录制失败:cv::VideoWriter初始化失败";
        return;
    }
    int tempCheckTimeout = 0;
    int tempFrameTotalCount = time * 25;
    int tempFrameCurrentCount = 0;
    while(tempFrameCurrentCount < tempFrameTotalCount){
        ++tempCheckTimeout;
        if(tempCheckTimeout > tempFrameTotalCount * 10){
            qDebug()<<"可见光视频录制失败:录制超时";
            break;
        }
        if(m_Frame.empty()) continue;
        m_VideoWriter.write(m_Frame);
        ++tempFrameCurrentCount;
        QThread::msleep(40);
    }
    m_VideoWriter.release();
    qDebug()<<"可见光视频录制成功";
}
