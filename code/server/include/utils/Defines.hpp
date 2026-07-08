#pragma once

#include <vulkan/vk_enum_string_helper.h>

#include "fmt/color.h"
#include "fmt/core.h"

#define VK_CHECK(x)                                                            \
  do {                                                                         \
    VkResult err = x;                                                          \
    if (err) {                                                                 \
      fmt::print(fmt::fg(fmt::color::dark_red), "[VULKAN] ERROR: {}\n", string_VkResult(err));                \
      abort();                                                                 \
    }                                                                          \
  } while (0)
