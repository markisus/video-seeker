#ifndef _LIUS_TOOLS_VIDEO_SEEKER_IMPL_H_
#define _LIUS_TOOLS_VIDEO_SEEKER_IMPL_H_

namespace lius_tools {

class VideoSeekerImpl {
  VideoSeekerImpl(const std::string& file_path);
  void seek(double ts);
  ~VideoSeekerImpl();

 private:
  AVPacket* packet_ = nullptr;
  AVFrame* frame_ = nullptr;

  AVFormatContext* format_context_ = nullptr;
  AVStream* stream_ = nullptr;
  uint16_t stream_index_ = -1;
  AVCodec* codec_ = nullptr;
  AVCodecContext* codec_context_ = nullptr;

  // For conversion to RGB32 
  uint8_t* pointers_[4];
  int linesizes_[4];
  SwsContext* sws_context_ = nullptr;

  uint16_t width_ = 0;
  uint16_t height_ = 0;

  double current_time_ = 0.0;
};

}

#endif /* _LIUS_TOOLS_VIDEO_SEEKER_IMPL_H_ */
