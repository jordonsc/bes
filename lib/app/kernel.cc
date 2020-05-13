#include "kernel.h"

using namespace bes::app;

KernelInterface* KernelInterface::instance = nullptr;

KernelInterface::KernelInterface(const int argc, char** const argv) : argc(argc), argv(argv)
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

bes::log::LogSink& KernelInterface::LogSink() const
{
    if (log_sink == nullptr) {
        throw KernelException("Log sink has not been constructed");
    }

    return *(log_sink.get());
}

bes::cli::Parser const& KernelInterface::Cli() const
{
    return cli;
}

bes::Config const& KernelInterface::Config()
{
    return config;
}

bes::Container& KernelInterface::Container()
{
    return container;
}

KernelInterface& KernelInterface::Instance()
{
    if (instance == nullptr) {
        throw KernelException("Kernel has not been constructed");
    }

    return *instance;
}

bool KernelInterface::Exists()
{
    return instance != nullptr;
}
