// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 Bitcoin Developers
// Copyright (c) 2014-2015 MonetaryUnit Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MONETARYUNIT_MINER_H
#define MONETARYUNIT_MINER_H

#include <stdint.h>

class CBlock;
class CBlockIndex;
struct CBlockTemplate;
class CReserveKey;
class CScript;
class CWallet;

/** Run the miner threads */
void GenerateMonetaryUnits(bool fGenerate, CWallet* pwallet, int nThreads);
/** Generate a new block, without valid proof-of-work */
CBlockTemplate* CreateNewBlock(const CScript& scriptPubKeyIn);
CBlockTemplate* CreateNewBlockWithKey(CReserveKey& reservekey);
/** Modify the extranonce in a block */
void IncrementExtraNonce(CBlock* pblock, CBlockIndex* pindexPrev, unsigned int& nExtraNonce);
/** Do mining precalculation */
void FormatHashBuffers(CBlock* pblock, char* pmidstate, char* pdata, char* phash1);
/** Check mined block */
bool CheckWork(CBlock* pblock, CWallet& wallet, CReserveKey& reservekey);
/** Base sha256 mining transform */
void SHA256Transform(void* pstate, void* pinput, const void* pinit);

extern double dHashesPerSec;
extern int64_t nHPSTimerStart;

#endif // MONETARYUNIT_MINER_H
