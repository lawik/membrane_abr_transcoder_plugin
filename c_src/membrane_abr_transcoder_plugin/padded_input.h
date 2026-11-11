#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/mem.h>
}

// av_parser_parse2 requires AV_INPUT_BUFFER_PADDING_SIZE zeroed bytes after
// the input: the H.264 parser reads up to a few bytes past the end of the
// buffer when a NAL unit ends inside it. The Elixir binary has no such
// padding, so the input is copied into a padded buffer before parsing.
struct PaddedInput {
  uint8_t* data = nullptr;
  size_t size = 0;

  PaddedInput(const uint8_t* payload, size_t payload_size) : size(payload_size) {
    data = static_cast<uint8_t*>(av_malloc(size + AV_INPUT_BUFFER_PADDING_SIZE));
    if (!data) {
      throw std::runtime_error("Failed to allocate padded input buffer");
    }
    memcpy(data, payload, size);
    memset(data + size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
  }

  ~PaddedInput() { av_free(data); }

  PaddedInput(const PaddedInput&) = delete;
  PaddedInput& operator=(const PaddedInput&) = delete;
};
