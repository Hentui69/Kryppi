#pragma once
 
#include <string>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
 
//Netværkskonfiguration
#define PORT        8888
#define BUFFER_SIZE 1024
 
// Broadcast-adresse
#define BROADCAST_ADDR "10.0.255.255"
 
// Fil gemt af setup-mesh.sh 
#define NODE_IP_FILE "/home/pi/mesh/node_ip.txt"
 
// Read the key from a local, gitignored secrets file.
inline std::string loadCryptoKey()
{
    const char* configuredPath = std::getenv("KRYPPI_SECRETS_FILE");
    const std::string path = configuredPath != nullptr && *configuredPath != '\0'
        ? configuredPath
        : ".secrets";

    std::ifstream secretsFile(path);
    if (!secretsFile.is_open())
        throw std::runtime_error(
            "Could not open secrets file '" + path +
            "'. Copy secrets.example to .secrets and set CRYPTO_KEY.");

    std::string line;
    while (std::getline(secretsFile, line)) {
        const std::string prefix = "CRYPTO_KEY=";
        if (line.compare(0, prefix.size(), prefix) == 0) {
            const std::string key = line.substr(prefix.size());
            if (!key.empty())
                return key;
        }
    }

    throw std::runtime_error(
        "CRYPTO_KEY is missing or empty in secrets file '" + path + "'.");
}

inline const std::string& cryptoKey()
{
    static const std::string key = loadCryptoKey();
    return key;
}