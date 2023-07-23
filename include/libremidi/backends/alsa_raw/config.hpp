#pragma once
#include <libremidi/config.hpp>

#include <chrono>
#include <functional>
#include <optional>
#include <span>
#include <thread>

#if __has_include(<poll.h>)
  #include <poll.h>
#else
struct pollfd
{
  int fd;
  short int events;
  short int revents;
};
#endif

namespace libremidi
{

/**
 * Used to determine how large sent messages will be chunked.
 */
struct LIBREMIDI_EXPORT chunking_parameters
{
  std::chrono::milliseconds interval{};
  int32_t size{};

  /**
   * @brief Will be called by the chunking code to allow the API user to wait.
   *
   * By default just calls sleep.
   * Arguments are: the time that must be waited, the bytes currently written.
   * Return false if you want to abort the transfer, and true otherwise.
   */
  std::function<bool(std::chrono::microseconds, int)> wait = chunking_parameters::default_wait;

  static bool default_wait(std::chrono::microseconds time_to_wait, int written_bytes)
  {
    std::this_thread::sleep_for(time_to_wait);
    return true;
  }
};

struct manual_poll_parameters
{
  std::span<pollfd> fds;
  std::function<int(std::span<pollfd> fds)> callback;
};

struct alsa_raw_input_configuration
{
  std::function<bool(const manual_poll_parameters&)> manual_poll;
};

struct alsa_raw_output_configuration
{
  /**
   * For large messages, chunk their content and wait.
   * Setting a null optional will disable chunking.
   */
  std::optional<chunking_parameters> chunking;
};

}
