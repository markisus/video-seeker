#include "video_seeker.h"
#include "logging.h"

using namespace lius_tools;

int main(int argc, char *argv[])
{
  LOG(INFO) << "This is the sandbox for testing video seeker";

  const std::string video_path = "C:/Users/marki/Videos/smash0.mp4";
  VideoSeeker video_seeker { video_path };

  LOG(INFO) << "Video width " << video_seeker.width();
  LOG(INFO) << "Video height " << video_seeker.height();
  LOG(INFO) << "Video duration " << video_seeker.duration();

  // Seek to 1.5 secs
  const double seeked_time = video_seeker.Seek(1.5);
  LOG(INFO) << "Seeked to " << seeked_time;

  LOG(INFO) << "Data pointer at " << (unsigned long)(video_seeker.data());

  // Test for memory leak
  if (false) {
    size_t BREAK_EVERY_N = 1000;
    size_t idx = 0;
    while (true) {
      VideoSeeker temp_seeker { video_path };
      temp_seeker.Seek(1.234);
      ++idx;

      if (BREAK_EVERY_N == idx) {
        idx = 0;
      }
    }
  }
  return 0;
}
