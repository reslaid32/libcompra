
#include <compra/compra.h>
using namespace LIBCOMPRA_NAMESPACE;

#include <test_framework.h>
#include <exception>

std::string input = "HELLO WORLD "
                    "FOO BAR "
                    "1337";

TEST_CASE(lz77, LZ77 Compression) {
    auto compressed = LZ77::compress(input);
    auto decompressed = LZ77::decompress(compressed);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(lz78, LZ78 Compression) {
    auto compressed = LZ78::compress(input);
    auto decompressed = LZ78::decompress(compressed);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(lzma, LZMA Compression) {
    auto compressed = LZMA::compress(input);
    auto decompressed = LZMA::decompress(compressed);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(huffman, Huffman Compression) {
    auto [compressed, freqmap, bitlen] = Huffman::compress(input);
    auto decompressed = Huffman::decompress(compressed, freqmap, bitlen);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(deflate, Deflate Compression) {
    auto [compressed, freqmap, bitlen] = Deflate::compress(input);
    auto decompressed = Deflate::decompress(compressed, freqmap, bitlen);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(lz4, LZ4 Compression) {
    auto compressed = LZ4::compress(input);
    auto decompressed = LZ4::decompress(compressed);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(lz5, LZ5 Compression) {
    auto compressed = LZ5::compress(input);
    auto decompressed = LZ5::decompress(compressed);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(lzw, LZW Compression) {
    auto compressed = LZW::compress(input);
    auto decompressed = LZW::decompress(compressed);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(lzo, LZO Compression) {
    auto compressed = LZO::compress(input);
    auto decompressed = LZO::decompress(compressed);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(lzss, LZSS Compression) {
    auto compressed = LZSS::compress(input);
    auto decompressed = LZSS::decompress(compressed);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(fse, FSE Compression) {
    auto [compressed, encodingTable, bitLength] = FSE::compress(input);
    auto decompressed = FSE::decompress(compressed, encodingTable, bitLength);
    ASSERT_EQ(input, decompressed);
}

TEST_CASE(zstd, Zstandard Compression) {
    auto [compressed, encodingTable, bitLength] = Zstandard::compress(input);
    auto decompressed = Zstandard::decompress(compressed, encodingTable, bitLength);
    ASSERT_EQ(input, decompressed);
}

RUN_ALL_TESTS();