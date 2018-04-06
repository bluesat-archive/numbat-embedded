#ifndef MESSAGE_H
#define MESSAGE_H

#define NUM_MSG 8

enum messages {
    FLD,
    FRD,
    BLD,
    BRD,
    FLS,
    FRS,
    BLS,
    BRS
};

struct message {
    double data[NUM_MSG];
    uint32_t endMagic;
};

struct messageAdapter {
    union _data {
        struct message msg;
        uint8_t structBytes[sizeof(struct message)];
    } data;
};

const uint8_t startMagic[4] = {0xFE, 0xED, 0xBE, 0xEF};
const uint32_t endMagic = 0xDEADBEEF;

#endif
