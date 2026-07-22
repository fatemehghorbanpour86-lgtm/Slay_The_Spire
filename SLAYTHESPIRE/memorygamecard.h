#ifndef MEMORYGAMECARD_H
#define MEMORYGAMECARD_H


class MemoryCard
{
public:
    MemoryCard();
    explicit MemoryCard(int imageId);

    // Identifies which of the TOTAL_PAIRS images this card shows on its
    // front face. Two cards sharing the same imageId form a matching pair.
    int getImageId() const;
    void setImageId(int id);

    // True once this card has been successfully paired with its twin.
    // Matched cards stay face-up permanently and can no longer be clicked.
    bool isMatched() const;
    void setMatched(bool value);

    // True while this card is currently showing its front image
    // (either mid-comparison or already matched).
    bool isFaceUp() const;
    void setFaceUp(bool value);

private:
    int imageId;
    bool matched;
    bool faceUp;
};

#endif // MEMORYGAMECARD_H
