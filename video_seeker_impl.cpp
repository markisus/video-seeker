#include "video_seeker_impl.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace lius_tools {

VideoSeekerImpl::VideoSeekerImpl(const std::string& file_path) {
  frame_ = av_frame_alloc();
  if (!frame) {
    qDebug() << "Could not allocate frame";
    return;
  }

  packet_ = av_packet_alloc();
  if (!packet) {
    qDebug() << "Could not allocate packet";
    return;
  }

  int ret = 0;
  if (ret = avformat_open_input(&format_context_, file_path.c_str(),
                                nullptr, nullptr)) {
    qDebug() << "avformat_open_input failed with error " << ret;
    return;
  }

  if ((ret = avformat_find_stream_info(format_context_, nullptr)) < 0) {
    qDebug() << "Could not find stream info with error " << ret;
    return;
  }

  for (uint16_t i = 0; i < format_context->nb_streams; ++i) {
    AVStream* candidate_stream = format_context->streams[i];
    if (candidate_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      stream_ = candidate_stream;
      stream_index_ = i;
      break;
    }
  }

  if (!stream_) {
    qDebug() << "Could not locate the video stream";
    return;
  }

  codec_ = avcodec_find_decoder(stream_->codecpar->codec_id);
  if (!codec_) {
    qDebug() << "Could not find codec";
    return;
  }

  codec_context_ = avcodec_alloc_context3(codec_);
  if (!codec_context_) {
    qDebug() << "Could not allocate codec context";
    return;
  }

  if(avcodec_parameters_to_context(
         codec_context_,
         stream_->codecpar) != 0) {
    qDebug() << "Could not make codec context from parameters";
    return;
  }

  if (avcodec_open2(codec_context_, codec_, nullptr) < 0) {
    qDebug() << "Could not open codec";
    return;
  }

  width_ = stream_->codecpar->width;
  height_ = stream_->codecpar->height;

  uint8_t* pointers[4];
  int linesizes[4];
  if ((ret = av_image_alloc(
          pointers,
          linesizes,
          width,
          height,
          AV_PIX_FMT_RGB32,
          1 /* align */)) < 0) {
    qDebug() << "Could not allocate image " << ret;
    return;
  }

  sws_context_ =
      sws_getContext(width_, /*src width */
                     height_, /* src height */
                     (AVPixelFormat)stream_->codecpar->format, /* src format */
                     width_, /* dest width */
                     height_, /* dest height */
                     AV_PIX_FMT_RGB32, /* dst format */
                     SWS_BILINEAR, /* flags */
                     nullptr, /* src filter */
                     nullptr, /* dst filter */
                     nullptr /* param */);

  if (!sws_context_) {
    qDebug() << "Could not make a sws context";
    return;
  }

  seek(0.0);
}

void VideoSeekerImpl::seek(double ts) {
  const uint64_t ts_int = ts * AV_TIME_BASE;
  if ((ret =
       av_seek_frame(
           format_context,
           -1, // stream idx
           ts_int, AVSEEK_FLAG_BACKWARD)) < 0) {
    qDebug() << "Could not seek " << ret;
    return;
  };

  av_flush_buffers(codec_context_);

  while (av_read_frame(format_context_, packet_) == 0) {
    if (packet_->stream_index != stream_index_) {
      // Not the video stream
      av_packet_unref(packet);
      continue;
    }

    if ((ret = avcodec_send_packet(codec_context, packet)) < 0) {
      qDebug() << "Error during decoding " << ret;
      av_packet_unref(packet);
      return;
    }

    while (ret >= 0) {
      ret = avcodec_receive_frame(codec_context_, frame_);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        qDebug() << "Codec wants another packet";
        break; // No more frames left in this packet
      } else if (ret < 0) {
        qDebug() << "Error during decoding (receiving frame) " << ret;
        av_packet_unref(packet_);
        return;
      }

      AVRational frame_pts_rational;
      frame_pts_rational.num = frame_->pts;
      frame_pts_rational.den = 1;
      frame_pts_rational =
          av_mul_q(frame_pts_rational, stream_->time_base);
        
      const double frame_pts_double = av_q2d(frame_pts_rational);
      if (frame_pts_double <= ts) {
        sws_scale(
            sws_context_,
            frame_->data,
            frame_->linesize,
            0, /* srcSliceY */
            codec_context_->height, /* srcSliceH */
            pointers_, /* dst[] */
            linesizes_ /* dstStride[] */);
      } else {
        av_packet_unref(packet_);
        return;
      }
    }

    av_packet_unref(packet_);
  }
}

VideoSeekerImpl::~VideoSeekerImpl() {
    // todo
    // free(packet_);
    // free(frame_);
    // free(format_context_)
    // free(codec_);
    // free(codec_context_);
    // free(pointers_[0])
    // free(sws_context_);
}

}
