#include "unity.h"
#include "xi2c.h"

void test_i2c(void)
{
	TEST_ASSERT_EQUAL_HEX8(0, xi2c_test());
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_i2c);
	return UNITY_END();
}
