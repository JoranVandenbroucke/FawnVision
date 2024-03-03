//
// Copyright (c) 2024.
// Author: Joran
//

#pragma once

namespace FawnVision
{
/**
 * \brief Retruns last error string.
 * \return Error String
 */
auto GetError() -> const char*;
/**
 * \brief Resets the error
 */
void ClearError();
} // namespace FawnVision
