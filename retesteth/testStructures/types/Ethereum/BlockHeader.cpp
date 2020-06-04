#include "BlockHeader.h"
#include "../../basetypes.h"
#include <libdevcore/Address.h>
#include <libdevcore/SHA3.h>
#include <retesteth/TestHelper.h>
#include <retesteth/testStructures/Common.h>

using namespace dev;

namespace test
{
namespace teststruct
{
BlockHeader::BlockHeader(DataObject const& _data)
{
    string const akey = _data.count("author") ? "author" : "coinbase";
    m_author = spFH20(new FH20(_data.atKey(akey)));
    m_difficulty = spVALUE(new VALUE(_data.atKey("difficulty")));
    m_extraData = spBYTES(new BYTES(_data.atKey("extraData")));
    m_gasLimit = spVALUE(new VALUE(_data.atKey("gasLimit")));
    m_gasUsed = spVALUE(new VALUE(_data.atKey("gasUsed")));
    m_hash = spFH32(new FH32(_data.atKey("hash")));
    string const bkey = _data.count("logsBloom") ? "logsBloom" : "bloom";
    m_logsBloom = spFH256(new FH256(_data.atKey(bkey)));

    if (_data.count("mixHash"))
    {
        m_mixHash = spFH32(new FH32(_data.atKey("mixHash")));
        m_nonce = spFH8(new FH8(_data.atKey("nonce")));
    }
    else
    {
        ETH_WARNING_TEST("BlockHeader `mixHash` is not defined. Using default `0x00..00` value!", 6);
        m_mixHash = spFH32(new FH32(FH32::zero()));
        m_nonce = spFH8(new FH8(FH8::zero()));
    }

    m_number = spVALUE(new VALUE(_data.atKey("number")));
    m_parentHash = spFH32(new FH32(_data.atKey("parentHash")));
    string const rkey = _data.count("receiptsRoot") ? "receiptsRoot" : "receiptTrie";
    m_receiptsRoot = spFH32(new FH32(_data.atKey(rkey)));
    string const ukey = _data.count("sha3Uncles") ? "sha3Uncles" : "uncleHash";
    m_sha3Uncles = spFH32(new FH32(_data.atKey(ukey)));
    m_stateRoot = spFH32(new FH32(_data.atKey("stateRoot")));
    m_timestamp = spVALUE(new VALUE(_data.atKey("timestamp")));
    string const tkey = _data.count("transactionsRoot") ? "transactionsRoot" : "transactionsTrie";
    m_transactionsRoot = spFH32(new FH32(_data.atKey(tkey)));

    // Allowed fields for this structure
    requireJsonFields(_data, "BlockHeader " + _data.getKey(),
        {{"bloom", {{DataType::String}, jsonField::Optional}},
         {"logsBloom", {{DataType::String}, jsonField::Optional}},
         {"coinbase", {{DataType::String}, jsonField::Optional}},
         {"author", {{DataType::String}, jsonField::Optional}},
         {"miner", {{DataType::String}, jsonField::Optional}},
         {"difficulty", {{DataType::String}, jsonField::Required}},
         {"extraData", {{DataType::String}, jsonField::Required}},
         {"gasLimit", {{DataType::String}, jsonField::Required}},
         {"gasUsed", {{DataType::String}, jsonField::Required}},
         {"hash", {{DataType::String}, jsonField::Optional}},
         {"mixHash", {{DataType::String}, jsonField::Optional}},
         {"nonce", {{DataType::String}, jsonField::Optional}},
         {"number", {{DataType::String}, jsonField::Required}},
         {"parentHash", {{DataType::String}, jsonField::Required}},
         {"receiptTrie", {{DataType::String}, jsonField::Optional}},
         {"receiptsRoot", {{DataType::String}, jsonField::Optional}},
         {"stateRoot", {{DataType::String}, jsonField::Required}},
         {"timestamp", {{DataType::String}, jsonField::Required}},
         {"transactionsTrie", {{DataType::String}, jsonField::Optional}},
         {"transactionsRoot", {{DataType::String}, jsonField::Optional}},
         {"sha3Uncles", {{DataType::String}, jsonField::Optional}},
         {"uncleHash", {{DataType::String}, jsonField::Optional}},
         {"size", {{DataType::String}, jsonField::Optional}},               // EthGetBlockBy field
         {"totalDifficulty", {{DataType::String}, jsonField::Optional}},    // EthGetBlockBy field
         {"transactions", {{DataType::Array}, jsonField::Optional}},        // EthGetBlockBy field
         {"uncles", {{DataType::Array}, jsonField::Optional}}               // EthGetBlockBy field
                      });
}

const DataObject BlockHeader::asDataObject() const
{
    DataObject out;
    out["bloom"] = m_logsBloom.getCContent().asString();
    out["coinbase"] = m_author.getCContent().asString();
    out["difficulty"] = m_difficulty.getCContent().asString();
    out["extraData"] = m_extraData.getCContent().asString();
    out["gasLimit"] = m_gasLimit.getCContent().asString();
    out["gasUsed"] = m_gasUsed.getCContent().asString();
    out["hash"] = m_hash.getCContent().asString();
    out["mixHash"] = m_mixHash.getCContent().asString();
    out["nonce"] = m_nonce.getCContent().asString();
    out["number"] = m_number.getCContent().asString();
    out["parentHash"] = m_parentHash.getCContent().asString();
    out["receiptTrie"] = m_receiptsRoot.getCContent().asString();
    out["stateRoot"] = m_stateRoot.getCContent().asString();
    out["timestamp"] = m_timestamp.getCContent().asString();
    out["transactionsTrie"] = m_transactionsRoot.getCContent().asString();
    out["uncleHash"] = m_sha3Uncles.getCContent().asString();
    return out;
}

const RLPStream BlockHeader::asRLPStream() const
{
    RLPStream header;
    header.appendList(15);

    header << h256(m_parentHash.getCContent().asString());
    header << h256(m_sha3Uncles.getCContent().asString());
    header << Address(m_author.getCContent().asString());
    header << h256(m_stateRoot.getCContent().asString());
    header << h256(m_transactionsRoot.getCContent().asString());
    header << h256(m_receiptsRoot.getCContent().asString());
    header << h2048(m_logsBloom.getCContent().asString());
    header << m_difficulty.getCContent().asU256();
    header << m_number.getCContent().asU256();
    header << m_gasLimit.getCContent().asU256();
    header << m_gasUsed.getCContent().asU256();
    header << m_timestamp.getCContent().asU256();
    header << test::sfromHex(m_extraData.getCContent().asString());
    header << h256(m_mixHash.getCContent().asString());
    header << h64(m_nonce.getCContent().asString());
    return header;
}

bool BlockHeader::operator==(BlockHeader const& _rhs) const
{
    return asDataObject() == _rhs.asDataObject();
}

void BlockHeader::recalculateHash()
{
    m_hash = spFH32(new FH32("0x" + toString(dev::sha3(asRLPStream().out()))));
}

}  // namespace teststruct
}  // namespace test
