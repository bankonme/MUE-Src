// Copyright (c) 2009-2015 Bitcoin Developers
// Copyright (c) 2014-2015 MonetaryUnit Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"
#include "key.h"
#include "txdb.h"
#include "base58.h"

#include <stdint.h>

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double SIGCHECK_VERIFICATION_FACTOR = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64_t nTimeLastCheckpoint;
        int64_t nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    bool fEnabled = true;

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0, uint256("0x0000070e6b650e7a6f20e015031b74c1f7e2b25ed4e419d8825ab9cc7eccfa92"))
        ( 1, uint256("0x000008a6e7c1c9547c5b2e7bc1e30f813cc9684428e11e8bbc2f0f270943f318"))
        ( 50000, uint256("0x00000010be6026f1f993dc2d51654ffeab948269cc067ea0bd3760c87ecfc7d0"))
        ( 100000, uint256("0x000000380d10bb4bb67ff8ef4c193f9a83a9d390389138c8a690722fb1ee2094"))
        ( 150000, uint256("0x000000020ee690f42fb7937038de393a734c7b0094b0185491c8483038b54a79"))
        ( 200000, uint256("0x0000000089e209620a4819d46a7bdbba03924d3af1ae6f7668d8cc977d4e542c"))
        ( 250000, uint256("0x000000010f69e2769cdacd033779acf44c7dc1e862eb5314b7179088d1c38b1e"))
        ( 281000, uint256("0x00000000b1fb0f01e1eca91c8f6b869ec339f64210afeb793a5d75fc6c26b87b"))
        ( 460000, uint256("0x00000000add7d91ce6ee36330c6ac821da1bda24dfbdf534d56c74e7f87da395"))
        ( 470000, uint256("0x00000001a7e2ebc09c3e8df413180b800c0a61f26fe744e539ddd339f89a468f"))
        ( 490000, uint256("0x00000000dfba6baf869f8b5b43568b3008531046fef5a6caf83ca2082b264e21"))
        ( 500000, uint256("0x0000000041e7a379e70a0bd8e2fcb97e9badd40199df9d4232ede49c87706a88"))
        ( 510000, uint256("0x000000000249a0638e89196a806350de4c302687c8939486875dd26faf102838"))
        ( 520000, uint256("0x000000006f3915dd9fceb71a3bcd7db79f069a7a3f81f0292eb69422628bb7b5"))
        ( 530000, uint256("0x0000000028aa5e234f9923e3b600ed15faf597e9adea8c0a63cc407122fd5452"))
        ( 540000, uint256("0x0000000029695216538bc6f2ed706237a2a7fc120ab53f37a629d69735babde4"))
        ( 550000, uint256("0x00000000209dbf5cdabdee5c0d1c19ba9d531386cebe0958d0e8ddb83a6fc513"))
        ( 600000, uint256("0x000000000f10d6e637f485f504745d99b9b372ac55b68cdb1a91c9d8f69bfb08"))
        ( 650000, uint256("0x0000000026a04fea7e487f152114788eefd49cadbb9010897a26b43865183f93"))
        ( 700000, uint256("0x00000000260bb06e7ca785e93a09b515d88fb4a83afd4fdb7b4e900bb6905247"))
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1432458632, // * UNIX timestamp of last checkpoint block
        918672,     // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        2880.0      // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        ( 0, uint256("0x0000070e6b650e7a6f20e015031b74c1f7e2b25ed4e419d8825ab9cc7eccfa92"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1373481000,
        0,
        2880.0
    };

    static MapCheckpoints mapCheckpointsRegtest =
        boost::assign::map_list_of
        ( 0, uint256("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"))
        ;
    static const CCheckpointData dataRegtest = {
        &mapCheckpointsRegtest,
        0,
        0,
        0
    };

    const CCheckpointData &Checkpoints() {
        if (Params().NetworkID() == CChainParams::TESTNET)
            return dataTestnet;
        else if (Params().NetworkID() == CChainParams::MAIN)
            return data;
        else
            return dataRegtest;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (!fEnabled)
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex, bool fSigchecks) {
        if (pindex==NULL)
            return 0.0;

        int64_t nNow = time(NULL);

        double fSigcheckVerificationFactor = fSigchecks ? SIGCHECK_VERIFICATION_FACTOR : 1.0;
        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkpoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!fEnabled)
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!fEnabled)
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    uint256 GetLatestHardenedCheckpoint()
    {
        LogPrintf("GetLatestHardenedCheckpoint\n");
        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return (checkpoints.rbegin()->second);
    }

    // ppcoin: synchronized checkpoint (centrally broadcasted)
    uint256 hashSyncCheckpoint = 0;
    uint256 hashPendingCheckpoint = 0;
    CSyncCheckpoint checkpointMessage;
    CSyncCheckpoint checkpointMessagePending;
    uint256 hashInvalidCheckpoint = 0;
    CCriticalSection cs_hashSyncCheckpoint;
    std::string strCheckpointWarning;

    // ppcoin: only descendant of current sync-checkpoint is allowed
    bool ValidateSyncCheckpoint(uint256 hashCheckpoint)
    {
        LogPrintf("ValidateSyncCheckpoint: hashCheckpoint=%s\n", hashCheckpoint.ToString().c_str());

        if ( (hashSyncCheckpoint == 0) || (!mapBlockIndex.count(hashSyncCheckpoint)) )
        {
            // NO SYNC CHECKPOINT
            return true;
        }

        if (!mapBlockIndex.count(hashSyncCheckpoint))
            return error("ValidateSyncCheckpoint: block index missing for current sync-checkpoint %s", hashSyncCheckpoint.ToString().c_str());
        if (!mapBlockIndex.count(hashCheckpoint))
            return error("ValidateSyncCheckpoint: block index missing for received sync-checkpoint %s", hashCheckpoint.ToString().c_str());

        CBlockIndex* pindexSyncCheckpoint = mapBlockIndex[hashSyncCheckpoint];
        CBlockIndex* pindexCheckpointRecv = mapBlockIndex[hashCheckpoint];

        if (pindexCheckpointRecv->nHeight <= pindexSyncCheckpoint->nHeight)
        {
            // Received an older checkpoint, trace back from current checkpoint
            // to the same height of the received checkpoint to verify
            // that current checkpoint should be a descendant block
            CBlockIndex* pindex = pindexSyncCheckpoint;
            while (pindex->nHeight > pindexCheckpointRecv->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("ValidateSyncCheckpoint: pprev1 null - block index structure failure");
            if (pindex->GetBlockHash() != hashCheckpoint)
            {
                hashInvalidCheckpoint = hashCheckpoint;
                return error("ValidateSyncCheckpoint: new sync-checkpoint %s is conflicting with current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
            }
            return false; // ignore older checkpoint
        }

        // Received checkpoint should be a descendant block of the current
        // checkpoint. Trace back to the same height of current checkpoint
        // to verify.
        CBlockIndex* pindex = pindexCheckpointRecv;
        while (pindex->nHeight > pindexSyncCheckpoint->nHeight)
            if (!(pindex = pindex->pprev))
                return error("ValidateSyncCheckpoint: pprev2 null - block index structure failure");
        if (pindex->GetBlockHash() != hashSyncCheckpoint)
        {
            hashInvalidCheckpoint = hashCheckpoint;
            return error("ValidateSyncCheckpoint: new sync-checkpoint %s is not a descendant of current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
        }

        LogPrintf("ValidateSyncCheckpoint: OK\n");
        return true;
    }

    bool WriteSyncCheckpoint(const uint256& hashCheckpoint)
    {
        if (!pblocktree->WriteSyncCheckpoint(hashCheckpoint))
        {
            return error("WriteSyncCheckpoint(): failed to write to txdb sync checkpoint %s", hashCheckpoint.ToString().c_str());
        }

        hashSyncCheckpoint = hashCheckpoint;
        return true;
    }

    bool IsSyncCheckpointEnforced()
    {
        return (GetBoolArg("-checkpointenforce", true) || mapArgs.count("-checkpointkey")); // checkpoint master node is always enforced
    }

    bool AcceptPendingSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint != 0 && mapBlockIndex.count(hashPendingCheckpoint))
        {
            if (!ValidateSyncCheckpoint(hashPendingCheckpoint))
            {
                hashPendingCheckpoint = 0;
                checkpointMessagePending.SetNull();
                LogPrintf("AcceptPendingSyncCheckpoint: FAIL1\n");
                return false;
            }

            CBlockIndex* pindexCheckpoint = mapBlockIndex[hashPendingCheckpoint];
            if (IsSyncCheckpointEnforced() && !pindexCheckpoint->IsInMainChain())
            {
                CBlock block;
                if (!ReadBlockFromDisk(block, pindexCheckpoint))
                    return error("AcceptPendingSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
                CValidationState state;
                LogPrintf("AcceptPendingSyncCheckpoint: ConnectTip\n");
                // if (!SetBestChain(state, pindexCheckpoint))
                if (!ConnectTip(state, pindexCheckpoint))
                {
                    hashInvalidCheckpoint = hashPendingCheckpoint;
                    return error("AcceptPendingSyncCheckpoint: SetBestChain failed for sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
                }
            }

            if (!WriteSyncCheckpoint(hashPendingCheckpoint))
                return error("AcceptPendingSyncCheckpoint(): failed to write sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
            hashPendingCheckpoint = 0;
            checkpointMessage = checkpointMessagePending;
            checkpointMessagePending.SetNull();
            LogPrintf("AcceptPendingSyncCheckpoint : sync-checkpoint at %s\n", hashSyncCheckpoint.ToString().c_str());
            // relay the checkpoint
            if (!checkpointMessage.IsNull())
            {
                BOOST_FOREACH(CNode* pnode, vNodes)
                    checkpointMessage.RelayTo(pnode);
            }
            return true;
        }
        return false;
    }

    // Automatically select a suitable sync-checkpoint
    uint256 AutoSelectSyncCheckpoint()
    {
        // Search backward for a block with specified depth policy
        // const CBlockIndex *pindex = pindexBest;
        // while (pindex->pprev && pindex->nHeight + (int)GetArg("-checkpointdepth", -1) > pindexBest->nHeight)
        const CBlockIndex *pindex = chainActive.Tip();
        while (pindex->pprev && pindex->nHeight + (int)GetArg("-checkpointdepth", -1) > chainActive.Tip()->nHeight)
            pindex = pindex->pprev;
        return pindex->GetBlockHash();
    }

    // Check against synchronized checkpoint
    bool CheckSyncCheckpoint(const uint256& hashBlock, const CBlockIndex* pindexPrev)
    {
        int nHeight = pindexPrev->nHeight + 1;
        LogPrintf("CheckSyncCheckpoint: nHeight=%d, hashSyncCheckpoint=%s\n", nHeight, hashSyncCheckpoint.ToString().c_str());

        LOCK(cs_hashSyncCheckpoint);

        if ((hashSyncCheckpoint == 0) || (mapBlockIndex.count(hashSyncCheckpoint) == 0))
            return true;

        // sync-checkpoint should always be accepted block
        // assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];

        if (nHeight > pindexSync->nHeight)
        {
            // trace back to same height as sync-checkpoint
            const CBlockIndex* pindex = pindexPrev;
            while (pindex->nHeight > pindexSync->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("CheckSyncCheckpoint: pprev null - block index structure failure");
            if (pindex->nHeight < pindexSync->nHeight || pindex->GetBlockHash() != hashSyncCheckpoint)
                return false; // only descendant of sync-checkpoint can pass check
        }
        if (nHeight == pindexSync->nHeight && hashBlock != hashSyncCheckpoint)
            return false; // same height with sync-checkpoint
        if (nHeight < pindexSync->nHeight && !mapBlockIndex.count(hashBlock))
            return false; // lower height than sync-checkpoint
        return true;
    }

    bool WantedByPendingSyncCheckpoint(uint256 hashBlock)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint == 0)
            return false;
        if (hashBlock == hashPendingCheckpoint)
            return true;
        if (mapOrphanBlocksSyncCheckpoint.count(hashPendingCheckpoint)
            && hashBlock == WantedByOrphan(mapOrphanBlocksSyncCheckpoint[hashPendingCheckpoint]))
            return true;
        return false;
    }

    // ppcoin: reset synchronized checkpoint to last hardened checkpoint
    bool ResetSyncCheckpoint()
    {
        LogPrintf("ResetSyncCheckpoint\n");
        LOCK(cs_hashSyncCheckpoint);
        const uint256& hash = Checkpoints::GetLatestHardenedCheckpoint();
        if (mapBlockIndex.count(hash) && !mapBlockIndex[hash]->IsInMainChain())
        {
            // checkpoint block accepted but not yet in main chain
            LogPrintf("ResetSyncCheckpoint: SetBestChain to hardened checkpoint %s\n", hash.ToString().c_str());

            CBlock block;
            if (!ReadBlockFromDisk(block, mapBlockIndex[hash]))
                return error("ResetSyncCheckpoint: ReadFromDisk failed for hardened checkpoint %s", hash.ToString().c_str());
            CValidationState state;
            LogPrintf("ResetSyncCheckpoint: ConnectTip\n");
            // if (!SetBestChain(state, mapBlockIndex[hash]))
            if (!ConnectTip(state, mapBlockIndex[hash]))
            {
                return error("ResetSyncCheckpoint: SetBestChain failed for hardened checkpoint %s", hash.ToString().c_str());
            }
        }
        else if(!mapBlockIndex.count(hash))
        {
            // checkpoint block not yet accepted
            hashPendingCheckpoint = hash;
            checkpointMessagePending.SetNull();
            LogPrintf("ResetSyncCheckpoint: pending for sync-checkpoint %s\n", hashPendingCheckpoint.ToString().c_str());
        }

        if (!WriteSyncCheckpoint((mapBlockIndex.count(hash) && mapBlockIndex[hash]->IsInMainChain())? hash : Params().HashGenesisBlock()))
            return error("ResetSyncCheckpoint: failed to write sync checkpoint %s", hash.ToString().c_str());
        LogPrintf("ResetSyncCheckpoint: sync-checkpoint reset to %s\n", hashSyncCheckpoint.ToString().c_str());
        return true;
    }

    void AskForPendingSyncCheckpoint(CNode* pfrom)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (pfrom && hashPendingCheckpoint != 0 && (!mapBlockIndex.count(hashPendingCheckpoint)) && (!mapOrphanBlocksSyncCheckpoint.count(hashPendingCheckpoint)))
            pfrom->AskFor(CInv(MSG_BLOCK, hashPendingCheckpoint));
    }

    // Verify sync checkpoint master pubkey and reset sync checkpoint if changed
    bool CheckCheckpointPubKey()
    {
        std::string strPubKey = "";
        std::string strMasterPubKey = TestNet() ? CSyncCheckpoint::strTestPubKey : CSyncCheckpoint::strMainPubKey;
        if (!pblocktree->ReadCheckpointPubKey(strPubKey) || strPubKey != strMasterPubKey)
        {
            // write checkpoint master key to db
            if (!pblocktree->WriteCheckpointPubKey(strMasterPubKey))
                return error("CheckCheckpointPubKey() : failed to write new checkpoint master key to db");
            if (!ResetSyncCheckpoint())
                return error("CheckCheckpointPubKey() : failed to reset sync-checkpoint");
        }
        return true;
    }

    bool SetCheckpointPrivKey(std::string strPrivKey)
    {
        // Test signing a sync-checkpoint with genesis block
        CSyncCheckpoint checkpoint;
        checkpoint.hashCheckpoint = Params().HashGenesisBlock();
        CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
        sMsg << (CUnsignedSyncCheckpoint)checkpoint;
        checkpoint.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());

        CMonetaryUnitSecret vchSecret;
        if (!vchSecret.SetString(strPrivKey))
            return error("SendSyncCheckpoint: Checkpoint master key invalid");
        // bool fCompressed;
        // CSecret secret = vchSecret.GetSecret(fCompressed);
        CKey secret = vchSecret.GetKey();
        // key.SetSecret(secret, fCompressed); // if key is not correct openssl may crash
        CKey key(secret);
        if (!key.Sign(Hash(checkpoint.vchMsg.begin(), checkpoint.vchMsg.end()), checkpoint.vchSig))
            return false;

        // Test signing successful, proceed
        CSyncCheckpoint::strMasterPrivKey = strPrivKey;
        return true;
    }

    bool SendSyncCheckpoint(uint256 hashCheckpoint)
    {
        LogPrintf("SendSyncCheckpoint: hashCheckpoint=%s\n", hashCheckpoint.ToString().c_str());

        CSyncCheckpoint checkpoint;
        checkpoint.hashCheckpoint = hashCheckpoint;
        CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
        sMsg << (CUnsignedSyncCheckpoint)checkpoint;
        checkpoint.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());

        if (CSyncCheckpoint::strMasterPrivKey.empty())
            return error("SendSyncCheckpoint: Checkpoint master key unavailable.");
        CMonetaryUnitSecret vchSecret;
        if (!vchSecret.SetString(CSyncCheckpoint::strMasterPrivKey))
            return error("SendSyncCheckpoint: Checkpoint master key invalid");
        // bool fCompressed;
        // CSecret secret = vchSecret.GetSecret(fCompressed);
        CKey secret = vchSecret.GetKey();
        // key.SetSecret(secret, fCompressed); // if key is not correct openssl may crash
        CKey key(secret);
        if (!key.Sign(Hash(checkpoint.vchMsg.begin(), checkpoint.vchMsg.end()), checkpoint.vchSig))
            return error("SendSyncCheckpoint: Unable to sign checkpoint, check private key?");

        if(!checkpoint.ProcessSyncCheckpoint(NULL))
        {
            LogPrintf("WARNING: SendSyncCheckpoint: Failed to process checkpoint.\n");
            return false;
        }

        // Relay checkpoint
        {
            LOCK(cs_vNodes);
            BOOST_FOREACH(CNode* pnode, vNodes)
                checkpoint.RelayTo(pnode);
        }
        return true;
    }

    // Is the sync-checkpoint outside maturity window?
    bool IsMatureSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];
        // return (nBestHeight >= pindexSync->nHeight + COINBASE_MATURITY);
        return (chainActive.Tip()->nHeight >= pindexSync->nHeight + COINBASE_MATURITY);
    }

    // Is the sync-checkpoint too old?
    bool IsSyncCheckpointTooOld(unsigned int nSeconds)
    {
        LOCK(cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];
        return (pindexSync->GetBlockTime() + nSeconds < GetAdjustedTime());
    }

    // ppcoin: find block wanted by given orphan block
    uint256 WantedByOrphan(const CBlock* pblockOrphan)
    {
        // Work back to the first block in the orphan chain
        while (mapOrphanBlocksSyncCheckpoint.count(pblockOrphan->hashPrevBlock))
            pblockOrphan = mapOrphanBlocksSyncCheckpoint[pblockOrphan->hashPrevBlock];
        return pblockOrphan->hashPrevBlock;
    }

}

// ppcoin: sync-checkpoint master key
const std::string CSyncCheckpoint::strMainPubKey = "0466aa7cf205be5c40f114c80d0d4087959508ace5642c9b849af1ba78d7c6b969f3e8d36b3d44e5a0ac1d2d8f3e6f7452055713943870700385544c2a04c5aa55";
const std::string CSyncCheckpoint::strTestPubKey = "041ba70a9e3afd1c0c13b7577e4f71ede2eee884df617fa28bfb0ee3fe993b9cc2835c16b794e46095bf425c4e2cdc2e628becdb196f0302840282d3d32d6c69bd";
std::string CSyncCheckpoint::strMasterPrivKey = "";

// ppcoin: verify signature of sync-checkpoint message
bool CSyncCheckpoint::CheckSignature()
{
    std::string strMasterPubKey = TestNet() ? CSyncCheckpoint::strTestPubKey : CSyncCheckpoint::strMainPubKey;
//    if (!key.Set(ParseHex(strMasterPubKey)))
//        return error("CSyncCheckpoint::CheckSignature() : SetPubKey failed");
    CPubKey key(ParseHex(strMasterPubKey));
    if (!key.Verify(Hash(vchMsg.begin(), vchMsg.end()), vchSig))
        return error("CSyncCheckpoint::CheckSignature() : verify signature failed");

    // Now unserialize the data
    CDataStream sMsg(vchMsg, SER_NETWORK, PROTOCOL_VERSION);
    sMsg >> *(CUnsignedSyncCheckpoint*)this;
    return true;
}

// ppcoin: process synchronized checkpoint
bool CSyncCheckpoint::ProcessSyncCheckpoint(CNode* pfrom)
{
    if (!CheckSignature())
        return false;

    LOCK(Checkpoints::cs_hashSyncCheckpoint);
    if (!mapBlockIndex.count(hashCheckpoint))
    {
        // We haven't received the checkpoint chain, keep the checkpoint as pending
        Checkpoints::hashPendingCheckpoint = hashCheckpoint;
        Checkpoints::checkpointMessagePending = *this;
        LogPrintf("ProcessSyncCheckpoint: pending for sync-checkpoint %s\n", hashCheckpoint.ToString().c_str());
        // Ask this guy to fill in what we're missing
        if (pfrom)
        {
            // pfrom->PushGetBlocks(pindexBest, hashCheckpoint);
			PushGetBlocks(pfrom, chainActive.Tip(), hashCheckpoint);
            // ask directly as well in case rejected earlier by duplicate
            // proof-of-stake because getblocks may not get it this time
            pfrom->AskFor(CInv(MSG_BLOCK, mapOrphanBlocksSyncCheckpoint.count(hashCheckpoint)? Checkpoints::WantedByOrphan(mapOrphanBlocksSyncCheckpoint[hashCheckpoint]) : hashCheckpoint));
        }
        return false;
    }

    if (!Checkpoints::ValidateSyncCheckpoint(hashCheckpoint))
        return false;

    CBlockIndex* pindexCheckpoint = mapBlockIndex[hashCheckpoint];
    if (!pindexCheckpoint->IsInMainChain())
    {
        // checkpoint chain received but not yet main chain
        CBlock block;
        if (!ReadBlockFromDisk(block, pindexCheckpoint))
            return error("ProcessSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());
        CValidationState state;
        LogPrintf("ProcessSyncCheckpoint: ConnectTip\n");
        // if (!SetBestChain(state, pindexCheckpoint))
        if (!ConnectTip(state, pindexCheckpoint))
        {
            Checkpoints::hashInvalidCheckpoint = hashCheckpoint;
            return error("ProcessSyncCheckpoint: SetBestChain failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());
        }
    }

    if (!Checkpoints::WriteSyncCheckpoint(hashCheckpoint))
        return error("ProcessSyncCheckpoint(): failed to write sync checkpoint %s", hashCheckpoint.ToString().c_str());
    Checkpoints::checkpointMessage = *this;
    Checkpoints::hashPendingCheckpoint = 0;
    Checkpoints::checkpointMessagePending.SetNull();

    LogPrintf("ProcessSyncCheckpoint: sync-checkpoint at %s\n", hashCheckpoint.ToString().c_str());
    return true;
}
