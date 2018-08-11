#include "video_seeker.h"
#include "video_seeker_impl.h" // link against video_seeker_impl

namespace lius_tools {

VideoSeeker::VideoSeeker(const std::string& file_path) :
    file_path_(file_path),
    impl_(new VideoSeekerImpl(file_path)) {};

VideoSeeker::seek(double ts) {
  impl_->seek(ts);
}

uint16_t VideoSeker::width() const {
  return impl_->width_;
}

uint16_t VideoSeker::height() const {
  return impl_->height_;
}

const QImage& VideoSeeker::image() const {
  return image_(impl_->pointers[0], impl_->width_, impl_->height, /* format rgb32*/);
}
  
}
