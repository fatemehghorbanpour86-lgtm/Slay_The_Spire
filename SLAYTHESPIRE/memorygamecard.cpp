#include "memorygamecard.h"
MemoryCard::MemoryCard()
    : imageId(-1), matched(false), faceUp(false)
{
}

MemoryCard::MemoryCard(int imageId)
    : imageId(imageId), matched(false), faceUp(false)
{
}

int MemoryCard::getImageId() const
{
    return imageId;
}

void MemoryCard::setImageId(int id)
{
    imageId = id;
}

bool MemoryCard::isMatched() const
{
    return matched;
}

void MemoryCard::setMatched(bool value)
{
    matched = value;
}

bool MemoryCard::isFaceUp() const
{
    return faceUp;
}

void MemoryCard::setFaceUp(bool value)
{
    faceUp = value;
}