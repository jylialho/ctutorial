#include "protocol.h"

int main()
{
    int result = report_stdout(REPORT_INTERVAL_100MS, CONTROL_DISABLED);
    return result;
}
