//
// Created by timetraveler314 on 5/3/24.
//

#include <vector>
#include <cassert>
#include <iostream>
#include <random>

#include "../src/value.h"
#include "../src/token.h"
#include "../src/tokenizer.h"
#include "../src/error.h"

void testValueToString() {
    // NumericValue
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    for (int i = 0; i < 10000; ++i) {
        int rand_int = dis(gen);
        auto value = NumericValue(static_cast<double>(rand_int));
        assert(value.toString() == std::to_string(rand_int));
    }
}

void testValueVector() {
    // Value::fromVector
    // Empty
    std::vector<ValuePtr> emptyVector;
    ValuePtr resultEmpty = Value::fromVector(emptyVector);
    assert(resultEmpty->getType() == ValueType::NIL_VALUE);

    // Non-empty
    std::vector<ValuePtr> nonEmptyVector = {
        std::make_shared<NumericValue>(1.0),
        std::make_shared<BooleanValue>(true),
        std::make_shared<StringValue>("hello"),
        std::make_shared<SymbolValue>("world"),
    };
    ValuePtr resultNonEmpty = Value::fromVector(nonEmptyVector);
    assert(resultNonEmpty->getType() == ValueType::PAIR_VALUE);
    assert(resultNonEmpty->toString() == "(1 #t \"hello\" world)");

    // Value::toVector
    // NilValue.toVector() should return an empty vector
    auto nilResult = std::make_shared<NilValue>()->toVector();
    assert(nilResult.empty());

    // toVector . fromVector = id
    auto pairResult = resultNonEmpty->toVector();
    assert(pairResult.size() == 4);
    assert(pairResult[0]->asNumber() == 1.0);
    assert(pairResult[1]->getType() == ValueType::BOOLEAN_VALUE);

    // Malformed list should throw
    try {
        auto malformedList = std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0));
        malformedList->toVector();
        assert(false && "Malformed list should throw");
    } catch (const LispError& e) {
        assert(true);
    } catch (...) {
        assert(false && "Malformed list should throw a LispError");
    }
}

void testParser() {
    // Tests in Lv2 tests
}

int main() {
    std::cout << "Running tests..." << std::endl;
    testValueToString();
    testValueVector();
    testParser();

    std::cout << "All tests passed!" << std::endl;
}