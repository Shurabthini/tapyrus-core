// Copyright (c) 2012-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <merkleblock.h>
#include <uint256.h>
#include <test/test_bitcoin.h>

#include <boost/test/unit_test.hpp>


BOOST_FIXTURE_TEST_SUITE(merkleblock_tests, BasicTestingSetup)

/**
 * Create a CMerkleBlock using a list of txids which will be found in the
 * given block.
 */
BOOST_AUTO_TEST_CASE(merkleblock_construct_from_txids_found)
{
    CBlock block = getBlock();

    std::set<uint256> txids;

    // Last txn in block.
    uint256 txhash1 = uint256S("0x460d152325ff55f0d2949433fe573b6458ac2cb538b881e8e44a6e00a62d1c42");

    // Second txn in block.
    uint256 txhash2 = uint256S("0x78809acb2f525c448f9fd4b301d1166c6a9417d31530a8540fabbccfed7196dc");

    txids.insert(txhash1);
    txids.insert(txhash2);

    CMerkleBlock merkleBlock(block, txids);

    BOOST_CHECK_EQUAL(merkleBlock.header.GetHash().GetHex(), block.GetHash().GetHex());

    // vMatchedTxn is only used when bloom filter is specified.
    BOOST_CHECK_EQUAL(merkleBlock.vMatchedTxn.size(), 0U);

    std::vector<uint256> vMatched;
    std::vector<unsigned int> vIndex;
    merkleBlock.txn.ExtractMatches(vMatched, vIndex);
    BOOST_CHECK_EQUAL(merkleBlock.txn.ExtractMatches(vMatched, vIndex).GetHex(), block.hashImMerkleRoot.GetHex());
    BOOST_CHECK_EQUAL(vMatched.size(), 2U);

    // Ordered by occurrence in depth-first tree traversal.
    BOOST_CHECK_EQUAL(vMatched[0].ToString(), txhash2.ToString());
    BOOST_CHECK_EQUAL(vIndex[0], 1U);

    BOOST_CHECK_EQUAL(vMatched[1].ToString(), txhash1.ToString());
    BOOST_CHECK_EQUAL(vIndex[1], 8U);
}


/**
 * Create a CMerkleBlock using a list of txids which will not be found in the
 * given block.
 */
BOOST_AUTO_TEST_CASE(merkleblock_construct_from_txids_not_found)
{
    CBlock block = getBlock();

    std::set<uint256> txids2;
    txids2.insert(uint256S("0xc0ffee00003bafa802c8aa084379aa98d9fcd632ddc2ed9782b586ec87451f20"));
    CMerkleBlock merkleBlock(block, txids2);

    BOOST_CHECK_EQUAL(merkleBlock.header.GetHash().GetHex(), block.GetHash().GetHex());
    BOOST_CHECK_EQUAL(merkleBlock.vMatchedTxn.size(), 0U);

    std::vector<uint256> vMatched;
    std::vector<unsigned int> vIndex;
    merkleBlock.txn.ExtractMatches(vMatched, vIndex);
    BOOST_CHECK_EQUAL(merkleBlock.txn.ExtractMatches(vMatched, vIndex).GetHex(), block.hashImMerkleRoot.GetHex());
    BOOST_CHECK_EQUAL(vMatched.size(), 0U);
    BOOST_CHECK_EQUAL(vIndex.size(), 0U);
}

BOOST_AUTO_TEST_SUITE_END()
