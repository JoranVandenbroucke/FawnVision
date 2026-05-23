#include "instance.hpp"

#if defined(BALBINO_DEBUG)
vk::Bool32 deer_vulkan::DebugMessageFunc(const vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
                                         vk::DebugUtilsMessengerCallbackDataEXT const* pCallbackData, void* /* pUserData */)
{
    std::ostringstream message;

    message << vk::to_string(messageSeverity) << ": " << vk::to_string(messageTypes) << ":\n";
    message << std::string("\t") << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
    message << std::string("\t") << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
    message << std::string("\t") << "message         = <" << pCallbackData->pMessage << ">\n";
    if (0 < pCallbackData->queueLabelCount)
    {
        message << std::string("\t") << "Queue Labels:\n";
        for (std::uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
        {
            message << std::string("\t\t") << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount)
    {
        message << std::string("\t") << "CommandBuffer Labels:\n";
        for (std::uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
        {
            message << std::string("\t\t") << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < pCallbackData->objectCount)
    {
        message << std::string("\t") << "Objects:\n";
        for (std::uint32_t i = 0; i < pCallbackData->objectCount; i++)
        {
            message << std::string("\t\t") << "Object " << i << "\n";
            message << std::string("\t\t\t") << "objectType   = " << vk::to_string(pCallbackData->pObjects[i].objectType) << "\n";
            message << std::string("\t\t\t") << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
            if (pCallbackData->pObjects[i].pObjectName)
            {
                message << std::string("\t\t\t") << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
            }
        }
    }

#    ifdef _WIN32
    MessageBox(NULL, message.str().c_str(), "Alert", MB_OK);
#    else
    std::cout << message.str() << std::endl;
#    endif

    return false;
}
#endif
