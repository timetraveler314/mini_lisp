//
// Created by timetraveler314 on 5/3/24.
//

#include <vector>
#include <cassert>
#include <iostream>

#include "../src/value.h"
#include "../src/token.h"
#include "../src/tokenizer.h"

void testFromVector() {
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
}

void testParser() {
    // Tests in Lv2 tests
}

int main() {
    std::cout << "Running tests..." << std::endl;
    testFromVector();
    testParser();

    std::cout << "All tests passed!" << std::endl;
}