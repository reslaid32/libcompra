#define LIBCOMPRA_LIBRARY_BUILD

#include <compra/compra.h>

#include <algorithm>
#include <sstream>
#include <cmath>

namespace LIBCOMPRA_NAMESPACE {
namespace LZ77 {
    LIBCOMPRA_API size_t findLongestMatch(const std::string& input, size_t i, size_t searchStart, size_t windowSize, size_t& bestOffset) {
        size_t bestLength = 0;

        for (size_t j = searchStart; j < i; ++j) {
            size_t length = 0;
            while (i + length < input.size() && input[j + length] == input[i + length]) {
                ++length;
                if (length > bestLength) {
                    bestOffset = i - j;
                    bestLength = length;
                }
            }
        }
        return bestLength;
    }

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t windowSize) {
        std::vector<Token> tokens;
        size_t i = 0;

        while (i < input.size()) {
            size_t bestOffset = 0;
            size_t bestLength = findLongestMatch(input, i, (i < windowSize) ? 0 : i - windowSize, windowSize, bestOffset);

            char nextChar = (i + bestLength < input.size()) ? input[i + bestLength] : '\0';
            tokens.push_back({bestOffset, bestLength, nextChar});
            i += bestLength + 1;
        }

        return tokens;
    }

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens) {
        std::string output;

        for (const auto& token : tokens) {
            size_t start = output.size() - token.offset;
            for (size_t i = 0; i < token.length; ++i)
                output += output[start + i];

            if (token.next != '\0')
                output += token.next;
        }

        return output;
    }

    namespace Utils {
        LIBCOMPRA_API std::string serializeToken(const Token& token) {
            return std::to_string(token.offset) + "," + std::to_string(token.length) + "," + token.next;
        }

        LIBCOMPRA_API std::string vectorToString(const std::vector<Token>& tokens) {
            std::string result;
            for (const auto& token : tokens) {
                result += serializeToken(token) + ";";
            }
            if (!result.empty()) {
                result.pop_back();
            }
            return result;
        }

        LIBCOMPRA_API Token deserializeToken(const std::string& str) {
            size_t firstComma = str.find(',');
            size_t secondComma = str.find(',', firstComma + 1);
            size_t offset = std::stoull(str.substr(0, firstComma));
            size_t length = std::stoull(str.substr(firstComma + 1, secondComma - firstComma - 1));
            char next = str[secondComma + 1];
            return {offset, length, next};
        }

        LIBCOMPRA_API std::vector<Token> stringToVector(const std::string& str) {
            std::vector<Token> tokens;
            size_t start = 0;
            size_t end;
            while ((end = str.find(';', start)) != std::string::npos) {
                tokens.push_back(deserializeToken(str.substr(start, end - start)));
                start = end + 1;
            }
            if (start < str.size()) {
                tokens.push_back(deserializeToken(str.substr(start)));
            }
            return tokens;
        }
    }
}

namespace LZ78 {
    LIBCOMPRA_API size_t findPrefixIndex(const std::map<std::string, size_t>& dictionary, const std::string& buffer) {
        if (buffer.size() > 1) {
            auto prefix = dictionary.find(buffer.substr(0, buffer.size() - 1));
            if (prefix != dictionary.end()) {
                return prefix->second;
            }
        }
        return 0;
    }

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input) {
        std::map<std::string, size_t> dictionary;
        std::vector<Token> tokens;
        std::string buffer;
        size_t dictSize = 1;

        for (char c : input) {
            buffer += c;
            if (dictionary.find(buffer) == dictionary.end()) {
                size_t prefixIndex = findPrefixIndex(dictionary, buffer);
                tokens.push_back({prefixIndex, c});
                dictionary[buffer] = dictSize++;
                buffer.clear();
            }
        }

        if (!buffer.empty()) {
            size_t prefixIndex = findPrefixIndex(dictionary, buffer);
            tokens.push_back(Token{prefixIndex, buffer.back()});
        }

        return tokens;
    }

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens) {
        std::vector<std::string> dictionary(1, "");
        std::string output;

        for (const auto& token : tokens) {
            std::string entry = (token.index < dictionary.size()) ? dictionary[token.index] : "";
            entry += token.next;
            dictionary.push_back(entry);
            output += entry;
        }

        return output;
    }
}

namespace LZMA {
    LIBCOMPRA_API size_t findLongestMatch(const std::string& input, size_t pos, size_t dictionarySize, size_t& matchPos) {
        size_t maxLength = 0;
        matchPos = 0;
        
        for (size_t i = (pos > dictionarySize ? pos - dictionarySize : 0); i < pos; ++i) {
            size_t length = 0;
            while (length < dictionarySize && pos + length < input.size() && input[i + length] == input[pos + length]) {
                ++length;
            }
            if (length > maxLength) {
                maxLength = length;
                matchPos = i;
            }
        }
        return maxLength;
    }

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t dictionarySize) {
        std::vector<Token> tokens;
        size_t inputSize = input.size();
        size_t pos = 0;

        while (pos < inputSize) {
            size_t matchLength = 0;
            size_t matchPos = 0;

            matchLength = findLongestMatch(input, pos, dictionarySize, matchPos);

            if (matchLength > 0) {
                tokens.push_back({pos - matchPos, matchLength, input[pos + matchLength]});
                pos += matchLength + 1;
            } else {
                tokens.push_back({0, 0, input[pos]});
                ++pos;
            }
        }

        return tokens;
    }

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens, size_t dictionarySize) {
        std::string output;

        for (const auto& token : tokens) {
            if (token.position > 0 && token.position <= output.size()) {
                size_t start = output.size() - token.position;
                for (size_t i = 0; i < token.length; ++i) {
                    output += output[start + i];
                }
            }
            if (token.next != '\0') {
                output += token.next;
            }
        }

        return output;
    }
}

namespace Huffman {
    LIBCOMPRA_API HuffmanNode::HuffmanNode(char data, int freq) {
        this->data = data;
        this->freq = freq;
        left = right = nullptr;
    }

    LIBCOMPRA_API bool HuffmanCompare::operator()(HuffmanNode* left, HuffmanNode* right) {
        return left->freq > right->freq;
    }

    namespace Methods {
        LIBCOMPRA_API HuffmanNode* BuildHuffmanTree(const Huffman::FreqMap& freqMap) {
            std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanCompare> pq;

            for (const auto& pair : freqMap) {
                pq.push(new HuffmanNode(pair.first, pair.second));
            }

            while (pq.size() != 1) {
                HuffmanNode *left = pq.top(); pq.pop();
                HuffmanNode *right = pq.top(); pq.pop();

                int sum = left->freq + right->freq;
                HuffmanNode* node = new HuffmanNode('\0', sum);
                node->left = left;
                node->right = right;
                pq.push(node);
            }

            return pq.top();
        }

        LIBCOMPRA_API void GenerateCodes(HuffmanNode* node, const std::string& code, std::map<char, std::string>& huffmanCode) {
            if (!node) return;

            if (!node->left && !node->right) {
                huffmanCode[node->data] = code;
            }

            GenerateCodes(node->left, code + '0', huffmanCode);
            GenerateCodes(node->right, code + '1', huffmanCode);
        }

        LIBCOMPRA_API void FreeTree(HuffmanNode* node) {
            if (node == nullptr) return;
            FreeTree(node->left);
            FreeTree(node->right);
            delete node;
        }

        LIBCOMPRA_API Huffman::ByteVector PackBitsToBytes(const std::string& bitString) {
            Huffman::ByteVector compressedData;
            Huffman::Byte currentByte = 0;
            int bitCount = 0;


            for (char bit : bitString) {
                currentByte <<= 1;
                if (bit == '1') {
                    currentByte |= 1;
                }
                bitCount++;

                if (bitCount == 8) {
                    compressedData.push_back(currentByte);
                    currentByte = 0;
                    bitCount = 0;
                }
            }

            if (bitCount > 0) {
                currentByte <<= (8 - bitCount);
                compressedData.push_back(currentByte);
            }

            return compressedData;
        }

        LIBCOMPRA_API std::string UnpackBytesToBits(const Huffman::ByteVector& compressedData) {
            std::string bitString;

            for (Huffman::Byte byte : compressedData) {
                for (int i = 7; i >= 0; --i) {
                    bitString += (byte & (1 << i)) ? '1' : '0';
                }
            }

            return bitString;
        }

        LIBCOMPRA_API Huffman::ByteVector PackBitsToBytes(const std::string& bitString, size_t& bitLength) {
            Huffman::ByteVector compressedData;
            Huffman::Byte currentByte = 0;
            int bitCount = 0;

            bitLength = bitString.size();

            for (char bit : bitString) {
                currentByte <<= 1;
                if (bit == '1') {
                    currentByte |= 1;
                }
                bitCount++;

                if (bitCount == 8) {
                    compressedData.push_back(currentByte);
                    currentByte = 0;
                    bitCount = 0;
                }
            }

            if (bitCount > 0) {
                currentByte <<= (8 - bitCount);
                compressedData.push_back(currentByte);
            }

            return compressedData;
        }

        LIBCOMPRA_API std::string UnpackBytesToBits(const Huffman::ByteVector& compressedData, size_t bitLength) {
            std::string bitString;

            for (Huffman::Byte byte : compressedData) {
                for (int i = 7; i >= 0; --i) {
                    bitString += (byte & (1 << i)) ? '1' : '0';
                }
            }

            return bitString.substr(0, bitLength);
        }
    }

    LIBCOMPRA_API Compressed compress(const std::string& text) {
        Compressed compressed;

        for (char ch : text) {
            compressed.freqMap[ch]++;
        }

        HuffmanNode* root = Methods::BuildHuffmanTree(compressed.freqMap);
        
        std::map<char, std::string> huffmanCode;
        Methods::GenerateCodes(root, std::string(), huffmanCode);

        std::string bitString;
        for (char ch : text) {
            bitString += huffmanCode[ch];
        }

        compressed.byteVec = Methods::PackBitsToBytes(bitString, compressed.bitLength);
        Methods::FreeTree(root);

        return compressed;
    }

    LIBCOMPRA_API std::string decompress(const ByteVector& compressed, const FreqMap& freqMap, size_t bitLength) {
        HuffmanNode* root = Methods::BuildHuffmanTree(freqMap);
        std::string result;

        std::string bitString = Methods::UnpackBytesToBits(compressed, bitLength);
        HuffmanNode* current = root;
        for (char bit : bitString) {
            current = (bit == '0') ? current->left : current->right;

            if (!current->left && !current->right) {
                result += current->data;
                current = root;
            }
        }

        Methods::FreeTree(root);
        return result;
    }

    LIBCOMPRA_API std::string decompress(const Compressed& compressed) {
        HuffmanNode* root = Methods::BuildHuffmanTree(compressed.freqMap);
        std::string result;
        
        std::string bitString = Methods::UnpackBytesToBits(compressed.byteVec, compressed.bitLength);
        HuffmanNode* current = root;
        for (char bit : bitString) {
            current = (bit == '0') ? current->left : current->right;

            if (!current->left && !current->right) {
                result += current->data;
                current = root;
            }
        }

        Methods::FreeTree(root);
        return result;
    }

    /* COMPATIBILITY */

    LIBCOMPRA_API Huffman::ByteVector compress(const std::string& text, FreqMap& freqMap, size_t& bitLength) {
        for (char ch : text) {
            freqMap[ch]++;
        }

        HuffmanNode* root = Methods::BuildHuffmanTree(freqMap);
        
        std::map<char, std::string> huffmanCode;
        Methods::GenerateCodes(root, std::string(), huffmanCode);

        std::string bitString;
        for (char ch : text) {
            bitString += huffmanCode[ch];
        }

        Huffman::ByteVector compressed = Methods::PackBitsToBytes(bitString, bitLength);
        Methods::FreeTree(root);
        return compressed;
    }

    LIBCOMPRA_API Huffman::ByteVector Compress(const std::string& text, FreqMap& freqMap, size_t& bitLength) {
        return compress(text, freqMap, bitLength);
    }

    LIBCOMPRA_API std::string Decompress(const ByteVector& compressed, const FreqMap& freqMap, size_t bitLength) {
        return decompress(compressed, freqMap, bitLength);
    }

    namespace Stringize {
        LIBCOMPRA_API std::string StringizeFreqMap(const Huffman::FreqMap& freqMap) {
            std::stringstream ss;

            for (const auto& pair : freqMap) {
                ss << "'" << pair.first << "'" << ":" << "'" << pair.second << "'" << " "; 
            }

            return ss.str();
        }

        LIBCOMPRA_API std::string StringizeByteVec(const Huffman::ByteVector& byteVec) {
            std::stringstream ss;

            for (size_t i = 0; i < byteVec.size(); ++i) {
                ss << byteVec[i];
            }

            return ss.str();
        }
    }
}

namespace Deflate {
    LIBCOMPRA_API Huffman::Compressed compress(const std::string& input, size_t windowSize) {
        std::vector<LZ77::Token> lz77Tokens = LZ77::compress(input, windowSize);

        std::string lz77Compressed = LZ77::Utils::vectorToString(lz77Tokens);

        auto [byteVec, freqMap, bitLength] = Huffman::compress(lz77Compressed);

        return {byteVec, freqMap, bitLength};
    }

    LIBCOMPRA_API std::string decompress(const Huffman::ByteVector& byteVec, const Huffman::FreqMap& freqMap, const size_t bitLength) {
        std::string decodedData = Huffman::decompress(byteVec, freqMap, bitLength);

        std::vector<LZ77::Token> lz77Tokens = LZ77::Utils::stringToVector(decodedData);

        return LZ77::decompress(lz77Tokens);
    }

    LIBCOMPRA_API std::string decompress(const Huffman::Compressed& compressed) {
        std::string decodedData = Huffman::decompress(compressed.byteVec, compressed.freqMap, compressed.bitLength);

        std::vector<LZ77::Token> lz77Tokens = LZ77::Utils::stringToVector(decodedData);

        return LZ77::decompress(lz77Tokens);
    }
}

namespace LZ4 {
    LIBCOMPRA_API size_t findLongestMatch(const std::string& input, size_t i, size_t& match_offset) {
        size_t match_length = 0;

        size_t window_start = std::max(i, size_t(16)) - 16;
        for (size_t j = window_start; j < i; ++j) {
            size_t k = 0;
            while (k < 255 && j + k < i && i + k < input.size() && input[j + k] == input[i + k]) {
                ++k;
            }
            if (k > match_length) {
                match_length = k;
                match_offset = i - j;
            }
        }

        return match_length;
    }


    LIBCOMPRA_API std::string compress(const std::string& input) {
        std::string output;
        size_t length = input.size();
        size_t i = 0;

        while (i < length) {
            size_t match_offset = 0;
            size_t match_length = findLongestMatch(input, i, match_offset);

            if (match_length > 3) {
                output += (char)match_offset;
                output += (char)match_length;
                i += match_length;
            } else {
                output += input[i];
                ++i;
            }
        }

        return output;
    }

    LIBCOMPRA_API std::string decompress(const std::string& input) {
        std::string output;
        size_t length = input.size();

        for (size_t i = 0; i < length;) {
            if (i + 1 < length && (unsigned char)input[i] < 16) {
                size_t offset = (unsigned char)input[i];
                size_t match_length = (unsigned char)input[i + 1];
                size_t start = output.size() - offset;
                for (size_t j = 0; j < match_length; ++j) {
                    output += output[start + j];
                }
                i += 2;
            } else {
                output += input[i];
                ++i;
            }
        }

        return output;
    }
}

namespace LZ5 {
    LIBCOMPRA_API size_t findLongestMatch(const std::string& input, size_t pos, size_t maxOffset, size_t& matchOffset) {
        size_t bestLength = 0;

        size_t searchStart = (pos > maxOffset) ? pos - maxOffset : 0;
        for (size_t i = searchStart; i < pos; ++i) {
            size_t length = 0;
            while (length < maxOffset && pos + length < input.size() && input[i + length] == input[pos + length]) {
                ++length;
            }

            if (length > bestLength) {
                bestLength = length;
                matchOffset = pos - i;
            }
        }

        return bestLength;
    }

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t maxOffset) {
        std::vector<Token> tokens;
        size_t inputSize = input.size();
        size_t pos = 0;

        while (pos < inputSize) {
            size_t matchOffset = 0;
            size_t matchLength = findLongestMatch(input, pos, maxOffset, matchOffset);

            if (matchLength > 3) {
                tokens.push_back({matchOffset, matchLength, input[pos + matchLength]});
                pos += matchLength + 1;
            } else {
                tokens.push_back({0, 0, input[pos]});
                ++pos;
            }
        }

        return tokens;
    }

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens) {
        std::string output;

        for (const auto& token : tokens) {
            size_t start = output.size() - token.offset;
            for (size_t i = 0; i < token.length; ++i) {
                output += output[start + i];
            }

            if (token.next != '\0') {
                output += token.next;
            }
        }

        return output;
    }
}

namespace LZW {
    LIBCOMPRA_API void addToDictionary(std::map<std::string, int>& dictionary, const std::string& key, int& code) {
        dictionary[key] = code++;
    }

    LIBCOMPRA_API void compress(std::vector<int>& result, std::map<std::string, int>& dictionary, const std::string& input, int& code) {
        std::string current;
        for (char c : input) {
            std::string next = current + c;
            if (dictionary.count(next)) {
                current = next;
            } else {
                result.push_back(dictionary[current]);
                addToDictionary(dictionary, next, code);
                current = std::string(1, c);
            }
        }

        if (!current.empty()) {
            result.push_back(dictionary[current]);
        }
    }

    LIBCOMPRA_API std::vector<int> compress(const std::string& input) {
        std::vector<int> result;
        std::map<std::string, int> dictionary;

        for (int i = 0; i < 256; ++i) {
            dictionary[std::string(1, i)] = i;
        }

        int code = 256;
        compress(result, dictionary, input, code);

        return result;
    }

    LIBCOMPRA_API void decompress(std::string& result, std::map<int, std::string>& dictionary, const std::vector<int>& input, int& code) {
        std::string current = dictionary[input[0]];
        result = current;

        for (size_t i = 1; i < input.size(); ++i) {
            std::string entry;
            if (dictionary.count(input[i])) {
                entry = dictionary[input[i]];
            } else if (input[i] == code) {
                entry = current + current[0];
            } else {
                throw std::runtime_error("Invalid LZW decompression input");
            }

            result += entry;
            dictionary[code++] = current + entry[0];
            current = entry;
        }
    }

    LIBCOMPRA_API std::string decompress(const std::vector<int>& input) {
        std::map<int, std::string> dictionary;

        for (int i = 0; i < 256; ++i) {
            dictionary[i] = std::string(1, i);
        }

        int code = 256;
        std::string result;
        decompress(result, dictionary, input, code);

        return result;
    }

    LIBCOMPRA_API void compressOptimized(std::vector<int>& result, std::map<std::string, int>& dictionary, const std::string& input, int& code) {
        std::string current;
        for (char c : input) {
            current += c;
            if (dictionary.count(current) == 0) {
                result.push_back(dictionary[current.substr(0, current.size() - 1)]);
                addToDictionary(dictionary, current, code);
                current = std::string(1, c);
            }
        }

        if (!current.empty()) {
            result.push_back(dictionary[current]);
        }
    }

    LIBCOMPRA_API std::vector<int> compressOptimized(const std::string& input) {
        std::vector<int> result;
        std::map<std::string, int> dictionary;

        for (int i = 0; i < 256; ++i) {
            dictionary[std::string(1, i)] = i;
        }

        int code = 256;
        compressOptimized(result, dictionary, input, code);

        return result;
    }

    LIBCOMPRA_API void decompressOptimized(std::string& result, std::map<int, std::string>& dictionary, const std::vector<int>& input, int& code) {
        std::string current = dictionary[input[0]];
        result = current;

        for (size_t i = 1; i < input.size(); ++i) {
            std::string entry;
            if (dictionary.count(input[i])) {
                entry = dictionary[input[i]];
            } else if (input[i] == code) {
                entry = current + current[0];
            } else {
                throw std::runtime_error("Invalid LZW decompression input");
            }

            result += entry;
            dictionary[code++] = current + entry[0];
            current = entry;
        }
    }

    LIBCOMPRA_API std::string decompressOptimized(const std::vector<int>& input) {
        std::map<int, std::string> dictionary;

        for (int i = 0; i < 256; ++i) {
            dictionary[i] = std::string(1, i);
        }

        int code = 256;
        std::string result;
        decompressOptimized(result, dictionary, input, code);

        return result;
    }
}

namespace LZO {
    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t windowSize) {
        std::vector<Token> tokens;
        size_t i = 0;

        while (i < input.size()) {
            size_t bestOffset = 0, bestLength = 0;
            size_t searchStart = (i > windowSize) ? i - windowSize : 0;

            for (size_t j = searchStart; j < i; ++j) {
                size_t length = 0;
                while (j + length < i && i + length < input.size() && input[j + length] == input[i + length]) {
                    ++length;
                }

                if (length > bestLength) {
                    bestLength = length;
                    bestOffset = i - j;
                }
            }

            char nextChar = (i + bestLength < input.size()) ? input[i + bestLength] : '\0';
            tokens.push_back({bestOffset, bestLength, nextChar});
            i += (bestLength > 0) ? bestLength + 1 : 1;
        }

        return tokens;
    }

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens) {
        std::string output;

        for (const auto& token : tokens) {
            if (token.offset > 0 && token.length > 0) {
                size_t start = output.size() - token.offset;

                for (size_t i = 0; i < token.length; ++i) {
                    output += output[start + i];
                }
            }

            if (token.next != '\0') {
                output += token.next;
            }
        }

        return output;
    }
}

namespace LZSS {
    LIBCOMPRA_API size_t findBestMatch(const std::string& input, size_t windowStart, size_t currentPos, size_t windowSize, size_t& bestOffset) {
        size_t bestLength = 0;
        bestOffset = 0;

        for (size_t j = windowStart; j < currentPos; ++j) {
            size_t length = 0;
            while (j + length < currentPos && currentPos + length < input.size() && input[j + length] == input[currentPos + length]) {
                ++length;
                if (length >= windowSize) break;
            }

            if (length > bestLength) {
                bestLength = length;
                bestOffset = currentPos - j;
            }
        }

        return bestLength;
    }

    LIBCOMPRA_API void addToken(std::vector<Token>& tokens, bool isLiteral, char literal, size_t offset, size_t length) {
        tokens.push_back({isLiteral, literal, offset, length});
    }

    LIBCOMPRA_API std::vector<Token> compress(const std::string& input, size_t windowSize, size_t lookaheadSize) {
        std::vector<Token> tokens;
        size_t inputSize = input.size();
        size_t i = 0;

        while (i < inputSize) {
            size_t bestOffset = 0;
            size_t bestLength = findBestMatch(input, std::max(i - windowSize, size_t(0)), i, windowSize, bestOffset);

            if (bestLength >= 3) {
                addToken(tokens, false, '\0', bestOffset, bestLength);
                i += bestLength;
            } else {
                addToken(tokens, true, input[i], 0, 0);
                ++i;
            }
        }

        return tokens;
    }

    LIBCOMPRA_API std::string decompress(const std::vector<Token>& tokens) {
        std::string output;

        for (const auto& token : tokens) {
            if (token.isLiteral) {
                output += token.literal;
            } else {
                size_t start = output.size() - token.offset;
                for (size_t i = 0; i < token.length; ++i) {
                    output += output[start + i];
                }
            }
        }

        return output;
    }
}

namespace FSE {
    namespace Methods {
        LIBCOMPRA_API EncodingTable buildEncodingTable(const std::string& input) {
            FrequencyTable frequencyTable;
            for (char c : input) {
                ++frequencyTable[c];
            }

            std::vector<Symbol> symbols;
            for (const auto& [character, frequency] : frequencyTable) {
                symbols.push_back({character, frequency});
            }

            std::sort(symbols.begin(), symbols.end(), [](const Symbol& a, const Symbol& b) {
                return a.frequency > b.frequency;
            });

            std::map<char, EncodedSymbol> encodingTable;
            for (size_t i = 0; i < symbols.size(); ++i) {
                std::string binaryCode;
                for (size_t bit = 0; bit < std::ceil(std::log2(symbols.size())); ++bit) {
                    binaryCode += ((i >> bit) & 1) ? '1' : '0';
                }
                encodingTable[symbols[i].character] = {symbols[i].character, binaryCode};
            }

            return encodingTable;
        }

        LIBCOMPRA_API ByteVector PackBitsToBytes(const std::string& bitString) {
            ByteVector byteVec;
            uint8_t currentByte = 0;
            size_t bitCount = 0;

            for (char bit : bitString) {
                currentByte = (currentByte << 1) | (bit == '1' ? 1 : 0);
                ++bitCount;

                if (bitCount == 8) {
                    byteVec.push_back(currentByte);
                    currentByte = 0;
                    bitCount = 0;
                }
            }

            if (bitCount > 0) {
                currentByte <<= (8 - bitCount);
                byteVec.push_back(currentByte);
            }

            return byteVec;
        }

        LIBCOMPRA_API std::string UnpackBytesToBits(const ByteVector& byteVec) {
            std::string bitString;

            for (uint8_t byte : byteVec) {
                for (int i = 7; i >= 0; --i) {
                    bitString += (byte & (1 << i)) ? '1' : '0';
                }
            }

            return bitString;
        }

        LIBCOMPRA_API ByteVector PackBitsToBytes(const std::string& bitString, size_t& bitLength) {
            ByteVector byteVec;
            uint8_t currentByte = 0;
            size_t bitCount = 0;

            bitLength = bitString.size();

            for (char bit : bitString) {
                currentByte = (currentByte << 1) | (bit == '1' ? 1 : 0);
                ++bitCount;

                if (bitCount == 8) {
                    byteVec.push_back(currentByte);
                    currentByte = 0;
                    bitCount = 0;
                }
            }

            if (bitCount > 0) {
                currentByte <<= (8 - bitCount);
                byteVec.push_back(currentByte);
            }

            return byteVec;
        }

        LIBCOMPRA_API std::string UnpackBytesToBits(const ByteVector& byteVec, size_t bitLength) {
            std::string bitString;

            for (uint8_t byte : byteVec) {
                for (int i = 7; i >= 0; --i) {
                    bitString += (byte & (1 << i)) ? '1' : '0';
                }
            }

            return bitString.substr(0, bitLength);
        }
    }

    LIBCOMPRA_API Compressed compress(const std::string& input) {
        auto encodingTable = Methods::buildEncodingTable(input);
        std::string encodedString;

        for (char c : input) {
            encodedString += encodingTable[c].code;
        }

        size_t bitLength = 0;
        ByteVector packedBits = Methods::PackBitsToBytes(encodedString, bitLength);

        return {packedBits, encodingTable, bitLength};
    }

    LIBCOMPRA_API std::string decompress(const Compressed& compressed) {
        std::map<std::string, char> reverseTable;
        for (const auto& [character, symbol] : compressed.encodingTable) {
            reverseTable[symbol.code] = character;
        }

        std::string unpackedBytes = Methods::UnpackBytesToBits(compressed.byteVec, compressed.bitLength);
        std::string decodedString;
        std::string currentCode;
        for (char bit : unpackedBytes) {
            currentCode += bit;
            if (reverseTable.find(currentCode) != reverseTable.end()) {
                decodedString += reverseTable[currentCode];
                currentCode.clear();
            }
        }

        return decodedString;
    }

    LIBCOMPRA_API std::string decompress(const ByteVector& encoded, const std::map<char, EncodedSymbol>& encodingTable, size_t bitLength) {
        std::map<std::string, char> reverseTable;
        for (const auto& [character, symbol] : encodingTable) {
            reverseTable[symbol.code] = character;
        }

        std::string unpackedBytes = Methods::UnpackBytesToBits(encoded, bitLength);
        std::string decodedString;
        std::string currentCode;
        for (char bit : unpackedBytes) {
            currentCode += bit;
            if (reverseTable.find(currentCode) != reverseTable.end()) {
                decodedString += reverseTable[currentCode];
                currentCode.clear();
            }
        }

        return decodedString;
    }

    namespace Stringize {
        LIBCOMPRA_API std::string StringizeEncodingTable(const FSE::EncodingTable& freqMap) {
            std::stringstream ss;

            for (const auto& pair : freqMap) {
                ss << "'" << pair.first << "'" << ":" << "'" << pair.second.character << "'" << " "; 
            }

            return ss.str();
        }

        LIBCOMPRA_API std::string StringizeByteVec(const FSE::ByteVector& byteVec) {
            std::stringstream ss;

            for (size_t i = 0; i < byteVec.size(); ++i) {
                ss << byteVec[i];
            }

            return ss.str();
        }
    }
}

namespace Zstandard {
    LIBCOMPRA_API FSE::Compressed compress(const std::string& input, size_t windowSize) {
        std::vector<LZ77::Token> lz77Tokens = LZ77::compress(input, windowSize);

        std::string lz77Compressed = LZ77::Utils::vectorToString(lz77Tokens);

        auto [byteVec, encodingTable, bitLength] = FSE::compress(lz77Compressed);

        return {byteVec, encodingTable, bitLength};
    }

    LIBCOMPRA_API std::string decompress(const FSE::ByteVector& byteVec, const FSE::EncodingTable& encodingTable, const size_t bitLength) {
        std::string decodedData = FSE::decompress(byteVec, encodingTable, bitLength);

        std::vector<LZ77::Token> lz77Tokens = LZ77::Utils::stringToVector(decodedData);

        return LZ77::decompress(lz77Tokens);
    }

    LIBCOMPRA_API std::string decompress(const FSE::Compressed& compressed) {
        std::string decodedData = FSE::decompress(compressed.byteVec, compressed.encodingTable, compressed.bitLength);

        std::vector<LZ77::Token> lz77Tokens = LZ77::Utils::stringToVector(decodedData);

        return LZ77::decompress(lz77Tokens);
    }
}
} // Compra