#ifndef LIBCOMPRA_H
#define LIBCOMPRA_H

#include "export.h"

#include <vector>
#include <string>
#include <map>
#include <queue>
#include <cstdint>

#define LIBCOMPRA_MAJOR 1
#define LIBCOMPRA_MINOR 1
#define LIBCOMPRA_NAMESPACE Compra

namespace LIBCOMPRA_NAMESPACE {
namespace LZ77 {
    struct Token {
        size_t offset;
        size_t length;
        char next;
    };

    LIBCOMPRA_API size_t findLongestMatch(const std::string& input, size_t i, size_t searchStart, size_t windowSize, size_t& bestOffset);

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t windowSize = 32 * 1024);

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens);

    namespace Utils {
        LIBCOMPRA_API std::string serializeToken(const Token& token);

        LIBCOMPRA_API std::string vectorToString(const std::vector<Token>& tokens);

        LIBCOMPRA_API Token deserializeToken(const std::string& str);

        LIBCOMPRA_API std::vector<Token> stringToVector(const std::string& str);
    }
}

namespace LZ78 {
    struct Token {
        size_t index;
        char next;
    };

    LIBCOMPRA_API size_t findPrefixIndex(const std::map<std::string, size_t>& dictionary, const std::string& buffer);

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input);

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens);

    namespace Utils {
        LIBCOMPRA_API std::string serializeToken(const Token& token);

        LIBCOMPRA_API std::string vectorToString(const std::vector<Token>& tokens);

        LIBCOMPRA_API Token deserializeToken(const std::string& str);

        LIBCOMPRA_API std::vector<Token> stringToVector(const std::string& str);
    }
}

namespace LZMA {
    struct Token {
        size_t position;
        size_t length;
        char next;
    };

    LIBCOMPRA_API size_t findLongestMatch(const std::string& input, size_t pos, size_t dictionarySize, size_t& matchPos);

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t dictionarySize = 4096);

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens, size_t dictionarySize = 4096);

    namespace Utils {
        LIBCOMPRA_API std::string serializeToken(const Token& token);

        LIBCOMPRA_API std::string vectorToString(const std::vector<Token>& tokens);

        LIBCOMPRA_API Token deserializeToken(const std::string& str);

        LIBCOMPRA_API std::vector<Token> stringToVector(const std::string& str);
    }
}

namespace Huffman {
    using Char = char;
    using Int = int;
    using Byte = unsigned char;
    using ByteVector = std::vector<Byte>;
    using FreqMap = std::map<Char, Int>;

    struct Compressed {
        ByteVector byteVec;
        FreqMap freqMap;
        size_t bitLength;
    };

    struct HuffmanNode {
        char data;
        int freq;
        HuffmanNode *left, *right;

        LIBCOMPRA_API HuffmanNode(char data, int freq);
    };

    struct HuffmanCompare {
        LIBCOMPRA_API bool operator()(HuffmanNode* left, HuffmanNode* right);
    };

    namespace Methods {
        LIBCOMPRA_API HuffmanNode* BuildHuffmanTree(const Huffman::FreqMap& freqMap);

        LIBCOMPRA_API void GenerateCodes(HuffmanNode* node, const std::string& code, std::map<char, std::string>& huffmanCode);

        LIBCOMPRA_API void FreeTree(HuffmanNode* node);

        LIBCOMPRA_API Huffman::ByteVector PackBitsToBytes(const std::string& bitString);

        LIBCOMPRA_API std::string UnpackBytesToBits(const Huffman::ByteVector& compressedData);

        LIBCOMPRA_API Huffman::ByteVector PackBitsToBytes(const std::string& bitString, size_t& bitLength);

        LIBCOMPRA_API std::string UnpackBytesToBits(const Huffman::ByteVector& compressedData, size_t bitLength);
    }

    LIBCOMPRA_API Compressed compress(const std::string& text);

    LIBCOMPRA_API std::string decompress(const ByteVector& compressed, const FreqMap& freqMap, size_t bitLength);

    LIBCOMPRA_API std::string decompress(const Compressed& compressed);

    /* COMPATIBILITY */

    LIBCOMPRA_API Huffman::ByteVector compress(const std::string& text, FreqMap& freqMap, size_t& bitLength);

    LIBCOMPRA_API Huffman::ByteVector Compress(const std::string& text, FreqMap& freqMap, size_t& bitLength);

    LIBCOMPRA_API std::string Decompress(const ByteVector& compressed, const FreqMap& freqMap, size_t bitLength);

    namespace Stringize {
        LIBCOMPRA_API std::string StringizeFreqMap(const Huffman::FreqMap& freqMap);

        LIBCOMPRA_API std::string StringizeByteVec(const Huffman::ByteVector& byteVec);
    }
}

namespace Deflate {
    LIBCOMPRA_API Huffman::Compressed compress(const std::string& input, size_t windowSize = 32 * 1024);

    LIBCOMPRA_API std::string decompress(const Huffman::ByteVector& byteVec, const Huffman::FreqMap& freqMap, const size_t bitLength);

    LIBCOMPRA_API std::string decompress(const Huffman::Compressed& compressed);
}


namespace LZ4 {
    LIBCOMPRA_API size_t findLongestMatch(const std::string& input, size_t i, size_t& match_offset);

    LIBCOMPRA_API std::string compress(const std::string& input);

    LIBCOMPRA_API std::string decompress(const std::string& input);
}

namespace LZ5 {
    struct Token {
        size_t offset;
        size_t length;
        char next;
    };

    LIBCOMPRA_API size_t findLongestMatch(const std::string& input, size_t pos, size_t maxOffset, size_t& matchOffset);

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t maxOffset = 32 * 1024);

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens);

    namespace Utils {
        LIBCOMPRA_API std::string serializeToken(const Token& token);

        LIBCOMPRA_API std::string vectorToString(const std::vector<Token>& tokens);

        LIBCOMPRA_API Token deserializeToken(const std::string& str);

        LIBCOMPRA_API std::vector<Token> stringToVector(const std::string& str);
    }
}

namespace LZW {
    LIBCOMPRA_API void addToDictionary(std::map<std::string, int>& dictionary, const std::string& key, int& code);

    LIBCOMPRA_API void compress(std::vector<int>& result, std::map<std::string, int>& dictionary, const std::string& input, int& code);

    LIBCOMPRA_API std::vector<int> compress(const std::string& input);

    LIBCOMPRA_API void decompress(std::string& result, std::map<int, std::string>& dictionary, const std::vector<int>& input, int& code);

    LIBCOMPRA_API std::string decompress(const std::vector<int>& input);

    LIBCOMPRA_API void compressOptimized(std::vector<int>& result, std::map<std::string, int>& dictionary, const std::string& input, int& code);

    LIBCOMPRA_API std::vector<int> compressOptimized(const std::string& input);

    LIBCOMPRA_API void decompressOptimized(std::string& result, std::map<int, std::string>& dictionary, const std::vector<int>& input, int& code);

    LIBCOMPRA_API std::string decompressOptimized(const std::vector<int>& input);
}

namespace LZO {
    struct Token {
        size_t offset;
        size_t length;
        char next;
    };

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t windowSize = 8 * 1024);

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens);

    namespace Utils {
        LIBCOMPRA_API std::string serializeToken(const Token& token);

        LIBCOMPRA_API std::string vectorToString(const std::vector<Token>& tokens);

        LIBCOMPRA_API Token deserializeToken(const std::string& str);

        LIBCOMPRA_API std::vector<Token> stringToVector(const std::string& str);
    }
}

namespace LZSS {
    struct Token {
        bool isLiteral;
        char literal;
        size_t offset;
        size_t length;
    };

    LIBCOMPRA_API size_t findBestMatch(const std::string& input, size_t windowStart, size_t currentPos, size_t windowSize, size_t& bestOffset);
    
    LIBCOMPRA_API void addToken(std::vector<Token>& tokens, bool isLiteral, char literal, size_t offset, size_t length);

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t windowSize = 4 * 1024, size_t lookaheadSize = 18);

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens);

    namespace Utils {
        LIBCOMPRA_API std::string serializeToken(const Token& token);

        LIBCOMPRA_API std::string vectorToString(const std::vector<Token>& tokens);

        LIBCOMPRA_API Token deserializeToken(const std::string& str);

        LIBCOMPRA_API std::vector<Token> stringToVector(const std::string& str);
    }
}

namespace FSE {
    struct Symbol {
        char character;
        size_t frequency;
    };

    struct EncodedSymbol {
        char character;
        std::string code;
    };

    using EncodingTable = std::map<char, EncodedSymbol>;
    using FrequencyTable = std::map<char, size_t>;
    using Byte = uint8_t;
    using ByteVector = std::vector<Byte>;

    struct Compressed {
        ByteVector byteVec;
        EncodingTable encodingTable;
        size_t bitLength;
    };

    namespace Methods {
        LIBCOMPRA_API EncodingTable buildEncodingTable(const std::string& input);

        LIBCOMPRA_API ByteVector PackBitsToBytes(const std::string& bitString);

        LIBCOMPRA_API std::string UnpackBytesToBits(const ByteVector& byteVec);

        LIBCOMPRA_API ByteVector PackBitsToBytes(const std::string& bitString, size_t& bitLength);

        LIBCOMPRA_API std::string UnpackBytesToBits(const ByteVector& byteVec, size_t bitLength);
    }

    LIBCOMPRA_API Compressed compress(const std::string& input);

    LIBCOMPRA_API std::string decompress(const Compressed& compressed);

    LIBCOMPRA_API std::string decompress(const ByteVector& encoded, const std::map<char, EncodedSymbol>& encodingTable, size_t bitLength);

    namespace Stringize {
        LIBCOMPRA_API std::string StringizeEncodingTable(const FSE::EncodingTable& freqMap);

        LIBCOMPRA_API std::string StringizeByteVec(const FSE::ByteVector& byteVec);
    }
}

namespace Zstandard {
    LIBCOMPRA_API FSE::Compressed compress(const std::string& input, size_t windowSize = 32 * 1024);

    LIBCOMPRA_API std::string decompress(const FSE::ByteVector& byteVec, const FSE::EncodingTable& encodingTable, const size_t bitLength);

    LIBCOMPRA_API std::string decompress(const FSE::Compressed& compressed);
}
} // Compra

#endif /* LIBCOMPRA_H */