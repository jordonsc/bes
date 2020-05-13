#include "config.h"

using namespace bes;

void Config::LoadString(std::string const& source)
{
    BES_LOG(DEBUG) << "Config loading from string";

    try {
        root = YAML::Load(source);
    } catch (YAML::BadFile&) {
        root = YAML::Node(YAML::NodeType::Map);
        throw FileNotFoundException(BES_CFG_NO_FILE_ERR_MSG, source);
    }
}

void Config::LoadFile(std::string const& source)
{
    BES_LOG(DEBUG) << "Config loading from <" << source << ">";

    try {
        root = YAML::LoadFile(source);
    } catch (YAML::BadFile&) {
        root = YAML::Node(YAML::NodeType::Map);
        throw FileNotFoundException(BES_CFG_NO_FILE_ERR_MSG, source);
    }
}

void Config::LoadFile(FileFinder const& ff)
{
    try {
        LoadFile(ff.Find());
    } catch (FileNotFoundException&) {
        root = YAML::Node(YAML::NodeType::Map);
        BES_LOG(WARNING) << "No configuration file found";
    }
}
