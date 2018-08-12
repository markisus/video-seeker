#include "video_seeker_impl.h"
#include "logging.h"

namespace lius_tools {

VideoSeekerImpl::VideoSeekerImpl(const std::string& file_path) {
  frame_ = av_frame_alloc();
  if (!frame_) {
    LOG(ERROR) << "Could not allocate frame";
    return;
  }

  packet_ = av_packet_alloc();
  if (!packet_) {
    LOG(ERROR) << "Could not allocate packet";
    return;
  }

  if (ret_ = avformat_open_input(&format_context_, file_path.c_str(),
                                nullptr, nullptr)) {
    LOG(ERROR) << "avformat_open_input failed with error " << ret_;
    return;
  }

  if ((ret_ = avformat_find_stream_info(format_context_, nullptr)) < 0) {
    LOG(ERROR) << "Could not find stream info with error " << ret_;
    return;
  }

  for (uint16_t i = 0; i < format_context_->nb_streams; ++i) {
    AVStream* candidate_stream = format_context_->streams[i];
    if (candidate_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      stream_ = candidate_stream;
      stream_index_ = i;
      break;
    }
  }

  if (!stream_) {
    LOG(ERROR) << "Could not locate the video stream";
    return;
  }

  codec_ = avcodec_find_decoder(stream_->codecpar->codec_id);
  if (!codec_) {
    LOG(ERROR) << "Could not find codec";
    return;
  }

  codec_context_ = avcodec_alloc_context3(codec_);
  if (!codec_context_) {
    LOG(ERROR) << "Could not allocate codec context";
    return;
  }

  if(avcodec_parameters_to_context(
         codec_context_,
         stream_->codecpar) != 0) {
    LOG(ERROR) << "Could not make codec context from parameters";
    return;
  }

  if (avcodec_open2(codec_context_, codec_, nullptr) < 0) {
    LOG(ERROR) << "Could not open codec";
    return;
  }

  width_ = stream_->codecpar->width;
  height_ = stream_->codecpar->height;

  uint8_t* pointers[4];
  int linesizes[4];
  if ((ret_ = av_image_alloc(
          pointers,
          linesizes,
          width_,
          height_,
          AV_PIX_FMT_RGB32,
          1 /* align */)) < 0) {
    LOG(ERROR) << "Could not allocate image " << ret_;
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
    LOG(ERROR) << "Could not make a sws context";
    return;
  }

  Seek(0.0);
}

void VideoSeekerImpl::Seek(double ts) {
  const uint64_t ts_int = ts * AV_TIME_BASE;
  if ((ret_ =
       av_seek_frame(
           format_context_,
           -1, // stream idx
           ts_int, AVSEEK_FLAG_BACKWARD)) < 0) {
    LOG(ERROR) << "Could not seek " << ret_;
    return;
  };

  avcodec_flush_buffers(codec_context_);

  while (av_read_frame(format_context_, packet_) == 0) {
    if (packet_->stream_index != stream_index_) {
      // Not the video stream
      av_packet_unref(packet_);
      continue;
    }

    if ((ret_ = avcodec_send_packet(codec_context_, packet_)) < 0) {
      LOG(ERROR) << "Error during decoding " << ret_;
      av_packet_unref(packet_);
      return;
    }

    while (ret_ >= 0) {
      ret_ = avcodec_receive_frame(codec_context_, frame_);
      if (ret_ == AVERROR(EAGAIN) || ret_ == AVERROR_EOF) {
        LOG(DEBUG) << "Codec wants another packet";
        break; // No more frames left in this packet
      } else if (ret_ < 0) {
        LOG(ERROR) << "Error during decoding (receiving frame) " << ret_;
        av_packet_unref(packet_);
        return;
      }

      AVRational frame_pts_rational;
      frame_pts_rational.num = frame_->pts;
      frame_pts_rational.den = 1;
      frame_pts_rational =
          av_mul_q(frame_pts_rational, stream_->time_base);
        
      current_time_ = av_q2d(frame_pts_rational);

      if (current_time_ <= ts) {
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
