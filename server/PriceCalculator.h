#ifndef PRICECALCULATOR_H
#define PRICECALCULATOR_H
class PriceCalculator {
public:
    static double calculateEffectivePercent(double directPercent, double timedPercent);
    static double calculateFinalPrice(double basePrice, double effectivePercent, double discountAmount);
    static inline double calculateItemFinalPrice(double basePrice, double percent, double amount) {
        return calculateFinalPrice(basePrice, percent, amount);
    }
};

#endif // PRICECALCULATOR_H
