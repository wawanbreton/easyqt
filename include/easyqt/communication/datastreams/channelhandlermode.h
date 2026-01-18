#pragma once

namespace ChannelHandlerMode
{
    enum Enum
    {
        SingleUse,        // Handler is finished when connection is failed or disconnected
        SinglePromotable, // Handler mode is changed when first connection is finished : if the
                          // connection is established, it is promoted to AlwaysConnected, but
                          // if the connection fails, it is retrograded to SingleUse
        AlwaysConnected,  // Handler is never finished, always tries to reconnect
    };
}
