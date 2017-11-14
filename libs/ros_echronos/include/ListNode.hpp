//
// Created by ros on 27/05/17.
//

#ifndef NUMBAT_EMBEDDED_LISTNODE_H
#define NUMBAT_EMBEDDED_LISTNODE_H

#include "ros.hpp"

class ListNode {
    public:
        ListNode * next = NULL;
        ListNode * prev = NULL;
};


#endif //NUMBAT_EMBEDDED_LISTNODE_H
