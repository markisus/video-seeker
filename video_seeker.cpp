#include "video_seeker.h"
#include "video_seeker_impl.h" // link against video_seeker_impl

namespace lius_tools {

VideoSeeker::VideoSeeker(const std::string& file_path) :
    file_path_(file_path),
    impl_(new VideoSeekerImpl(file_path)) {}

VideoSeeker::~VideoSeeker() = default;

double VideoSeeker::Seek(double ts) {
  impl_->Seek(ts);
  // todo: return actual seek time
  return ts;
}

const std::string & VideoSeeker::file_path() const
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
