#include "unity.h"
#include "xuart.h"

void test_xuart(void)
{
	TEST_ASSERT_EQUAL_HEX8(1, xuart_echo(1));
	TEST_ASSERT_EQUAL_HEX8(17, xuart_echo(17));
	TEST_ASSERT_EQUAL_HEX8(42, xuart_echo(42));
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_xuart);
	return UNITY_END();
}
