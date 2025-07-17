/*
Name: Abhishek Singh (624)
Course Number: CS 51520  Operating Systems
Assignment: Project HW for Memory Management
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

using namespace std;

// Constants
const int PAGE_COUNT = 256;
const int TLB_ENTRIES = 16;
const int FRAME_BYTES = 256;
const int TOTAL_FRAMES = 256;
const int PHYSICAL_MEM_SIZE = FRAME_BYTES * TOTAL_FRAMES;
const int ADDR_MASK = 0xFFFF; // Mask to get 16 bits

// Structure for the translation lookaside buffer
struct TLBRecord {
    int virtualPage;
    int physicalFrame;
};

// Helper to write to both console and file
void logOutput(ofstream &dest, const string &message) {
    cout << message << endl;
    dest << message << endl;
}

int main() {    // Files
    ifstream virtualAddrFile("addresses.txt");
    ifstream diskStorage("BACKING_STORE.bin", ios::in | ios::binary);
    ofstream resultFile("sample_output.txt");    if (!virtualAddrFile.is_open() || !diskStorage.is_open() || !resultFile.is_open()) {
        cerr << "Error opening addresses.txt, BACKING_STORE.bin, or sample_output.txt" << endl;
        return 1;
    }

    // Memory
    signed char mainMemory[PHYSICAL_MEM_SIZE];

    // Page table
    int pageDirectory[PAGE_COUNT];
    memset(pageDirectory, -1, sizeof(pageDirectory)); // -1 means invalid

    // TLB
    TLBRecord tlbCache[TLB_ENTRIES];
    int tlbPosition = 0;

    // Stats
    int processedAddresses = 0;
    int pageMisses = 0;
    int tlbSuccesses = 0;

    int availableFrame = 0;

    // Address translation
    string inputLine;
    while (getline(virtualAddrFile, inputLine)) {
        int virtualAddress = stoi(inputLine);

        // Mask to get lower 16 bits. The lower 16 form the page number and offset values
        virtualAddress = virtualAddress & ADDR_MASK;

        int pageId = (virtualAddress >> 8) & 0xFF;
        int byteOffset = virtualAddress & 0xFF;

        int frameId = -1;
        bool tlbFound = false;

        // Check TLB first
        for (int index = 0; index < TLB_ENTRIES; ++index) {
            if (tlbCache[index].virtualPage == pageId) {
                frameId = tlbCache[index].physicalFrame;
                tlbSuccesses++;
                tlbFound = true;
                break;
            }
        }

        // If not in TLB, check page table
        //When there is a page fault, check the backing store and get the required page using Page number
        if (!tlbFound) {
            if (pageDirectory[pageId] != -1) {
                frameId = pageDirectory[pageId];
            } else {
                // Page fault
                pageMisses++;

                // Seek to page in backing store
                diskStorage.seekg(pageId * FRAME_BYTES);
                diskStorage.read(reinterpret_cast<char*>(&mainMemory[availableFrame * FRAME_BYTES]), FRAME_BYTES);

                frameId = availableFrame;
                pageDirectory[pageId] = frameId;

                availableFrame++;
                if (availableFrame >= TOTAL_FRAMES) {
                    availableFrame = 0; // Simple wrap around (FIFO)
                }
            }

            // Update TLB (FIFO replacement)
            tlbCache[tlbPosition % TLB_ENTRIES].virtualPage = pageId;
            tlbCache[tlbPosition % TLB_ENTRIES].physicalFrame = frameId;
            tlbPosition++;
        }

        int realAddress = (frameId * FRAME_BYTES) + byteOffset;
        int storedValue = mainMemory[realAddress];

        // Prepare output text
        stringstream resultLine;
        resultLine << "Virtual Address: " << virtualAddress
                   << " Physical Address: " << realAddress
                   << " Value: " << static_cast<int>(storedValue);

        logOutput(resultFile, resultLine.str());

        processedAddresses++;
    }

    // Final Stats
    logOutput(resultFile, "");
    logOutput(resultFile, "Total Translated Addresses = " + to_string(processedAddresses));
    logOutput(resultFile, "Page Faults = " + to_string(pageMisses));
    logOutput(resultFile, "Page Fault Rate = " + 
                to_string((static_cast<double>(pageMisses) / processedAddresses)));
    logOutput(resultFile, "TLB Hits = " + to_string(tlbSuccesses));
    logOutput(resultFile, "TLB Hit Rate = " + 
                to_string((static_cast<double>(tlbSuccesses) / processedAddresses)));

    // Close files
    virtualAddrFile.close();
    diskStorage.close();
    resultFile.close();

    return 0;
}


