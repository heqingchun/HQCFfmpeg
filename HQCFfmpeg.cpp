#include "HQCFfmpeg.h"

using namespace cv;

HQCCFfmpeg::HQCCFfmpeg(const QString& path){
    m_VideoPath = path;
}
HQCCFfmpeg::~HQCCFfmpeg(){
    if(buffer) av_free(buffer);
    if(p_SwsContext) sws_freeContext(p_SwsContext);
    if(p_AVFrameRGB) av_frame_free(&p_AVFrameRGB);
    if(p_AVFrame) av_frame_free(&p_AVFrame);
    if(p_AVCodecContext) avcodec_close(p_AVCodecContext);
    if(p_AVFormatContext) avformat_close_input(&p_AVFormatContext);
    if(p_AVFormatContext) avformat_free_context(p_AVFormatContext);
    if(p_AVPacket) av_packet_free(&p_AVPacket);
}
void HQCCFfmpeg::Init(){
    m_AVPixelFormat = AV_PIX_FMT_BGR24;
    QString UrlType = m_VideoPath.mid(0,4);
    p_AVFormatContext = avformat_alloc_context();
    p_AVPacket = av_packet_alloc();
    p_AVFrame = av_frame_alloc();
    p_AVFrameRGB = av_frame_alloc();
    if(!p_AVFormatContext || !p_AVPacket || !p_AVFrame || !p_AVFrameRGB){
        qDebug()<< "Init alloc Failed.";
    }
    av_dict_set(&p_AVDictionary,"buffer_size","10240000",0); //设置缓存大小，1080p可将值调大
    av_dict_set(&p_AVDictionary,"rtsp_transport","tcp",0); //以udp方式打开，如果以tcp方式打开将udp替换为tcp
    av_dict_set(&p_AVDictionary,"stimeout","20000000",0); //设置超时断开连接时间，单位微秒
    av_dict_set(&p_AVDictionary,"max_delay","30000000",0);
    if(avformat_open_input(&p_AVFormatContext,m_VideoPath.toStdString().data(),nullptr,nullptr)){
      qDebug()<< "Failed to open the video file.";
      QTimer::singleShot(1000 * 10,this,&HQCCFfmpeg::Init);
      return;
    }
    if(avformat_find_stream_info(p_AVFormatContext,nullptr) < 0){
      qDebug()<<"Failed to find the stream information.";
      QTimer::singleShot(1000 * 10,this,&HQCCFfmpeg::Init);
      return;
    }
    av_dump_format(p_AVFormatContext,0,m_VideoPath.toStdString().data(),0);
    for(unsigned int i = 0; i < p_AVFormatContext->nb_streams;++i){
      p_AVCodecContext = p_AVFormatContext->streams[i]->codec;
      if(p_AVCodecContext->codec_type == AVMEDIA_TYPE_VIDEO){
        videoStreamIndex = i;
        break;
      }
    }
    if(videoStreamIndex == -1 || !p_AVCodecContext) {
      qDebug()<< "Failed to find a video stream.";
      QTimer::singleShot(1000 * 10,this,&HQCCFfmpeg::Init);
      return;
    }
    p_AVCodec = avcodec_find_decoder(p_AVCodecContext->codec_id);
    if(!p_AVCodec) {
      qDebug()<< "Failed to find the p_AVCodec.";
      QTimer::singleShot(1000 * 10,this,&HQCCFfmpeg::Init);
      return;
    }
    if(UrlType == "rtsp"){
        if(avcodec_open2(p_AVCodecContext,p_AVCodec,&p_AVDictionary) < 0){
          qDebug()<< "Failed to open the p_AVCodec.";
          QTimer::singleShot(1000 * 10,this,&HQCCFfmpeg::Init);
          return;
        }
    }else{
        if(avcodec_open2(p_AVCodecContext,p_AVCodec,nullptr) < 0){
          qDebug()<< "Failed to open the p_AVCodec.";
          QTimer::singleShot(1000 * 10,this,&HQCCFfmpeg::Init);
          return;
        }
    }
    qDebug()<<"Video信息: W x H = "<< p_AVCodecContext->width << " x " << p_AVCodecContext->height<<p_AVCodecContext->pix_fmt;
    int numBytes = av_image_get_buffer_size(m_AVPixelFormat,p_AVCodecContext->width,p_AVCodecContext->height,1);
    buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(p_AVFrameRGB->data,p_AVFrameRGB->linesize,buffer,m_AVPixelFormat,p_AVCodecContext->width,p_AVCodecContext->height,1);
    p_SwsContext = sws_getContext(p_AVCodecContext->width,p_AVCodecContext->height,p_AVCodecContext->pix_fmt,p_AVCodecContext->width,
                                p_AVCodecContext->height,m_AVPixelFormat,SWS_BILINEAR,nullptr,nullptr,nullptr);
    m_InitSuccess = true;
}
void HQCCFfmpeg::Work(){
    if(!m_InitSuccess) return;
    while(av_read_frame(p_AVFormatContext,p_AVPacket) >= 0){
        if(p_AVPacket->stream_index == videoStreamIndex){
            if(avcodec_send_packet(p_AVCodecContext,p_AVPacket)){
                qDebug() <<"Failed to avcodec_send_packet(pAVCodecContext, pAVPacket)";
                break;
            }
            int response = avcodec_receive_frame(p_AVCodecContext,p_AVFrame);
            if(response == AVERROR(EAGAIN) || response == AVERROR_EOF){
                continue;
            }
            else if (response < 0){
                qDebug()<<"解码一帧出错.";
                return;
            }
            sws_scale(p_SwsContext,p_AVFrame->data,p_AVFrame->linesize,0, p_AVCodecContext->height,p_AVFrameRGB->data,p_AVFrameRGB->linesize);
            // 在这里你可以使用 frameRGB 数据进行视频的显示或处理
            //qDebug()<<"可见光解析出一帧并发送信号";
            emit signalReceiveFrame(Mat(p_AVCodecContext->height,p_AVCodecContext->width,CV_8UC3,p_AVFrameRGB->data[0]));
            av_frame_unref(p_AVFrame);
        }
        av_packet_unref(p_AVPacket);
    }
}
