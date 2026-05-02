#pragma once

#include <string>

namespace oe {

// Writes the pixels of the framebuffer currently bound for reading to a
// 24-bit BMP file. Throws std::runtime_error if the file cannot be written.
void saveFramebuffer(const std::string& path, int width, int height);

// A framebuffer that is not on screen, with 4x multisampling. Lets the demo
// render and save an image without showing a window (for screenshots, CI).
class OffscreenTarget
{
public:
    OffscreenTarget(int width, int height);
    ~OffscreenTarget();
    OffscreenTarget(const OffscreenTarget&) = delete;
    OffscreenTarget& operator=(const OffscreenTarget&) = delete;

    void bind() const;  // draw into this target instead of the window
    void save(const std::string& path) const;

private:
    int width;
    int height;
    unsigned int multisampled = 0;
    unsigned int resolved = 0;
    unsigned int buffers[3] = {};  // multisampled colour and depth, resolved colour
};

}  // namespace oe
