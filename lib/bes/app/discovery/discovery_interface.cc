#include "discovery_interface.h"

using namespace bes::app::discovery;

std::shared_ptr<DiscoveryInterface> DiscoveryInterface::discovery_iface = nullptr;
std::shared_mutex DiscoveryInterface::discovery_m{};
