#include "kernel.h"

using namespace bes::app;

KernelInterface* KernelInterface::instance = nullptr;

KernelInterface::KernelInterface(int argc, char** argv) : argc(argc), argv(argv)
{
    // This interface should be a singleton, check it doesn't already exist and then bind the static variable
    if (instance != nullptr) {
        throw KernelException("Only a single instance of the kernel can exist.");
    }

    instance = this;
}

KernelInterface::~KernelInterface()
{
    // Singleton no longer lives
    instance = nullptr;
}

bes::log::LogSink& KernelInterface::getLogSink() const
{
    if (log_sink == nullptr) {
        throw KernelException("Log sink has not been constructed");
    }

    return *log_sink;
}

bes::cli::Parser const& KernelInterface::getCli() const
{
    return cli_parser;
}

bes::Config const& KernelInterface::getConfig()
{
    return config;
}

bes::Container& KernelInterface::getContainer()
{
    return container;
}

KernelInterface& KernelInterface::getInstance()
{
    if (instance == nullptr) {
        throw KernelException("Kernel has not been constructed");
    }

    return *instance;
}

bool KernelInterface::exists()
{
    return instance != nullptr;
}
