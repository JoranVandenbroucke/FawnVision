//
// Created by Joran on 20/02/2024.
//

#pragma once

namespace FawnVision
{
    /**
     * \brief Retruns last error string.
     * \return Error String
     */
    const char* GetError();
    /**
     * \brief Resets the error
     */
    void ClearError();
}// namespace FawnVision
