#include <cstdint>
namespace types {
enum EPacketType : uint32_t {
  NetMessageUnknown,
  NetMessageServerHello,
  NetMessageGenericText,
  NetMessageGameMessage,
  NetMessageGamePacket,
  NetMessageError,
  NetMessageTrack,
  NetMessageClientLogRequest,
  NetMessageClientLogResponse,
  NetMessageMax,
};
}