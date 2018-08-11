#ifndef _LIUS_TOOLS_VIDEO_SEEKER_H_
#define _LIUS_TOOLS_VIDEO_SEEKER_H_

#include <string>
#include <memory>

namespace lius_tools {

class VideoSeekerImpl;

class VideoSeeker {
 public:
  VideoSeeker(const std::string& file_path);
  ~VideoSeeker();

  double seek(double timestamp);
  uint16_t width() const;
  uint16_t height() const;
  const QImage& image() const;

  // Returns pointer to raw image buffer in RGB32 format
  const uint8* data() const;
  
 private:
  std::string& file_path_;
  std::unique_ptr<VideoSeekerImpl> impl_;
  QImage image_;
};

}

#endif /* _LIUS_TOOLS_VIDEO_SEEKER_H_ */
