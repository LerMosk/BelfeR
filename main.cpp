#include <vector>
#include <bitset>
#include <iostream>

constexpr auto BYTE = 8;
constexpr auto BLOCKSIZE = 138;
constexpr auto PAYLOADSIZE = 128;

struct Block {
    Block() {}
    std::bitset<2 * BYTE> prevBlockAddress;
    std::bitset<2 * BYTE> nextBlockAddress;
    std::bitset<1 * BYTE> frameHeader;
    std::bitset<3 * BYTE> packetHeader;
    std::bitset<128 * BYTE> payload;
    std::bitset<2 * BYTE> checksum;
};

struct FrameRR {
    FrameRR() {}
    std::bitset<1 * BYTE> data;
    std::bitset<2 * BYTE> checksum;
};

struct FreeBlocksInfo {
    std::bitset<2 * BYTE> firstBlockAddress;
    std::bitset<2 * BYTE> lastBlockAddress;
    size_t size;
};

struct FilledBlocksInfo {
    std::bitset<2 * BYTE> firstBlockAddress;
    std::bitset<2 * BYTE> lastBlockAddress;
    size_t size;
};

struct RepeatBlocksInfo {
    std::bitset<2 * BYTE> firstBlockAddress;
    std::bitset<2 * BYTE> lastBlockAddress;
    size_t size;
};

struct KpmBlocksInfo {
    std::bitset<2 * BYTE> firstBlockAddress;
    std::bitset<2 * BYTE> lastBlockAddress;
    size_t size;
};

void p1(std::vector<Block> &q, size_t n1, FreeBlocksInfo& fbi) {
    for (size_t i = 0; i < n1; ++i) {
        Block block;

        if (i == 0)
            block.prevBlockAddress = std::bitset<2 * BYTE>(0);
        else
            block.prevBlockAddress = std::bitset<2 * BYTE>(i * BLOCKSIZE - BLOCKSIZE);

        if (i == n1 - 1)
            block.nextBlockAddress = std::bitset<2 * BYTE>(0);
        else
            block.nextBlockAddress = std::bitset<2 * BYTE>(i * BLOCKSIZE + BLOCKSIZE);

        q.push_back(block);
    }

    fbi.firstBlockAddress = std::bitset<2 * BYTE>(0);
    fbi.lastBlockAddress = std::bitset<2 * BYTE>(n1 * BLOCKSIZE - BLOCKSIZE);
    fbi.size = n1;
}

void p2(std::vector<Block>& q, size_t n2, int m) {
    for (size_t i = 0; i < n2; ++i) {
        std::string payloadTemp;
        for (size_t j = 0; j < PAYLOADSIZE; ++j) {
            payloadTemp += std::bitset<BYTE>(m + 1).to_string();
        }
        q.at(i).payload = std::bitset<128*BYTE>(payloadTemp);
        m++;
    }
}

void p3(std::vector<Block>& q, FreeBlocksInfo& fbi, FilledBlocksInfo& fibi, size_t n1, size_t n2) {
    fibi.firstBlockAddress = std::bitset<2 * BYTE>(0);
    fibi.lastBlockAddress = std::bitset<2 * BYTE>((n2 * BLOCKSIZE - BLOCKSIZE) * BYTE);
    fibi.size = n2;

    fbi.firstBlockAddress = std::bitset<2 * BYTE>(n2 * BLOCKSIZE * BYTE);
    fbi.lastBlockAddress = std::bitset<2 * BYTE>((n1 * BLOCKSIZE - BLOCKSIZE) * BYTE);
    fbi.size = n1 - n2;

    q.at(n2).prevBlockAddress = std::bitset<2 * BYTE>(0);
}

Block p4(std::vector<Block>& q, int z1, int z2) {
    Block informationFrame = q.at(0);

    informationFrame.frameHeader = std::bitset<BYTE>(
            std::bitset<3>(z2).to_string() +
            std::bitset<1>(0).to_string() +
            std::bitset<3>(z1).to_string() +
            std::bitset<1>(0).to_string()
    );

    std::bitset<BYTE> payloadPart;
    for (auto i = BYTE - 1; i >= 0; --i) {
        payloadPart[i] = informationFrame.payload[i];
    }
    std::bitset<BYTE> checksumLow = payloadPart ^ informationFrame.frameHeader;
    std::bitset<BYTE> checksumUpper;
    for (auto i = 0; i < PAYLOADSIZE; ++i) {
        checksumUpper ^= payloadPart;
    }
    informationFrame.checksum = std::bitset<BYTE * 2>(checksumUpper.to_string() + checksumLow.to_string());

    return informationFrame;
}

void p5(std::vector<Block>& rq, Block infFrame, RepeatBlocksInfo& rbi, FilledBlocksInfo &fibi) {
    rbi.firstBlockAddress = std::bitset<2 * BYTE>(0);
    rbi.lastBlockAddress = fibi.firstBlockAddress;
    rbi.size++;

    fibi.firstBlockAddress = std::bitset<2 * BYTE>(fibi.firstBlockAddress.to_ulong() + BLOCKSIZE);
    fibi.size--;

    rq.push_back(infFrame);
}

FrameRR p6(Block infFrame) {
    FrameRR rr;
    rr.data = std::bitset<BYTE>(
            std::bitset<1>(infFrame.frameHeader[3]).to_string() +
            std::bitset<1>(infFrame.frameHeader[2]).to_string() +
            std::bitset<1>(infFrame.frameHeader[1]).to_string() +
            std::bitset<5>(1).to_string()
    );

    rr.checksum = std::bitset<BYTE * 2>(
            std::bitset<BYTE>(0).to_string() +
            std::bitset<BYTE>(rr.data).to_string()
    );

    return rr;
}

void p7(FrameRR rr, std::vector<Block> &q, size_t n2, FreeBlocksInfo& fbi, FilledBlocksInfo& fibi){
    q.at(n2).frameHeader = rr.data;
    q.at(n2).checksum = rr.checksum;
    fibi.lastBlockAddress = std::bitset<2 * BYTE>((n2 * BLOCKSIZE - BLOCKSIZE) * BYTE);
    ++fibi.size;

    fbi.firstBlockAddress = std::bitset<2 * BYTE>((n2 + 1) * BLOCKSIZE * BYTE);
    --fbi.size;
}

void p8(std::vector<Block> &q, std::vector<Block> &kq, size_t n2, KpmBlocksInfo& kbi){
    kq.push_back(q.at(n2));
    kbi.firstBlockAddress = std::bitset<2 * BYTE>( 0);
    kbi.lastBlockAddress = std::bitset<2 * BYTE>( 0);
    kbi.size = 1;
}

void p9(FrameRR rr, int z2) {
    int nr = std::bitset<3>(
            std::bitset<1>(rr.data[7]).to_string() +
            std::bitset<1>(rr.data[6]).to_string() +
            std::bitset<1>(rr.data[5]).to_string()
    ).to_ulong();
    if (nr - 1 != z2) throw;
}

void p10(std::vector<Block> &q, std::vector<Block> &rq, size_t n2, size_t n1, FreeBlocksInfo& fbi, FilledBlocksInfo& fibi, RepeatBlocksInfo& rbi){
    q.at(n2+1) = rq.back();
    rbi.firstBlockAddress = std::bitset<2 * BYTE>( 0);
    rbi.lastBlockAddress = std::bitset<2 * BYTE>( 0);
    rbi.size = 0;
    fibi.lastBlockAddress = std::bitset<2 * BYTE>(((n2 + 1) * BLOCKSIZE - BLOCKSIZE) * BYTE);
    ++fibi.size;

    fbi.firstBlockAddress = std::bitset<2 * BYTE>((n2 + 2) * BLOCKSIZE * BYTE);
    --fbi.size;

    q.at(n2).prevBlockAddress = std::bitset<2 * BYTE>((n1 - 1) * BLOCKSIZE * BYTE);
    q.at(n2).nextBlockAddress = std::bitset<2 * BYTE>(0);
}

const void RGout(Block frame, std::vector<Block> rq) {
    std::cout << "RG register: " << std::endl;
    std::cout << "N(S): " <<
              frame.frameHeader[7] <<
              frame.frameHeader[6] <<
              frame.frameHeader[5] << std::endl;

    std::cout << "N(R): " <<
              frame.frameHeader[3] <<
              frame.frameHeader[2] <<
              frame.frameHeader[1] << std::endl;

    std::cout << "Frame type: " <<
              frame.frameHeader[0] << std::endl;

    std::cout << "Payload: ";
    for (auto i = BYTE - 1; i >= 0; --i) {
        std::cout << frame.payload[i];
    }
    std::cout << " (x" << PAYLOADSIZE << ")" << std::endl;
    std::cout << "Checksum: " << frame.checksum.to_string() << std::endl;
}

const void printResult(std::vector<Block> q, size_t n2) {
    std::cout << std::endl << "Last frame in queue (I) head: ";
    for (auto i = BYTE - 1; i >= 0; --i) {
        std::cout << q.at(n2 + 1).frameHeader[i];
    }
    std::cout << std::endl;

    Block frame = q.at(n2);
    std::cout << "Penultimate frame in queue (RR): " << std::endl;
    std::cout << "First address field: ";
    for (auto i = BYTE * 2 - 1; i >= 0; --i) {
        std::cout << frame.prevBlockAddress[i];
    }
    std::cout << std::endl;
    std::cout << "Second address field: ";
    for (auto i = BYTE * 2 - 1; i >= 0; --i) {
        std::cout << frame.nextBlockAddress[i];
    }
    std::cout << std::endl;
    std::cout << "Head: ";
    for (auto i = BYTE - 1; i >= 0; --i) {
        std::cout << frame.frameHeader[i];
    }
    std::cout << std::endl;
    std::cout << "Checksum: " << frame.checksum.to_string() << std::endl;
}

int main() {
    std::vector<Block> queue;
    std::vector<Block> repeatQueue;
    std::vector<Block> kpmQueue;
    FreeBlocksInfo fbi;
    FilledBlocksInfo fibi;
    RepeatBlocksInfo rbi;
    KpmBlocksInfo kbi;
    size_t n1 = 20;
    size_t n2 = 8;
    int z1 = 2;
    int z2 = 1;
    int m = 1;

    p1(queue, n1, fbi);
    p2(queue, n2, m);
    p3(queue, fbi, fibi, n1, n2);
    Block informationFrame = p4(queue, z1, z2);
    p5(repeatQueue, informationFrame, rbi, fibi);
    RGout(informationFrame, repeatQueue);
    FrameRR rr = p6(informationFrame);
    p7(rr, queue, n2, fbi, fibi);
    p8(queue, kpmQueue, n2, kbi);
    p9(rr, z2);
    p10(queue, repeatQueue, n2, n1, fbi, fibi, rbi);
    printResult(queue, n2);
    return 0;
}
