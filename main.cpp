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

  // Need max timestamp accessors

  // Seek to 1.5 secs
  video_seeker.Seek(1.5);

  LOG(INFO) << "Data pointer at " << (unsigned long)(video_seeker.data());
  return 0;
}
