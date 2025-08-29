#pragma

#include <optional>

// forware declare
class CanMessage;

class CanInterface {
 private:
 public:
  virtual int sent(CanMessage msg) = 0;
  virtual std::optional<CanMessage> received() = 0;
};
