// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_MAIN_H
#define BITCOIN_MAIN_H

#include "bignum.h"
#include "sync.h"
#include "net.h"
#include "script.h"
#include "scrypt.h"
#include "hashgroestl.h"
#include "hashblake.h"
#include "hashx17.h"
#include "Lyra2RE.h"
#include <list>
#include "ui_interface.h"
#include "primitives/transaction.h"

class CWallet;
class CBlock;
class CBlockIndex;
class CKeyItem;
class CReserveKey;
class COutPoint;

class CAddress;
class CInv;
class CRequestTracker;
class CNode;

static const int SAME_ALGO_MAX_COUNT = 6;
static const unsigned int MAX_BLOCK_SIZE = 1000000;
static const unsigned int MAX_BLOCK_SIZE_GEN = MAX_BLOCK_SIZE/2;
static const unsigned int MAX_BLOCK_SIGOPS = MAX_BLOCK_SIZE/50;
static const unsigned int MAX_ORPHAN_TRANSACTIONS = MAX_BLOCK_SIZE/100;
static const unsigned int MAX_INV_SZ = 50000;
static const int64 HALF_HELMING_BLOCKS = 1048320; // Number of block which will be mined in one year, two, three and etc.
static const int64 LAST_BLOCK_WITH_REWARDS = 6361141;
static const int DISABLE_POS_BLOCK = 1;

// Threshold for nLockTime: below this value it is interpreted as block number, otherwise as UNIX timestamp.
static const unsigned int LOCKTIME_THRESHOLD = 500000000; // Tue Nov  5 00:53:20 1985 UTC

#ifdef USE_UPNP
static const int fHaveUPnP = true;
#else
static const int fHaveUPnP = false;
#endif

static const uint256 hashGenesisBlockOfficial("0x000003cf5b8e89511781d6189d6b6896e8ca48b10a6163a9c2204643fe377f56");
//static const uint256 hashGenesisBlockOfficial("0x00000fc63692467faeb20cdb3b53200dc601d75bdfa1001463304cc790d77278");
static const uint256 hashGenesisBlockTestNet("0x65b4e101cacf3e1e4f3a9237e3a74ffd1186e595d8b78fa8ea22c21ef5bf9347");

static const int64 nMaxClockDrift = 2 * 60 * 60;        // two hours

extern CScript COINBASE_FLAGS;


extern CCriticalSection cs_main;
extern std::map<uint256, CBlockIndex*> mapBlockIndex;
extern std::set<std::pair<COutPoint, unsigned int> > setStakeSeen;
extern uint256 hashGenesisBlock;
extern CBlockIndex* pindexGenesisBlock;
extern unsigned int nStakeMinAge;
extern int nCoinbaseMaturity;
extern int nBestHeight;
extern CBigNum bnBestChainTrust;
extern CBigNum bnBestInvalidTrust;
extern uint256 hashBestChain;
extern CBlockIndex* pindexBest;
extern unsigned int nTransactionsUpdated;
extern uint64 nLastBlockTx;
extern uint64 nLastBlockSize;
extern int64 nLastCoinStakeSearchInterval;
extern const std::string strMessageMagic;
extern double dHashesPerSec;
extern int64 nHPSTimerStart;
extern int64 nTimeBestReceived;
extern CCriticalSection cs_setpwalletRegistered;
extern std::set<CWallet*> setpwalletRegistered;
extern unsigned char pchMessageStart[4];
extern std::map<uint256, CBlock*> mapOrphanBlocks;

// Settings
extern int64 nTransactionFee;

extern int miningAlgo;

// Minimum disk space required - used in CheckDiskSpace()
static const uint64 nMinDiskSpace = 52428800;


class CReserveKey;
class CTxDB;
class CTxIndex;

void RegisterWallet(CWallet* pwalletIn);
void UnregisterWallet(CWallet* pwalletIn);
void SyncWithWallets(const CTransaction& tx, const CBlock* pblock = NULL, bool fUpdate = false, bool fConnect = true);
bool ProcessBlock(CNode* pfrom, CBlock* pblock);
bool CheckDiskSpace(uint64 nAdditionalBytes=0);
FILE* OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode="rb");
FILE* AppendBlockFile(unsigned int& nFileRet);
bool LoadBlockIndex(bool fAllowNew=true, CClientUIInterface* uiInterface=NULL);
void PrintBlockTree();
CBlockIndex* FindBlockByHeight(int nHeight);
bool ProcessMessages(CNode* pfrom);
bool SendMessages(CNode* pto, bool fSendTrickle);
bool LoadExternalBlockFile(FILE* fileIn);
void GenerateBitcoins(bool fGenerate, CWallet* pwallet);
CBlock* CreateNewBlock(CWallet* pwallet, bool fProofOfStake, int algo);
void IncrementExtraNonce(CBlock* pblock, CBlockIndex* pindexPrev, unsigned int& nExtraNonce);
void FormatHashBuffers(CBlock* pblock, char* pmidstate, char* pdata, char* phash1);
bool CheckWork(CBlock* pblock, CWallet& wallet, CReserveKey& reservekey);
bool CheckProofOfWork(uint256 hash, unsigned int nBits, int algo);
int64 GetProofOfWorkReward(int nHeight, int64 nFees);
int64 GetProofOfStakeReward(int64 nCoinAge, unsigned int nBits, unsigned int nTime);
unsigned int ComputeMinWork(unsigned int nBase, int64 nTime);
int GetNumBlocksOfPeers();
bool IsInitialBlockDownload();
std::string GetWarnings(std::string strFor);
bool GetTransaction(const uint256 &hash, CTransaction &tx, uint256 &hashBlock);
uint256 WantedByOrphan(const CBlock* pblockOrphan);
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake);
void CrypticCoinMiner(CWallet *pwallet, bool fProofOfStake);
void ResendWalletTransactions();

enum
{
    ALGO_SCRYPT  = 0,
    ALGO_X17	 = 1,
    ALGO_LYRA2RE = 2,
    ALGO_BLAKE   = 3,
    ALGO_GROESTL = 4,
    NUM_ALGOS
};

extern CBigNum bnProofOfWorkLimit[NUM_ALGOS];

enum
{
    // primary version
	BLOCK_VERSION_STEALTH     = 4,

    // algo
    BLOCK_VERSION_ALGO_BROKEN = (10 << 11),
    BLOCK_VERSION_ALGO        = (15 << 11),
    BLOCK_VERSION_SCRYPT      = (1  << 11),
    BLOCK_VERSION_GROESTL     = (2  << 11),
    BLOCK_VERSION_X17		  = (3  << 11),
    BLOCK_VERSION_BLAKE 	  = (4  << 11),
    BLOCK_VERSION_LYRA2RE	  = (10 << 11),
};

inline int GetAlgo(int nVersion)
{
    switch (nVersion & BLOCK_VERSION_ALGO)
    {
        case BLOCK_VERSION_SCRYPT:
            return ALGO_SCRYPT;
        case BLOCK_VERSION_GROESTL:
            return ALGO_GROESTL;
        case BLOCK_VERSION_LYRA2RE:
            return ALGO_LYRA2RE;
        case BLOCK_VERSION_X17:
            return ALGO_X17;
        case BLOCK_VERSION_BLAKE:
            return ALGO_BLAKE;
    }
    return ALGO_SCRYPT;
}

inline std::string GetAlgoName(int Algo)
{
    switch (Algo)
    {
        case ALGO_SCRYPT:
            return std::string("scrypt");
        case ALGO_GROESTL:
            return std::string("groestl");
        case ALGO_LYRA2RE:
            return std::string("lyra2re");
	case ALGO_BLAKE:
	    return std::string("blake");
	case ALGO_X17:
	    return std::string("x17");

    }
    return std::string("unknown");
}





bool GetWalletFile(CWallet* pwallet, std::string &strWalletFileOut);

/** Position on disk for a particular transaction. */
class CDiskTxPos
{
public:
    unsigned int nFile;
    unsigned int nBlockPos;
    unsigned int nTxPos;

    CDiskTxPos()
    {
        SetNull();
    }

    CDiskTxPos(unsigned int nFileIn, unsigned int nBlockPosIn, unsigned int nTxPosIn)
    {
        nFile = nFileIn;
        nBlockPos = nBlockPosIn;
        nTxPos = nTxPosIn;
    }

    IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
    void SetNull() { nFile = (unsigned int) -1; nBlockPos = 0; nTxPos = 0; }
    bool IsNull() const { return (nFile == (unsigned int) -1); }

    friend bool operator==(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return (a.nFile     == b.nFile &&
                a.nBlockPos == b.nBlockPos &&
                a.nTxPos    == b.nTxPos);
    }

    friend bool operator!=(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return !(a == b);
    }


    std::string ToString() const
    {
        if (IsNull())
            return "null";
        else
            return strprintf("(nFile=%u, nBlockPos=%u, nTxPos=%u)", nFile, nBlockPos, nTxPos);
    }

    void print() const
    {
        printf("%s", ToString().c_str());
    }
};

/** A transaction with a merkle branch linking it to the block chain. */
class CMerkleTx : public CTransaction
{
public:
    uint256 hashBlock;
    std::vector<uint256> vMerkleBranch;
    int nIndex;

    // memory only
    mutable bool fMerkleVerified;


    CMerkleTx()
    {
        Init();
    }

    CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
    {
        Init();
    }

    void Init()
    {
        hashBlock = 0;
        nIndex = -1;
        fMerkleVerified = false;
    }


    IMPLEMENT_SERIALIZE
    (
        nSerSize += SerReadWrite(s, *(CTransaction*)this, nType, nVersion, ser_action);
        nVersion = this->nVersion;
        READWRITE(hashBlock);
        READWRITE(vMerkleBranch);
        READWRITE(nIndex);
    )


    int SetMerkleBranch(const CBlock* pblock=NULL);
    int GetDepthInMainChain(CBlockIndex* &pindexRet) const;
    int GetDepthInMainChain() const { CBlockIndex *pindexRet; return GetDepthInMainChain(pindexRet); }
    bool IsInMainChain() const { return GetDepthInMainChain() > 0; }
    int GetBlocksToMaturity() const;
    bool AcceptToMemoryPool(CTxDB& txdb, bool fCheckInputs=true);
    bool AcceptToMemoryPool();
};




/**  A txdb record that contains the disk location of a transaction and the
 * locations of transactions that spend its outputs.  vSpent is really only
 * used as a flag, but having the location is very helpful for debugging.
 */
class CTxIndex
{
public:
    CDiskTxPos pos;
    std::vector<CDiskTxPos> vSpent;

    CTxIndex()
    {
        SetNull();
    }

    CTxIndex(const CDiskTxPos& posIn, unsigned int nOutputs)
    {
        pos = posIn;
        vSpent.resize(nOutputs);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(pos);
        READWRITE(vSpent);
    )

    void SetNull()
    {
        pos.SetNull();
        vSpent.clear();
    }

    bool IsNull()
    {
        return pos.IsNull();
    }

    friend bool operator==(const CTxIndex& a, const CTxIndex& b)
    {
        return (a.pos    == b.pos &&
                a.vSpent == b.vSpent);
    }

    friend bool operator!=(const CTxIndex& a, const CTxIndex& b)
    {
        return !(a == b);
    }
    int GetDepthInMainChain() const;

};





/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 *
 * Blocks are appended to blk0001.dat files on disk.  Their location on disk
 * is indexed by CBlockIndex objects in memory.
 */
class CBlockHeader
{
public:
    // header
    int nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    unsigned int nTime;
    unsigned int nBits;
    unsigned int nNonce;

    CBlockHeader()
    {
        SetNull();
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
    )

    void SetNull()
    {
        nVersion = BLOCK_VERSION_STEALTH;
        hashPrevBlock = 0;
        hashMerkleRoot = 0;
        nTime = 0;
        nBits = 0;
        nNonce = 0;
    }

    bool IsNull() const
    {
        return (nBits == 0);
    }

    uint256 GetHash() const
    {
	return GetScryptHash();
    }
    uint256 GetScryptHash() const
    {
        uint256 thash;
        scrypt_1024_1_1_256(BEGIN(nVersion), BEGIN(thash));
        return thash;
    }
    uint256 GetLyraHash() const
    {
	uint256 thash;
	lyra2re2_hash(BEGIN(nVersion), BEGIN(thash));
	return thash;
    }
    uint256 GetPoWHash(int algo) const
    {
        switch (algo)
        {
            case ALGO_SCRYPT:
                return GetScryptHash();
            case ALGO_GROESTL:
                return HashGroestl(BEGIN(nVersion), END(nNonce));
            case ALGO_BLAKE:
                return HashBlake(BEGIN(nVersion), END(nNonce));
            case ALGO_X17:
                return HashX17(BEGIN(nVersion), END(nNonce));
	    case ALGO_LYRA2RE:
		return GetLyraHash();
        }
        return GetScryptHash();
    }

    int64 GetBlockTime() const
    {
        return (int64)nTime;
    }

    void UpdateTime(const CBlockIndex* pindexPrev);
};

class CBlock : public CBlockHeader
{
public:
    // network and disk
    std::vector<CTransaction> vtx;

    // ppcoin: block signature - signed by one of the coin base txout[N]'s owner
    std::vector<unsigned char> vchBlockSig;

    // memory only
    mutable std::vector<uint256> vMerkleTree;

    // Denial-of-service detection:
    mutable int nDoS;
    bool DoS(int nDoSIn, bool fIn) const { nDoS += nDoSIn; return fIn; }

    CBlock()
    {
        SetNull();
    }

    int GetAlgo() const { return ::GetAlgo(nVersion); }
    bool CheckPrevAlgo(CBlockIndex* pIndex);

    IMPLEMENT_SERIALIZE
    (
        READWRITE(*(CBlockHeader*)this);

        // ConnectBlock depends on vtx following header to generate CDiskTxPos
        if (!(nType & (SER_GETHASH|SER_BLOCKHEADERONLY)))
        {
            READWRITE(vtx);
            READWRITE(vchBlockSig);
        }
        else if (fRead)
        {
            const_cast<CBlock*>(this)->vtx.clear();
            const_cast<CBlock*>(this)->vchBlockSig.clear();
        }
    )

    void SetNull()
    {
    	CBlockHeader::SetNull();
    	vtx.clear();
        vchBlockSig.clear();
        vMerkleTree.clear();
        nDoS = 0;
    }

    bool IsNull() const
    {
        return (nBits == 0);
    }

    int64 GetBlockTime() const
    {
        return (int64)nTime;
    }

    void UpdateTime(const CBlockIndex* pindexPrev);

    // ppcoin: entropy bit for stake modifier if chosen by modifier
    unsigned int GetStakeEntropyBit(unsigned int nHeight) const
    {
        // Protocol switch to support p2pool at CrypticCoin block #1
        if (nHeight >= 1 || fTestNet)
        {
            // Take last bit of block hash as entropy bit
            unsigned int nEntropyBit = ((GetHash().Get64()) & 1llu);
            if (fDebug && GetBoolArg("-printstakemodifier"))
                printf("GetStakeEntropyBit: nHeight=%u hashBlock=%s nEntropyBit=%u\n", nHeight, GetHash().ToString().c_str(), nEntropyBit);
            return nEntropyBit;
        }
        // Before CrypticCoin block #1 - old protocol
        uint160 hashSig = Hash160(vchBlockSig);
        if (fDebug && GetBoolArg("-printstakemodifier"))
            printf("GetStakeEntropyBit: hashSig=%s", hashSig.ToString().c_str());
        hashSig >>= 159; // take the first bit of the hash
        if (fDebug && GetBoolArg("-printstakemodifier"))
            printf(" entropybit=%" PRI64d"\n", hashSig.Get64());
        return hashSig.Get64();
    }

    // ppcoin: two types of block: proof-of-work or proof-of-stake
    bool IsProofOfStake() const
    {
        return (vtx.size() > 1 && vtx[1].IsCoinStake());
    }

    bool IsProofOfWork() const
    {
        return !IsProofOfStake();
    }

    std::pair<COutPoint, unsigned int> GetProofOfStake() const
    {
        return IsProofOfStake()? std::make_pair(vtx[1].vin[0].prevout, vtx[1].nTime) : std::make_pair(COutPoint(), (unsigned int)0);
    }

    // ppcoin: get max transaction timestamp
    int64 GetMaxTransactionTime() const
    {
        int64 maxTransactionTime = 0;
        BOOST_FOREACH(const CTransaction& tx, vtx)
            maxTransactionTime = std::max(maxTransactionTime, (int64)tx.nTime);
        return maxTransactionTime;
    }

	CBlockHeader GetBlockHeader() const
    {
         CBlockHeader block;
        block.nVersion       = nVersion;
        block.hashPrevBlock  = hashPrevBlock;
        block.hashMerkleRoot = hashMerkleRoot;
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        return block;
    }

    uint256 BuildMerkleTree() const
    {
        vMerkleTree.clear();
        BOOST_FOREACH(const CTransaction& tx, vtx)
            vMerkleTree.push_back(tx.GetHash());
        int j = 0;
        for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
        {
            for (int i = 0; i < nSize; i += 2)
            {
                int i2 = std::min(i+1, nSize-1);
                vMerkleTree.push_back(Hash(BEGIN(vMerkleTree[j+i]),  END(vMerkleTree[j+i]),
                                           BEGIN(vMerkleTree[j+i2]), END(vMerkleTree[j+i2])));
            }
            j += nSize;
        }
        return (vMerkleTree.empty() ? 0 : vMerkleTree.back());
    }

    std::vector<uint256> GetMerkleBranch(int nIndex) const
    {
        if (vMerkleTree.empty())
            BuildMerkleTree();
        std::vector<uint256> vMerkleBranch;
        int j = 0;
        for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
        {
            int i = std::min(nIndex^1, nSize-1);
            vMerkleBranch.push_back(vMerkleTree[j+i]);
            nIndex >>= 1;
            j += nSize;
        }
        return vMerkleBranch;
    }

    static uint256 CheckMerkleBranch(uint256 hash, const std::vector<uint256>& vMerkleBranch, int nIndex)
    {
        if (nIndex == -1)
            return 0;
        BOOST_FOREACH(const uint256& otherside, vMerkleBranch)
        {
            if (nIndex & 1)
                hash = Hash(BEGIN(otherside), END(otherside), BEGIN(hash), END(hash));
            else
                hash = Hash(BEGIN(hash), END(hash), BEGIN(otherside), END(otherside));
            nIndex >>= 1;
        }
        return hash;
    }


    bool WriteToDisk(unsigned int& nFileRet, unsigned int& nBlockPosRet)
    {
        // Open history file to append
        CAutoFile fileout = CAutoFile(AppendBlockFile(nFileRet), SER_DISK, CLIENT_VERSION);
        if (!fileout)
            return error("CBlock::WriteToDisk() : AppendBlockFile failed");

        // Write index header
        unsigned int nSize = fileout.GetSerializeSize(*this);
        fileout << FLATDATA(pchMessageStart) << nSize;

        // Write block
        long fileOutPos = ftell(fileout);
        if (fileOutPos < 0)
            return error("CBlock::WriteToDisk() : ftell failed");
        nBlockPosRet = fileOutPos;
        fileout << *this;

        // Flush stdio buffers and commit to disk before returning
        fflush(fileout);
        if (!IsInitialBlockDownload() || (nBestHeight+1) % 500 == 0)
            FileCommit(fileout);

        return true;
    }

    bool ReadFromDisk(unsigned int nFile, unsigned int nBlockPos, bool fReadTransactions=true)
    {
        SetNull();

        // Open history file to read
        CAutoFile filein = CAutoFile(OpenBlockFile(nFile, nBlockPos, "rb"), SER_DISK, CLIENT_VERSION);
        if (!filein)
            return error("CBlock::ReadFromDisk() : OpenBlockFile failed");
        if (!fReadTransactions)
            filein.nType |= SER_BLOCKHEADERONLY;

        // Read block
        try {
            filein >> *this;
        }
        catch (std::exception &e) {
            return error("%s() : deserialize or I/O error", __PRETTY_FUNCTION__);
        }

        // Check the header
        if (fReadTransactions && IsProofOfWork() && !CheckProofOfWork(GetPoWHash(GetAlgo()), nBits, GetAlgo()))
            return error("CBlock::ReadFromDisk() : errors in block header");

        return true;
    }

    void print() const
    {
        printf("CBlock(hash=%s, ver=%d, algo=%d, mined_hash=%s, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%" PRIszu", vchBlockSig=%s)\n",
            GetHash().ToString().c_str(),
            nVersion, GetAlgo(),
            GetPoWHash(GetAlgo()).ToString().c_str(),
            hashPrevBlock.ToString().c_str(),
            hashMerkleRoot.ToString().c_str(),
            nTime, nBits, nNonce,
            vtx.size(),
            HexStr(vchBlockSig.begin(), vchBlockSig.end()).c_str());
        for (unsigned int i = 0; i < vtx.size(); i++)
        {
            printf("  ");
            vtx[i].print();
        }
        printf("  vMerkleTree: ");
        for (unsigned int i = 0; i < vMerkleTree.size(); i++)
            printf("%s ", vMerkleTree[i].ToString().substr(0,10).c_str());
        printf("\n");
    }


    bool DisconnectBlock(CTxDB& txdb, CBlockIndex* pindex);
    bool ConnectBlock(CTxDB& txdb, CBlockIndex* pindex, bool fJustCheck=false);
    bool ReadFromDisk(const CBlockIndex* pindex, bool fReadTransactions=true);
    bool SetBestChain(CTxDB& txdb, CBlockIndex* pindexNew);
    bool AddToBlockIndex(unsigned int nFile, unsigned int nBlockPos);
    bool CheckBlock(bool fCheckPOW=true, bool fCheckMerkleRoot=true, bool fCheckSig=true) const;
    bool AcceptBlock();
    bool GetCoinAge(uint64& nCoinAge) const; // ppcoin: calculate total coin age spent in block
    bool SignBlock(const CKeyStore& keystore);
    bool CheckBlockSignature() const;

private:
    bool SetBestChainInner(CTxDB& txdb, CBlockIndex *pindexNew);
};






/** The block chain is a tree shaped structure starting with the
 * genesis block at the root, with each block potentially having multiple
 * candidates to be the next block.  pprev and pnext link a path through the
 * main/longest chain.  A blockindex may have multiple pprev pointing back
 * to it, but pnext will only point forward to the longest branch, or will
 * be null if the block is not part of the longest chain.
 */
class CBlockIndex
{
public:
    const uint256* phashBlock;
    CBlockIndex* pprev;
    CBlockIndex* pnext;
    unsigned int nFile;
    unsigned int nBlockPos;
    CBigNum bnChainTrust; // ppcoin: trust score of block chain
    int nHeight;

    int64 nMint;
    int64 nMoneySupply;

    unsigned int nFlags;  // ppcoin: block index flags
    enum
    {
        BLOCK_PROOF_OF_STAKE = (1 << 0), // is proof-of-stake block
        BLOCK_STAKE_ENTROPY  = (1 << 1), // entropy bit for stake modifier
        BLOCK_STAKE_MODIFIER = (1 << 2), // regenerated stake modifier
    };

    uint64 nStakeModifier; // hash modifier for proof-of-stake
    unsigned int nStakeModifierChecksum; // checksum of index; in-memeory only

    // proof-of-stake specific fields
    COutPoint prevoutStake;
    unsigned int nStakeTime;
    uint256 hashProofOfStake;

    // block header
    int nVersion;
    uint256 hashMerkleRoot;
    unsigned int nTime;
    unsigned int nBits;
    unsigned int nNonce;

    CBlockIndex()
    {
        phashBlock = NULL;
        pprev = NULL;
        pnext = NULL;
        nFile = 0;
        nBlockPos = 0;
        nHeight = 0;
        bnChainTrust = 0;
        nMint = 0;
        nMoneySupply = 0;
        nFlags = 0;
        nStakeModifier = 0;
        nStakeModifierChecksum = 0;
        hashProofOfStake = 0;
        prevoutStake.SetNull();
        nStakeTime = 0;

        nVersion       = 0;
        hashMerkleRoot = 0;
        nTime          = 0;
        nBits          = 0;
        nNonce         = 0;
    }

    CBlockIndex(unsigned int nFileIn, unsigned int nBlockPosIn, CBlock& block)
    {
        phashBlock = NULL;
        pprev = NULL;
        pnext = NULL;
        nFile = nFileIn;
        nBlockPos = nBlockPosIn;
        nHeight = 0;
        bnChainTrust = 0;
        nMint = 0;
        nMoneySupply = 0;
        nFlags = 0;
        nStakeModifier = 0;
        nStakeModifierChecksum = 0;
        hashProofOfStake = 0;
        if (block.IsProofOfStake())
        {
            SetProofOfStake();
            prevoutStake = block.vtx[1].vin[0].prevout;
            nStakeTime = block.vtx[1].nTime;
        }
        else
        {
            prevoutStake.SetNull();
            nStakeTime = 0;
        }

        nVersion       = block.nVersion;
        hashMerkleRoot = block.hashMerkleRoot;
        nTime          = block.nTime;
        nBits          = block.nBits;
        nNonce         = block.nNonce;
    }

    CBlock GetBlockHeader() const
    {
        CBlock block;
        block.nVersion       = nVersion;
        if (pprev)
            block.hashPrevBlock = pprev->GetBlockHash();
        block.hashMerkleRoot = hashMerkleRoot;
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        return block;
    }

    uint256 GetBlockHash() const
    {
        return *phashBlock;
    }

    int64 GetBlockTime() const
    {
        return (int64)nTime;
    }

    CBigNum GetBlockTrust() const
    {
        CBigNum bnTarget;
        bnTarget.SetCompact(nBits);
        if (bnTarget <= 0)
            return 0;
        return (IsProofOfStake() ? (CBigNum(1)<<256) / (bnTarget+1) : 1);
    }

    bool IsInMainChain() const
    {
        return (pnext || this == pindexBest);
    }

    bool CheckIndex() const
    {
        return true;
    }

    enum { nMedianTimeSpan=11 };

    int64 GetMedianTimePast() const
    {
        int64 pmedian[nMedianTimeSpan];
        int64* pbegin = &pmedian[nMedianTimeSpan];
        int64* pend = &pmedian[nMedianTimeSpan];

        const CBlockIndex* pindex = this;
        for (int i = 0; i < nMedianTimeSpan && pindex; i++, pindex = pindex->pprev)
            *(--pbegin) = pindex->GetBlockTime();

        std::sort(pbegin, pend);
        return pbegin[(pend - pbegin)/2];
    }

    int64 GetMedianTime() const
    {
        const CBlockIndex* pindex = this;
        for (int i = 0; i < nMedianTimeSpan/2; i++)
        {
            if (!pindex->pnext)
                return GetBlockTime();
            pindex = pindex->pnext;
        }
        return pindex->GetMedianTimePast();
    }

    /**
     * Returns true if there are nRequired or more blocks of minVersion or above
     * in the last nToCheck blocks, starting at pstart and going backwards.
     */
    static bool IsSuperMajority(int minVersion, const CBlockIndex* pstart,
                                unsigned int nRequired, unsigned int nToCheck);


    bool IsProofOfWork() const
    {
        return !(nFlags & BLOCK_PROOF_OF_STAKE);
    }

    bool IsProofOfStake() const
    {
        return (nFlags & BLOCK_PROOF_OF_STAKE);
    }

    void SetProofOfStake()
    {
        nFlags |= BLOCK_PROOF_OF_STAKE;
    }

    unsigned int GetStakeEntropyBit() const
    {
        return ((nFlags & BLOCK_STAKE_ENTROPY) >> 1);
    }

    bool SetStakeEntropyBit(unsigned int nEntropyBit)
    {
        if (nEntropyBit > 1)
            return false;
        nFlags |= (nEntropyBit? BLOCK_STAKE_ENTROPY : 0);
        return true;
    }

    bool GeneratedStakeModifier() const
    {
        return (nFlags & BLOCK_STAKE_MODIFIER);
    }

    void SetStakeModifier(uint64 nModifier, bool fGeneratedStakeModifier)
    {
        nStakeModifier = nModifier;
        if (fGeneratedStakeModifier)
            nFlags |= BLOCK_STAKE_MODIFIER;
    }

    std::string ToString() const
    {
        return strprintf("CBlockIndex(nprev=%p, pnext=%p, nFile=%u, nBlockPos=%-6d nHeight=%d, nMint=%s, nMoneySupply=%s, nFlags=(%s)(%d)(%s), nStakeModifier=%016" PRI64x", nStakeModifierChecksum=%08x, hashProofOfStake=%s, prevoutStake=(%s), nStakeTime=%d merkle=%s, hashBlock=%s)",
            pprev, pnext, nFile, nBlockPos, nHeight,
            FormatMoney(nMint).c_str(), FormatMoney(nMoneySupply).c_str(),
            GeneratedStakeModifier() ? "MOD" : "-", GetStakeEntropyBit(), IsProofOfStake()? "PoS" : "PoW",
            nStakeModifier, nStakeModifierChecksum,
            hashProofOfStake.ToString().c_str(),
            prevoutStake.ToString().c_str(), nStakeTime,
            hashMerkleRoot.ToString().c_str(),
            GetBlockHash().ToString().c_str());
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};



/** Used to marshal pointers into hashes for db storage. */
class CDiskBlockIndex : public CBlockIndex
{
public:
    uint256 hashPrev;
    uint256 hashNext;

    CDiskBlockIndex()
    {
        hashPrev = 0;
        hashNext = 0;
    }

    explicit CDiskBlockIndex(CBlockIndex* pindex) : CBlockIndex(*pindex)
    {
        hashPrev = (pprev ? pprev->GetBlockHash() : 0);
        hashNext = (pnext ? pnext->GetBlockHash() : 0);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);

        READWRITE(hashNext);
        READWRITE(nFile);
        READWRITE(nBlockPos);
        READWRITE(nHeight);
        READWRITE(nMint);
        READWRITE(nMoneySupply);
        READWRITE(nFlags);
        READWRITE(nStakeModifier);
        if (IsProofOfStake())
        {
            READWRITE(prevoutStake);
            READWRITE(nStakeTime);
            READWRITE(hashProofOfStake);
        }
        else if (fRead)
        {
            const_cast<CDiskBlockIndex*>(this)->prevoutStake.SetNull();
            const_cast<CDiskBlockIndex*>(this)->nStakeTime = 0;
            const_cast<CDiskBlockIndex*>(this)->hashProofOfStake = 0;
        }

        // block header
        READWRITE(this->nVersion);
        READWRITE(hashPrev);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
    )

    uint256 GetBlockHash() const
    {
        CBlock block;
        block.nVersion        = nVersion;
        block.hashPrevBlock   = hashPrev;
        block.hashMerkleRoot  = hashMerkleRoot;
        block.nTime           = nTime;
        block.nBits           = nBits;
        block.nNonce          = nNonce;
        return block.GetHash();
    }


    std::string ToString() const
    {
        std::string str = "CDiskBlockIndex(";
        str += CBlockIndex::ToString();
        str += strprintf("\n                hashBlock=%s, hashPrev=%s, hashNext=%s)",
            GetBlockHash().ToString().c_str(),
            hashPrev.ToString().c_str(),
            hashNext.ToString().c_str());
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};








/** Describes a place in the block chain to another node such that if the
 * other node doesn't have the same branch, it can find a recent common trunk.
 * The further back it is, the further before the fork it may be.
 */
class CBlockLocator
{
protected:
    std::vector<uint256> vHave;
public:

    CBlockLocator()
    {
    }

    explicit CBlockLocator(const CBlockIndex* pindex)
    {
        Set(pindex);
    }

    explicit CBlockLocator(uint256 hashBlock)
    {
        std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hashBlock);
        if (mi != mapBlockIndex.end())
            Set((*mi).second);
    }

    CBlockLocator(const std::vector<uint256>& vHaveIn)
    {
        vHave = vHaveIn;
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    )

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull()
    {
        return vHave.empty();
    }

    void Set(const CBlockIndex* pindex)
    {
        vHave.clear();
        int nStep = 1;
        while (pindex)
        {
            vHave.push_back(pindex->GetBlockHash());

            // Exponentially larger steps back
            for (int i = 0; pindex && i < nStep; i++)
                pindex = pindex->pprev;
            if (vHave.size() > 10)
                nStep *= 2;
        }
        vHave.push_back((!fTestNet ? hashGenesisBlock : hashGenesisBlockTestNet));
    }

    int GetDistanceBack()
    {
        // Retrace how far back it was in the sender's branch
        int nDistance = 0;
        int nStep = 1;
        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;
                if (pindex->IsInMainChain())
                    return nDistance;
            }
            nDistance += nStep;
            if (nDistance > 10)
                nStep *= 2;
        }
        return nDistance;
    }

    CBlockIndex* GetBlockIndex()
    {
        // Find the first block the caller has in the main chain
        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;
                if (pindex->IsInMainChain())
                    return pindex;
            }
        }
        return pindexGenesisBlock;
    }

    uint256 GetBlockHash()
    {
        // Find the first block the caller has in the main chain
        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;
                if (pindex->IsInMainChain())
                    return hash;
            }
        }
        return (!fTestNet ? hashGenesisBlock : hashGenesisBlockTestNet);
    }

    int GetHeight()
    {
        CBlockIndex* pindex = GetBlockIndex();
        if (!pindex)
            return 0;
        return pindex->nHeight;
    }
};








class CTxMemPool
{
public:
    mutable CCriticalSection cs;
    std::map<uint256, CTransaction> mapTx;
    std::map<COutPoint, CInPoint> mapNextTx;

    bool accept(CTxDB& txdb, CTransaction &tx,
                bool fCheckInputs, bool* pfMissingInputs);
    bool addUnchecked(const uint256& hash, CTransaction &tx);
    bool remove(CTransaction &tx);
    void clear();
    void queryHashes(std::vector<uint256>& vtxid);

    unsigned long size()
    {
        LOCK(cs);
        return mapTx.size();
    }

    bool exists(uint256 hash)
    {
        return (mapTx.count(hash) != 0);
    }

    CTransaction& lookup(uint256 hash)
    {
        return mapTx[hash];
    }
};

extern CTxMemPool mempool;

/** Data structure that represents a partial merkle tree.
 *
 * It respresents a subset of the txid's of a known block, in a way that
 * allows recovery of the list of txid's and the merkle root, in an
 * authenticated way.
 *
 * The encoding works as follows: we traverse the tree in depth-first order,
 * storing a bit for each traversed node, signifying whether the node is the
 * parent of at least one matched leaf txid (or a matched txid itself). In
 * case we are at the leaf level, or this bit is 0, its merkle node hash is
 * stored, and its children are not explorer further. Otherwise, no hash is
 * stored, but we recurse into both (or the only) child branch. During
 * decoding, the same depth-first traversal is performed, consuming bits and
 * hashes as they written during encoding.
 *
 * The serialization is fixed and provides a hard guarantee about the
 * encoded size:
 *
 *   SIZE <= 10 + ceil(32.25*N)
 *
 * Where N represents the number of leaf nodes of the partial tree. N itself
 * is bounded by:
 *
 *   N <= total_transactions
 *   N <= 1 + matched_transactions*tree_height
 *
 * The serialization format:
 *  - uint32     total_transactions (4 bytes)
 *  - varint     number of hashes   (1-3 bytes)
 *  - uint256[]  hashes in depth-first order (<= 32*N bytes)
 *  - varint     number of bytes of flag bits (1-3 bytes)
 *  - byte[]     flag bits, packed per 8 in a byte, least significant bit first (<= 2*N-1 bits)
 * The size constraints follow from this.
 */
class CPartialMerkleTree
{
protected:
    // the total number of transactions in the block
    unsigned int nTransactions;

    // node-is-parent-of-matched-txid bits
    std::vector<bool> vBits;

    // txids and internal hashes
    std::vector<uint256> vHash;

    // flag set when encountering invalid data
    bool fBad;

    // helper function to efficiently calculate the number of nodes at given height in the merkle tree
    unsigned int CalcTreeWidth(int height) {
        return (nTransactions+(1 << height)-1) >> height;
    }

    // calculate the hash of a node in the merkle tree (at leaf level: the txid's themself)
    uint256 CalcHash(int height, unsigned int pos, const std::vector<uint256> &vTxid);

    // recursive function that traverses tree nodes, storing the data as bits and hashes
    void TraverseAndBuild(int height, unsigned int pos, const std::vector<uint256> &vTxid, const std::vector<bool> &vMatch);

    // recursive function that traverses tree nodes, consuming the bits and hashes produced by TraverseAndBuild.
    // it returns the hash of the respective node.
    uint256 TraverseAndExtract(int height, unsigned int pos, unsigned int &nBitsUsed, unsigned int &nHashUsed, std::vector<uint256> &vMatch);

public:

    // serialization implementation
    IMPLEMENT_SERIALIZE(
        READWRITE(nTransactions);
        READWRITE(vHash);
        std::vector<unsigned char> vBytes;
        if (fRead) {
            READWRITE(vBytes);
            CPartialMerkleTree &us = *(const_cast<CPartialMerkleTree*>(this));
            us.vBits.resize(vBytes.size() * 8);
            for (unsigned int p = 0; p < us.vBits.size(); p++)
                us.vBits[p] = (vBytes[p / 8] & (1 << (p % 8))) != 0;
            us.fBad = false;
        } else {
            vBytes.resize((vBits.size()+7)/8);
            for (unsigned int p = 0; p < vBits.size(); p++)
                vBytes[p / 8] |= vBits[p] << (p % 8);
            READWRITE(vBytes);
        }
    )

    // Construct a partial merkle tree from a list of transaction id's, and a mask that selects a subset of them
    CPartialMerkleTree(const std::vector<uint256> &vTxid, const std::vector<bool> &vMatch);

    CPartialMerkleTree();

    // extract the matching txid's represented by this partial merkle tree.
    // returns the merkle root, or 0 in case of failure
    uint256 ExtractMatches(std::vector<uint256> &vMatch);
};

/** Used to relay blocks as header + vector<merkle branch>
 * to filtered nodes.
 */
class CMerkleBlock
{
public:
    // Public only for unit testing
    CBlockHeader
     header;
    CPartialMerkleTree txn;

public:
    // Public only for unit testing and relay testing
    // (not relayed)
    std::vector<std::pair<unsigned int, uint256> > vMatchedTxn;

    // Create from a CBlock, filtering transactions according to filter
    // Note that this will call IsRelevantAndUpdate on the filter for each transaction,
    // thus the filter will likely be modified.
    CMerkleBlock(const CBlock& block, CBloomFilter& filter);

    IMPLEMENT_SERIALIZE
    (
        READWRITE(header);
        READWRITE(txn);
    )
};

#endif
