#ifndef NPROFILE

#include "util.h"
#include "string.h"

#define FRT 0xfffffe10
#define TCR 6
#define TIER 0
#define TCSR 1
#define FRT_H 2
#define FRT_L 3

void setFastTimer(void)
{ /* set timer to cycles/32 */
    POKE_B(FRT + TCR, (PEEK_B(FRT + TCR) & ~3) | 1);
    /* turn off interrupts */
    POKE_B(FRT + TIER, PEEK_B(FRT + TIER) & ~8);
    /* turn off compare/reset & zero overflow bit */
    POKE_B(FRT + TCSR, PEEK_B(FRT + TCSR) & ~1);
    /* clear frt */
    POKE_B(FRT + FRT_H, 0);
    POKE_B(FRT + FRT_L, 0);
}

uint16 getTimer(void)
{
    uint8 h, l;
    h = PEEK_B(FRT + FRT_H);
    l = PEEK_B(FRT + FRT_L);
    return (h << 8) | l;
}

#define MAXNMCHILDREN 8

typedef struct _node
{
    char* id;
    uint32 totalTime, lastReportedTime;
    struct _node* child[MAXNMCHILDREN];
    struct _node* parent;
    sint32 nmChildren;
} ProfileNode;

#define MAXNMNODES 60
static ProfileNode nodes[MAXNMNODES];

static sint32 nmNodes;
static sint32 level;
static uint16 lastTime;
ProfileNode* currentNode;

void initProfiler(void)
{
    sint32 i;
    setFastTimer();
    level = 0;

    for (i = 0; i < MAXNMNODES; i++)
    {
        nodes[i].nmChildren = 0;
        nodes[i].totalTime = 0;
        nodes[i].parent = NULL;
        nodes[i].lastReportedTime = 0;
    }

    nmNodes = 1;
    currentNode = nodes;
    currentNode->id = "root";
    lastTime = getTimer();
}

void pushProfile(char* id)
{
    sint32 i;
    for (i = 0; i < currentNode->nmChildren; i++)
        if (currentNode->child[i]->id == id)
            break;

    currentNode->totalTime += (getTimer() - lastTime) & 0xffff;

    if (i == currentNode->nmChildren)
    {
        assert(nmNodes < MAXNMNODES);
        assert(currentNode->nmChildren < MAXNMCHILDREN);
        currentNode->child[currentNode->nmChildren++] = nodes + nmNodes;
        nodes[nmNodes].parent = currentNode;
        nodes[nmNodes].id = id;
        currentNode = nodes + nmNodes;
        nmNodes++;
    }
    else
        currentNode = currentNode->child[i];

    lastTime = getTimer();
}

void popProfile(void)
{
    currentNode->totalTime += (getTimer() - lastTime) & 0xffff;
    currentNode = currentNode->parent;
    assert(currentNode);
    lastTime = getTimer();
}

static uint32 sumChildren(ProfileNode* node)
{
    sint32 i;
    uint32 tot;
    tot = node->totalTime;
    for (i = 0; i < node->nmChildren; i++)
        tot += sumChildren(node->child[i]);
    return tot;
}

static void printTree(ProfileNode* tree, sint32 level, uint32 parentTotal, uint32 parentDifference)
{
    sint32 i;
    char buff[80];
    uint32 difPercent, totPercent;
    uint32 sum;
    for (i = 0; i < level; i++)
        debugPrint(" ");
    sum = sumChildren(tree) >> 10;
    if (parentTotal < 1)
        totPercent = 0;
    else
        totPercent = (1000 * sum) / parentTotal;

    if (parentDifference < 1)
        difPercent = 0;
    else
        difPercent = (1000 * (sum - tree->lastReportedTime)) / parentDifference;

    sprintf(buff, "%s : %d (%d)\n", tree->id, difPercent, totPercent);
    debugPrint(buff);
    for (i = 0; i < tree->nmChildren; i++)
        printTree(tree->child[i], level + 1, sum, sum - tree->lastReportedTime);

    tree->lastReportedTime = sum;
}

void dumpProfileData(void)
{
    uint32 sum;
    debugPrint("\n\n");
    sum = sumChildren(nodes) >> 10;
    printTree(nodes, 0, sum, sum - nodes->lastReportedTime);
}

#endif
