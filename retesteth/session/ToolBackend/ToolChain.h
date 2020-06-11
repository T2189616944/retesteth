#pragma once
#include <testStructures/types/RPC/ToolResponse.h>
#include <testStructures/types/ethereum.h>
#include <boost/filesystem.hpp>
#include <vector>
namespace fs = boost::filesystem;

namespace toolimpl
{
// Manage test blockchains
class ToolChain : public GCP_SPointerBase
{
public:
    ToolChain(EthereumBlockState const& _genesis, SealEngine _sealEngine, FORK const& _fork, fs::path const& _toolPath);

    EthereumBlockState const& lastBlock() const
    {
        assert(m_blocks.size() > 0);
        return m_blocks.at(m_blocks.size() - 1);
    }

    std::vector<EthereumBlockState> const& blocks() const { return m_blocks; }

    void mineBlock(EthereumBlockState const& _pendingBlock);
    void rewindToBlock(size_t _number);

private:
    ToolChain(){};
    // Execute t8ntool cmd with input _block information, and get the output block information
    // Information includes header, transactions, state
    ToolResponse mineBlockOnTool(EthereumBlockState const& _block, SealEngine _engine = SealEngine::NoReward);


    std::vector<EthereumBlockState> m_blocks;
    SealEngine m_engine;
    spFORK m_fork;
    fs::path m_toolPath;
};

typedef GCP_SPointer<ToolChain> spToolChain;

}  // namespace toolimpl