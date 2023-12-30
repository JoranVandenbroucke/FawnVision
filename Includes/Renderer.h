//
// Created by joran on 30/12/2023.
//

#pragma once
#include "FawnVision_Core.h"

namespace FawnVision {
class Renderer {
public:
    Renderer()=default;

    error_code Initialize();
    error_code Cleanup();

    error_code Start();
    error_code NextPass();
    error_code EndDraw();
};
} // FawnVision
