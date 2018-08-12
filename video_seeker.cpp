#include "video_seeker.h"
#include "video_seeker_impl.h" // link against video_seeker_impl
#include "logging.h"

namespace lius_tools {

VideoSeeker::VideoSeeker(const std::string& file_path) :
    file_path_(file_path),
    impl_(new VideoSeekerImpl(file_path)) {
  if (impl_->ret_ < 0) {
    LOG(ERROR) << "Failed to initialize video seeker";
    exit(-1);
  }
}

VideoSeeker::~VideoSeeker() = default;

double VideoSeeker::Seek(double ts) {
  impl_->Seek(ts);
  return impl_->current_time_;
}

double VideoSeeker::duration() const {
  return impl_->duration_;
}

const std::string& VideoSeeker::file_path() const
{
  return file_path_;
}

uint16_t VideoSeeker::width() const {
  return impl_->width_;
}

uint16_t VideoSeeker::height() const {
  return impl_->height_;
}

const uint8_t* VideoSeeker::data() const {
  return impl_->pointers_[0];
};

}
