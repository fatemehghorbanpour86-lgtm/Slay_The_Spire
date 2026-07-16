#include "player.h"
#include "combatdeck.h"
#include "potion.h"
#include "masterdeck.h"
#include "attackcards.h"
#include "skillcards.h"

Player::Player(const QString& name, int maxHealth)
    : Character(name, maxHealth),
    currentEnergy(3),
    maxEnergy(3),
    gold(99),
    cardRemovalCost(50),
    masterDeck(new MasterDeck()),
    combatDeck(new CombatDeck()),
    relicSystem(new RelicSystem())
{
    for(int i = 0; i < 5; ++i)
    {
        masterDeck->addCard(new Strike());
    }
    for(int i = 0; i < 4; ++i)
    {
        masterDeck->addCard(new Defend());
    }
    masterDeck->addCard(new Carnage());
    relicSystem.addRelic(new BurningBlood(), this);
}

Player::~Player()
{
    delete combatDeck;
    delete masterDeck;

    qDeleteAll(potions);
    potions.clear();
}


int Player::getCurrentEnergy() const
{
    return currentEnergy;
}


int Player::getMaxEnergy() const
{
    return maxEnergy;
}


void Player::setEnergy(int value)
{
    if(value < 0)
        value = 0;

    currentEnergy = value;
}


void Player::gainEnergy(int amount)
{
    if(amount <= 0)
        return;

    setEnergy(currentEnergy + amount);
}

// for playing cards
bool Player::useEnergy(int amount)
{
    if(amount <= 0)
        return true;

    if(currentEnergy < amount)
        return false;

    currentEnergy -= amount;

    return true;
}


void Player::resetEnergy()
{
    currentEnergy = maxEnergy;

    // TODO (Anahita)
    // Apply card effects that change Energy.


    // TODO (Fateme)
    // Apply Relic effects that grant Energy.
}


int Player::getGold() const
{
    return gold;
}


void Player::gainGold(int amount)
{
    if(amount <= 0)
        return;

    gold += amount;
}

// for shop
bool Player::spendGold(int amount)
{
    if(amount <= 0)
        return true;

    if(gold < amount)
        return false;

    gold -= amount;

    return true;
}

void Player::increaseMaxHealth(int amount)
{
    if(amount <= 0)
        return;

    maxHealth += amount;
    currentHealth += amount;

    if(currentHealth > maxHealth)
    {
        currentHealth = maxHealth;
    }
}

void Player::decreaseMaxHealth(int amount)
{
    if (amount <= 0)
        return;

    maxHealth -= amount;

    if (maxHealth < 1)
        maxHealth = 1;

    if (currentHealth > maxHealth)
        currentHealth = maxHealth;
}

void Player::increaseMaxEnergy(int amount)
{
    if(amount <= 0)
        return;

    maxEnergy += amount;
}

void Player::addPotion(Potion* potion)
{
    if (potion == nullptr)
        return;

    potions.append(potion);
}

bool Player::removePotion(Potion* potion)
{
    int index = potions.indexOf(potion);

    if(index == -1)
        return false;

    delete potions[index];
    potions.removeAt(index);

    return true;
}


Potion* Player::getPotion(int index) const
{
    if(index < 0 || index >= potions.size())
        return nullptr;

    return potions[index];
}


int Player::getPotionCount() const
{
    return potions.size();
}

const QVector<Potion*>& Player::getPotions() const
{
    return potions;
}

void Player::addRelic(Relic* relic)
{
    relicSystem.addRelic(relic, this);
}

bool Player::hasRelic(RelicId id) const
{
    return relicSystem.hasRelic(id);
}

Relic* Player::getRelic(RelicId id) const
{
    return relicSystem.getRelic(id);
}

const QVector<Relic*>& Player::getAllRelics() const
{
    return relicSystem.getAllRelics();
}

bool Player::canRest() const
{
    return relicSystem.canRest();
}

bool Player::canUsePotions() const
{
    return relicSystem.canUsePotions();
}

bool Player::canPlayCard()
{
    return relicSystem.canPlayCard(this);
}

RelicSystem& Player::getRelicSystem()
{
    return relicSystem;
}

const RelicSystem& Player::getRelicSystem() const
{
    return relicSystem;
}


bool Player::canLift() const
{
    return relicSystem.canLift();
}

bool Player::lift()
{
    return relicSystem.lift(this);
}

CombatDeck* Player::getCombatDeck()
{
    return combatDeck;
}

const CombatDeck* Player::getCombatDeck() const
{
    return combatDeck;
}


 MasterDeck* Player::getMasterDeck()
{
    return masterDeck;
}

const MasterDeck* Player::getMasterDeck() const
{
    return masterDeck;
}

void Player::prepareForCombat()
{
    if (masterDeck && combatDeck)
    {
        combatDeck->initializeFromMasterDeck(*masterDeck);
    }
}

Card* Player::drawCard()
{
    return combatDeck->drawCard();
}

void Player::drawCards(int count)
{
    combatDeck->drawCards(count);
}

void Player::discardHand()
{
    combatDeck->discardHand();
}

bool Player::discardCard(Card* card)
{
    return combatDeck->moveFromHandToDiscard(card);
}

bool Player::exhaustCard(Card* card)
{
    return combatDeck->moveFromHandToExhaust(card);
}

bool Player::moveFromExhaustToHand(Card* card)
{
    return combatDeck->moveFromExhaustToHand(card);
}

void Player::addCardToDiscardPile(Card* card)
{
    if (combatDeck)
    {
        combatDeck->addCardToDiscardPile(card);
    }
}

Card* Player::getRandomHandCard()
{
    if (!combatDeck)
        return nullptr;

    return combatDeck->getRandomCardFromHand();
}

bool Player::exhaustRandomCardFromHand()
{
    if (!combatDeck)
        return false;

    return combatDeck->exhaustRandomCardFromHand();
}

PlayerSaveData Player::extractState() const
{
    PlayerSaveData data;

    data.characterData = Character::extractState();
    data.gold = gold;
    data.maxEnergy = maxEnergy;
    data.cardRemovalCost = cardRemovalCost;
    data.relics = relicSystem.extractState();

    if (masterDeck)
        data.masterDeckData = masterDeck->extractState();

    return data;
}


int Player::getCardRemovalCost() const
{
    return cardRemovalCost;
}
void Player::increaseCardRemovalCost(int amount)
{
    if (amount <= 0) return;
    cardRemovalCost += amount;
}

void Player::restoreState(const PlayerSaveData& data)
{
    Character::restoreState(data.characterData);

    gold = data.gold;
    maxEnergy = data.maxEnergy;
    cardRemovalCost = data.cardRemovalCost;

    relicSystem.restoreState(data.relics, this);

    if (masterDeck)
        masterDeck->restoreState(data.masterDeckData);
}
