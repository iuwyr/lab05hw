#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Transaction.h"
#include "Account.h"

using ::testing::Return;
using ::testing::_;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}

    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
    MOCK_METHOD(int, id, (), (const));
};

class TestTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account&, Account&, int), (override));
};

TEST(TransactionTest, ErrorOnSameAccount) {
    MockAccount acc(1, 1000);
    TestTransaction txn;
    EXPECT_CALL(acc, id()).WillRepeatedly(Return(1));
    EXPECT_THROW(txn.Make(acc, acc, 100), std::logic_error);
}

TEST(TransactionTest, ErrorOnNegativeAmount) {
    MockAccount from(1, 1000), to(2, 1000);
    TestTransaction txn;
    ON_CALL(from, id()).WillByDefault(Return(1));
    ON_CALL(to, id()).WillByDefault(Return(2));
    EXPECT_THROW(txn.Make(from, to, -10), std::invalid_argument);
}

TEST(TransactionTest, ErrorOnTooSmallAmount) {
    MockAccount from(1, 1000), to(2, 1000);
    TestTransaction txn;
    ON_CALL(from, id()).WillByDefault(Return(1));
    ON_CALL(to, id()).WillByDefault(Return(2));
    EXPECT_THROW(txn.Make(from, to, 50), std::logic_error);
}

TEST(TransactionTest, FailIfFeeTooBig) {
    MockAccount from(1, 1000), to(2, 1000);
    TestTransaction txn;
    txn.set_fee(100);
    ON_CALL(from, id()).WillByDefault(Return(1));
    ON_CALL(to, id()).WillByDefault(Return(2));
    EXPECT_FALSE(txn.Make(from, to, 100));
}

TEST(TransactionTest, SuccessfulTransfer) {
    MockAccount from(1, 1000), to(2, 1000);
    TestTransaction txn;

    EXPECT_CALL(from, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(2));

    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, Unlock());
    EXPECT_CALL(to, Unlock());

    EXPECT_CALL(to, ChangeBalance(100));
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(1100));
    EXPECT_CALL(to, ChangeBalance(-101));

    EXPECT_CALL(txn, SaveToDataBase(_, _, 100));

    EXPECT_TRUE(txn.Make(from, to, 100));
}

TEST(TransactionTest, RollbackIfNotEnoughBalance) {
    MockAccount from(1, 1000), to(2, 1000);
    TestTransaction txn;

    EXPECT_CALL(from, id()).WillRepeatedly(Return(1));
    EXPECT_CALL(to, id()).WillRepeatedly(Return(2));

    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, Unlock());
    EXPECT_CALL(to, Unlock());

    EXPECT_CALL(to, ChangeBalance(100));
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(50));
    EXPECT_CALL(to, ChangeBalance(-100));

    EXPECT_CALL(txn, SaveToDataBase(_, _, 100));

    EXPECT_FALSE(txn.Make(from, to, 100));
}
