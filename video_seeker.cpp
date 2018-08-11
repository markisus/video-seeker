#include "video_seeker.h"
#include "video_seeker_impl.h" // link against video_seeker_impl

namespace lius_tools {

VideoSeeker::VideoSeeker(const std::string& file_path) :
    file_path_(file_path),
    impl_(new VideoSeekerImpl(file_path)) {};

VideoSeeker::Seek(double ts) {
  impl_->Seek(ts);
}

uint16_t VideoSeker::width() const {
  return impl_->width_;
}

uint16_t VideoSeker::height() const {
  return impl_->height_;
}

const uint8_t* data() const {
  return impl_->pointers[0];
};

}
