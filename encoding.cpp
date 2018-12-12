/*
 * encoding.cpp
 * Haseeb Khan
 * This file implements functions from encoding.h header.
 */

#include "encoding.h"

#include "pqueue.h"
#include "filelib.h"
#include "HuffmanNode.h"

// Function prototypes
void buildCode(HuffmanNode* node, Map<int, string> &encodingMap, string code = "");
void writeCode(const string& code, obitstream& output);
bool isLeaf(HuffmanNode* node);

// This function reads input from a given istream
// and builds frequency table for all characters in the file
Map<int, int> buildFrequencyTable(istream& input)
{
    // create empty map
    Map<int, int> freqTable;
    int key;
    // read input until EOF
    while((key = input.get()) != EOF)
    {
        // get value for current key from the map
        int value = freqTable.get(key);
        // put key back to the map with value increased by 1
        freqTable.put(key, value + 1);
    }
    // finally add PSEUDO_EOF key
    freqTable.put(PSEUDO_EOF, 1);
    return freqTable;
}

// Accepts a frequency table and use it to create a Huffman encoding tree
// based on those frequencies.
// Return a pointer to the node representing the root of the tree.
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable)
{
    if (freqTable.isEmpty())
        return nullptr;

    // create empty priority queue of HuffmanNode pointers
    PriorityQueue<HuffmanNode*> pq;

    // for each key in tablle
    for (const auto key: freqTable)
    {
        int value = freqTable.get(key);
        // create new HuffmanNode
        HuffmanNode *huffNode = new HuffmanNode(key, value);
        // add it to queue with priority equal to frequency
        pq.add(huffNode, value);
    }

    // while queue size greater than one
    while (pq.size() > 1)
    {
        // dequeue first two nodes
        HuffmanNode *zero = pq.dequeue();
        HuffmanNode *one = pq.dequeue();
        // add count of two nodes
        int count = zero->count + one->count;
        // create new HuffmanNode with sum count and left and right children
        HuffmanNode *huffNode = new HuffmanNode(NOT_A_CHAR, count, zero, one);
        // enqueue new node
        pq.add(huffNode, count);
    }

    // now queue has one last node
    // that is root of Huffman Tree
    // Dequeue and return it
    return pq.dequeue();
}

// Accepts a pointer to the root node of a Huffman tree
// and use it to create and return a Huffman encoding map based on the tree's structure.
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree)
{
    Map<int, string> encodingMap;
    if (encodingTree != nullptr)
    {
        // recursively build codes for each leaf node
        buildCode(encodingTree, encodingMap);
    }
    return encodingMap;
}

// Reads one character at a time from a given input file,
// and use the provided encoding map to encode each character to binary,
// then write the character's encoded binary bits to the given bit output bit stream.
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output)
{
    int key;
    // read input until EOF
    while((key = input.get()) != EOF)
    {
        // get string represented code for current character from the encoding map
        string code = encodingMap.get(key);
        writeCode(code, output);

    }
    // write PSEUDO_EOF code at the end
    string code = encodingMap.get(PSEUDO_EOF);
    writeCode(code, output);
}

// Reads bits from the given input file one at a time,
// and recursively walk through the specified decoding tree to write
// the original uncompressed contents of that file to the given output stream.
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output)
{
    int bit;
    HuffmanNode* curr = encodingTree; // current position in the tree
    while (true)
    {
        bit = input.readBit(); // read one bit

        // move on the tree, depending on the bit value
        if (bit == 0)
            curr = curr->zero;
        else if (bit == 1)
            curr = curr->one;

        // if leaf is reached
        if (isLeaf(curr))
        {
            // each leaf contains real character
            int ch = curr->character;

            // break if PSEUDO_EOF is reached
            if (ch == PSEUDO_EOF)
                break;

            // write character to output
            output << static_cast<char>(ch);
            // reset tree for next traversal
            curr = encodingTree;
        }
    }
}

// Compresses the given input file into the given output file.
void compress(istream& input, obitstream& output)
{
    // Create frequency table from input file
    Map<int, int> freqTable = buildFrequencyTable(input);
    // write it to output
    output << freqTable;
    // rewind stream
    rewindStream(input);
    // build encoding tree from frequenncy table
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    // build encoding map from encoding tree
    Map<int, string> encodingMap = buildEncodingMap(encodingTree);
    // destroy tree
    freeTree(encodingTree);

    // encode data
    encodeData(input, encodingMap, output);
}

// Read the bits from the given input file one at a time,
// including header packed inside the start of the file,
// to write the original contents of that file to the file specified by the output parameter.
void decompress(ibitstream& input, ostream& output)
{
    // create empty frequenncy table
    Map<int, int> freqTable;
    // read table from input file
    input >> freqTable;
    // build encoding tree from frequenncy table
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    // decode data
    decodeData(input, encodingTree, output);
    // destroy tree
    freeTree(encodingTree);
}


// Frees the memory associated with the tree whose root node is represented by the given pointer.
void freeTree(HuffmanNode* node)
{
    // recursively delete all nodes
    if (node != nullptr)
    {
        freeTree(node->zero);
        freeTree(node->one);
        delete node;
    }
}

// Recursive function to search leaf nodes.
// When a leaf is found, this function adds binary string
// that was built when the tree was traversed to encodingMap.
void buildCode(HuffmanNode *node, Map<int, string> &encodingMap, string code)
{
    if (isLeaf(node)) // leaf node
    {
        encodingMap.add(node->character, code);
        return;
    }

    // recursive traversal

    if (node->zero != nullptr) // add 0 if move to left
        buildCode(node->zero, encodingMap, code + "0");

    if (node->one != nullptr) // add 1 if move to right
        buildCode(node->one, encodingMap, code + "1");
}

// Writes binary string code to the bit stream
void writeCode(const string &code, obitstream &output)
{
    // convert each digit to integer and write it to the bit stream
    for (const auto &c: code)
    {
        output.writeBit(static_cast<int>(c - '0'));
    }
}

// Returns true if node is leaf node
bool isLeaf(HuffmanNode *node)
{
    if (node->zero == nullptr && node->one == nullptr)
        return true;
    return false;
}
