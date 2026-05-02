#include "engine/Screenshot.h"

#include <glad/glad.h>

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace oe {

namespace {

void writeLittleEndian(std::ofstream& out, std::uint32_t value, int bytes)
{
    for (int i = 0; i < bytes; ++i) {
        out.put(static_cast<char>((value >> (8 * i)) & 0xFF));
    }
}

}  // namespace

void saveFramebuffer(const std::string& path, int width, int height)
{
    // BMP rows are padded to 4 bytes and stored bottom to top, which is
    // exactly how glReadPixels returns them with the default pack alignment.
    const int rowSize = (width * 3 + 3) & ~3;
    std::vector<unsigned char> pixels(static_cast<size_t>(rowSize) * height);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Could not write " + path);
    }
    const auto dataSize = static_cast<std::uint32_t>(pixels.size());
    out.put('B');
    out.put('M');
    writeLittleEndian(out, 54 + dataSize, 4);  // file size
    writeLittleEndian(out, 0, 4);              // reserved
    writeLittleEndian(out, 54, 4);             // offset of the pixels
    writeLittleEndian(out, 40, 4);             // size of this header
    writeLittleEndian(out, static_cast<std::uint32_t>(width), 4);
    writeLittleEndian(out, static_cast<std::uint32_t>(height), 4);
    writeLittleEndian(out, 1, 2);   // colour planes
    writeLittleEndian(out, 24, 2);  // bits per pixel
    writeLittleEndian(out, 0, 4);   // no compression
    writeLittleEndian(out, dataSize, 4);
    writeLittleEndian(out, 2835, 4);  // 72 dpi
    writeLittleEndian(out, 2835, 4);
    writeLittleEndian(out, 0, 4);
    writeLittleEndian(out, 0, 4);
    out.write(reinterpret_cast<const char*>(pixels.data()), static_cast<std::streamsize>(pixels.size()));
}

OffscreenTarget::OffscreenTarget(int width, int height) : width(width), height(height)
{
    glGenRenderbuffers(3, buffers);
    glBindRenderbuffer(GL_RENDERBUFFER, buffers[0]);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, buffers[1]);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, buffers[2]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);

    glGenFramebuffers(1, &multisampled);
    glBindFramebuffer(GL_FRAMEBUFFER, multisampled);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffers[0]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffers[1]);
    const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

    // Multisampled pixels cannot be read directly; they are resolved here first.
    glGenFramebuffers(1, &resolved);
    glBindFramebuffer(GL_FRAMEBUFFER, resolved);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffers[2]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (!complete) {
        throw std::runtime_error("Could not create an offscreen framebuffer");
    }
}

OffscreenTarget::~OffscreenTarget()
{
    glDeleteFramebuffers(1, &resolved);
    glDeleteFramebuffers(1, &multisampled);
    glDeleteRenderbuffers(3, buffers);
}

void OffscreenTarget::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, multisampled);
    glViewport(0, 0, width, height);
}

void OffscreenTarget::save(const std::string& path) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolved);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, resolved);
    saveFramebuffer(path, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace oe
