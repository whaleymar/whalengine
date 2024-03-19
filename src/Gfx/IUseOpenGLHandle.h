#pragma once

#include "Util/Types.h"
namespace whal {

class IUseOpenGLHandle {
public:
    u32 handle() const { return mHandle; }

protected:
    u32 mHandle;
};

}  // namespace whal
