#include "PriceCalculator.h"
double PriceCalculator::calculateEffectivePercent(double directPercent, double timedPercent) {
    return directPercent > timedPercent ? directPercent : timedPercent;
}
double PriceCalculator::calculateFinalPrice(double basePrice, double effectivePercent, double discountAmount) {
    if (basePrice < 0) basePrice = 0;
    if (effectivePercent < 0) effectivePercent = 0;
    if (effectivePercent > 100) effectivePercent = 100;
    if (discountAmount < 0) discountAmount = 0;
    double afterPercent = basePrice - (basePrice * effectivePercent / 100);
    double final = afterPercent - discountAmount;
    return final < 0 ? 0 : final;
}